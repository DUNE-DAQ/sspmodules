/**
 * @file SSPLEDCalibWrapper.cpp SSP library wrapper implementation
 *
 * This is part of the DUNE DAQ , copyright 2022.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_SSPLEDCALIBWRAPPER_CPP_
#define SSPMODULES_SRC_SSPLEDCALIBWRAPPER_CPP_

// From Module
#include "SSPLEDCalibWrapper.hpp"
/**
 * @brief TRACE debug levels used in this source file
 */
#define TRACE_NAME "SSPLEDCalibWrapper" // NOLINT

enum
{
  TLVL_ENTER_EXIT_METHODS = 5,
  TLVL_WORK_STEPS = 10,
  TLVL_BOOKKEEPING = 15,
  TLVL_FULL_DEBUG = 63
};

namespace dunedaq {
namespace sspmodules {

SSPLEDCalibWrapper::SSPLEDCalibWrapper()
  : m_device_interface(0)
  , m_run_marker{ false }
{
}

SSPLEDCalibWrapper::~SSPLEDCalibWrapper()
{
}

void
SSPLEDCalibWrapper::init(const dal::SSPCalibModule* conf)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::init called." << std::endl;

  m_device_interface = new dunedaq::sspmodules::DeviceInterface();

  m_number_channels = conf->get_number_channels();
  m_channel_mask = conf->get_channel_mask();
  m_burst_count = conf->get_burst_count();
  m_double_pulse_delay_ticks = conf->get_double_pulse_delay_ticks();
  m_pulse1_width_ticks = conf->get_pulse1_width_ticks();
  m_pulse2_width_ticks = conf->get_pulse2_width_ticks();
  m_pulse_bias_percent_270nm = conf->get_pulse_bias_percent_270nm();
  m_pulse_bias_percent_367nm = conf->get_pulse_bias_percent_367nm();

  m_board_id = conf->get_board_id();
  m_module_id = conf->get_module_id();
  m_instance_name_for_metrics = "SSP LED Calib " + std::to_string(m_board_id);
  m_partition_number =conf->get_partition_number(); // this should be 0-3

  m_timing_address = conf->get_timing_address(); // 0x20 is default for 101, 0x2B for 304, and 0x36 for 603
  if (m_timing_address > 0xff) {
    std::stringstream ss;
    ss << "Error: Incorrect timing address set (" << m_timing_address << ")!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Board ID is listed as: " << m_board_id << std::endl
                              << "Partition Number is: " << m_partition_number << std::endl
                              << "Timing Address is: " << m_timing_address << std::endl
                              << "Module ID is: " << m_module_id << std::endl;

  m_device_interface->SetPartitionNumber(m_partition_number);
  m_device_interface->SetTimingAddress(m_timing_address);
  m_device_interface->ConfigureLEDCalib(conf); //This sets up the ethernet interface and make sure that the pdts is synched
  m_device_interface->SetRegisterByName("module_id", m_module_id);
  //m_device_interface->SetRegisterByName("eventDataInterfaceSelect", m_cfg.interface_type);

  if ( conf->get_pulse_mode() == "single") {
    m_single_pulse = true;
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: I think that you want SSP LED Calib module to be in single pulse..." << std::endl;
  } else if ( conf->get_pulse_mode() == "burst") {
    m_burst_mode = true;
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: I think that you want SSP LED Calib module to be in BURST MODE..." << std::endl;
  } 

  if ( (m_single_pulse && m_burst_mode) ) {
    std::stringstream ss;
    ss << "ERROR: SOMEHOW ENDED UP WITH SEVERAL PULSE MODES SET ON!!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
    
  if (m_burst_mode) {
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Configuring for BURST MODE..." << std::endl;
    this->configure_burst_mode();
  } else if (m_single_pulse) {
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Configuring for single pulse mode..." << std::endl;
    this->configure_single_pulse();
  } else {
    std::stringstream ss;
    ss << "ERROR: SOMEHOW ENDED UP WITH NO PULSE MODES SET ON!!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }  

  //if there are "literal" entries in the configuration they are explicit writes to the specified register with given value
  //these literal entries are paresed and applied last after any other parameters so this method call needs to be after the
  //other configuration calls
  this->manual_configure_device(conf->get_hardware_configuration());

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::configure complete.";
}

void
SSPLEDCalibWrapper::start(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Start pulsing SSPLEDCalibWrapper of card " << m_board_id << "...";

  if (m_run_marker.load()) {
    TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Run Marker says that SSPLEDCalibWrapper card " << m_board_id << " is already pulsing...";
    return;
  }

  unsigned int base_bias_regAddress = 0x40000340;
  unsigned int base_timing_regAddress = 0x800003C0;

  if (m_number_channels == 5) {
    base_bias_regAddress = 0x4000035C;
    base_timing_regAddress = 0x800003DC;
    TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Configuring board with 5 channels..." ;
  } else if (m_number_channels == 12) {
    TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Configuring board with 12 channels..." ;
  } else {    
    std::stringstream ss;
    ss << "ERROR: SOMEHOW ENDED UP WITHOUT NUMBER OF CHANNELS SET!!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  unsigned int pulse_bias_setting_270nm = ( m_pulse_bias_percent_270nm);
  unsigned int pulse_bias_setting_367nm = ( m_pulse_bias_percent_367nm/2);
  
  for (unsigned int counter = 0; counter < m_number_channels ; counter++) {
    unsigned int bias_regAddress =  base_bias_regAddress + 0x4*(counter);  //0x40000340 - 0x4000036C
    unsigned int bias_regVal = 0x00040000;
    unsigned int timing_regAddress =  base_timing_regAddress + 0x4*(counter); //0x80003C0 - 0x800003EC
    unsigned int timing_regVal = 0x0; //the highest byte sets 8 (pdts trigger) + 1 (953.5 Hz) for burst mode, but 8 (pdts trigger) + 7 (single shot) for single
    if (m_single_pulse) {
      timing_regVal = 0xF0000000;
    } else if (m_burst_mode) {
      timing_regVal = 0x90000000;
    }
      
    TLOG(TLVL_FULL_DEBUG) << "Channel map is 0x" << std::hex << m_channel_mask << " and the comparison is 0x" << (1 << counter ) << std::endl;
    if ( (m_channel_mask & ( (unsigned int)1 << counter)) == ((unsigned int)1 << counter) ) {      
      if ( counter < 6) {
	bias_regVal = bias_regVal + pulse_bias_setting_270nm;
      } else {
	bias_regVal = bias_regVal + pulse_bias_setting_367nm;
      }
      timing_regVal = timing_regVal + m_pulse1_width_ticks;
      timing_regVal = timing_regVal + (m_pulse2_width_ticks << 8);
      timing_regVal = timing_regVal + (m_double_pulse_delay_ticks << 16);
      TLOG(TLVL_FULL_DEBUG) << "Will turn on " << std::dec << counter << " channel at bias register 0x" << std::hex << bias_regAddress << " with bias value 0x" << bias_regVal << std::endl;
      TLOG(TLVL_FULL_DEBUG) << " and set the width regsiter 0x" << std::hex << timing_regAddress << " to value of 0x" << timing_regVal << std::dec << std::endl;
      m_device_interface->SetRegister(bias_regAddress, bias_regVal); //BIAS_DAC_CONFIG_N
      if ( (counter == 7) && ! ( (m_channel_mask & ( (unsigned int)1 << counter)) == ((unsigned int)1 << (counter-1) ) ) )  {
	//this is a really convoluted situation where for the very specific 12 channel board that arrived at CERN in June 2022
	//the bias for channel 7 was not working and so the bias is taken from channel 6
	//which means that if channel 7 is to be turned on while channel 6 is masked off
	//you still have to bias channel 6 in order for the led on channel 7 to emmit light
	m_device_interface->SetRegister((bias_regAddress - 0x4), bias_regVal); //BIAS_DAC_CONFIG_N
      }
	
      m_device_interface->SetRegister(timing_regAddress, timing_regVal); //cal_CONFIG_N
    } else {
      TLOG(TLVL_FULL_DEBUG) << "Will turn off channel " << std::dec << counter << " at timing register 0x" << std::hex << timing_regAddress << std::dec << std::endl;
      m_device_interface->SetRegister(bias_regAddress, 0x0); //BIAS_DAC_CONFIG_N
      m_device_interface->SetRegister(timing_regAddress, 0x0); //cal_CONFIG_N
    }
  }

  m_device_interface->SetRegister(0x40000300, 0x1); //writing 0x1 to this register applies the bias voltage settings

  m_run_marker = true;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Start pulsing SSPLEDCalibWrapper of card " << m_board_id << " complete.";
}

void
SSPLEDCalibWrapper::stop(const data_t& /*args*/)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stop pulsing SSPLEDCalibWrapper of card " << m_board_id << "...";
  if (!m_run_marker.load()) {
    TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "The run_marker says that SSPLEDCalibWrapper card " << m_board_id << " is already stopped, but stopping anyways...";
  }

  for (unsigned int counter = 0; counter < 12; counter++) { //switch this to 12 for a 12 channel SSP
    unsigned int bias_regAddress =  0x40000340 + 0x4*(counter);
    unsigned int timing_regAddress =  0x800003c0 + 0x4*(counter);
    m_device_interface->SetRegister(bias_regAddress, 0x00000000); //BIAS_DAC_CONFIG_N
    m_device_interface->SetRegister(timing_regAddress, 0x00000000); //cal_CONFIG_N
  }

  m_device_interface->SetRegister(0x40000300, 0x1); //writing 0x1 to this register applies the bias voltage settings

  m_run_marker = false;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stop pulsing SSPLEDCalibWrapper of card " << m_board_id << " complete.";
}

void
SSPLEDCalibWrapper::configure_single_pulse()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse called.";

  m_device_interface->SetRegister(0x80000464, 0x000002E7); //pdts_cmd_control_1
  m_device_interface->SetRegister(0x80000940, 0x00030036); //pdts_cmd_delay_0
  m_device_interface->SetRegister(0x80000944, 0x00030036); //pdts_cmd_delay_1
  m_device_interface->SetRegister(0x80000948, 0x00030036); //pdts_cmd_delay_2
  m_device_interface->SetRegister(0x8000094C, 0x00030036); //pdts_cmd_delay_3
  m_device_interface->SetRegister(0x80000950, 0x00030036); //pdts_cmd_delay_4
  m_device_interface->SetRegister(0x80000954, 0x00030036); //pdts_cmd_delay_5
  m_device_interface->SetRegister(0x80000958, 0x00030036); //pdts_cmd_delay_6
  m_device_interface->SetRegister(0x8000095C, 0x00030036); //pdts_cmd_delay_7
  m_device_interface->SetRegister(0x80000960, 0x00030036); //pdts_cmd_delay_8
  m_device_interface->SetRegister(0x80000964, 0x00030036); //pdts_cmd_delay_9
  m_device_interface->SetRegister(0x80000968, 0x00030036); //pdts_cmd_delay_10
  m_device_interface->SetRegister(0x8000096C, 0x00030036); //pdts_cmd_delay_11
  m_device_interface->SetRegister(0x80000970, 0x00030036); //pdts_cmd_delay_12
  m_device_interface->SetRegister(0x80000974, 0x00030036); //pdts_cmd_delay_13
  m_device_interface->SetRegister(0x80000978, 0x00030036); //pdts_cmd_delay_14
  m_device_interface->SetRegister(0x8000097C, 0x00030036); //pdts_cmd_delay_15
  m_device_interface->SetRegister(0x80000468, 0x80000000); //pdts_cmd_control_2
  m_device_interface->SetRegister(0x80000520, 0x00000011); //pulser_mode_control
  m_device_interface->SetRegister(0x80000448, 0x00000001); //cal_count
  
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse completed.";
}

void
SSPLEDCalibWrapper::configure_burst_mode()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureBurstMode called.";

  m_device_interface->SetRegister(0x80000464, 0x000002E7); //pdts_cmd_control_1
  m_device_interface->SetRegister(0x80000940, 0x00030036); //pdts_cmd_delay_0
  m_device_interface->SetRegister(0x80000944, 0x00030036); //pdts_cmd_delay_1
  m_device_interface->SetRegister(0x80000948, 0x00030036); //pdts_cmd_delay_2
  m_device_interface->SetRegister(0x8000094C, 0x00030036); //pdts_cmd_delay_3
  m_device_interface->SetRegister(0x80000950, 0x00030036); //pdts_cmd_delay_4
  m_device_interface->SetRegister(0x80000954, 0x00030036); //pdts_cmd_delay_5
  m_device_interface->SetRegister(0x80000958, 0x00030036); //pdts_cmd_delay_6
  m_device_interface->SetRegister(0x8000095C, 0x00030036); //pdts_cmd_delay_7
  m_device_interface->SetRegister(0x80000960, 0x00030036); //pdts_cmd_delay_8
  m_device_interface->SetRegister(0x80000964, 0x00030036); //pdts_cmd_delay_9
  m_device_interface->SetRegister(0x80000968, 0x00030036); //pdts_cmd_delay_10
  m_device_interface->SetRegister(0x8000096C, 0x00030036); //pdts_cmd_delay_11
  m_device_interface->SetRegister(0x80000970, 0x00030036); //pdts_cmd_delay_12
  m_device_interface->SetRegister(0x80000974, 0x00030036); //pdts_cmd_delay_13
  m_device_interface->SetRegister(0x80000978, 0x00030036); //pdts_cmd_delay_14
  m_device_interface->SetRegister(0x8000097C, 0x00030036); //pdts_cmd_delay_15
  m_device_interface->SetRegister(0x80000468, 0x80000000); //pdts_cmd_control_2
  m_device_interface->SetRegister(0x80000520, 0x00000011); //pulser_mode_control
  m_device_interface->SetRegister(0x80000448, m_burst_count); //cal_count
  
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureBurstMode completed.";
}

void
SSPLEDCalibWrapper::manual_configure_device(const std::vector<const dal::SSPRegister*>& hw_conf)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDevice called.";
  TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Processing the Hardware Configuration list..." << std::endl;
  for (auto regValuesIter : hw_conf) {
    std::string m_name = regValuesIter->get_name();
    unsigned int regAddr = regValuesIter->get_address();
    unsigned int regVal = regValuesIter->get_value();
    unsigned int regMask = regValuesIter->get_mask();
    m_device_interface->SetRegister(regAddr, regVal, regMask);
  }
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDevice complete.";
} // NOLINT(readability/fn_size)

void
SSPLEDCalibWrapper::validate_config(const data_t& /*args*/  )
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::validate_config called.";

  if ( ! ( (m_number_channels == 5) || (m_number_channels == 12) ) ) {
    std::stringstream ss;
    ss << "ERROR: Incorrect number_channels value " << m_number_channels << " is not equal to 5 or 12!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (m_channel_mask > 4095) {
    std::stringstream ss;
    ss << "ERROR: Incorrect channel_maks value " << m_channel_mask << " is higher than the limit of 4095!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (!( m_single_pulse || m_burst_mode ) ) {
    std::stringstream ss;
    ss << "ERROR: Incorrect pulse_mode value. Neither single or burst."
       << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  if (m_double_pulse_delay_ticks > 4095) {
    std::stringstream ss;
    ss << "ERROR: Strange!! double_pulse_delay_ticks value is " << m_double_pulse_delay_ticks << ", which is greater than the limit of 4095"
       << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (m_burst_count > 10000) {
    std::stringstream ss;
    ss << "ERROR: Strange!! burst_count value is " << m_burst_count << ", which is more time than in a drift readout window"
       << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (m_pulse1_width_ticks > 255) {
    std::stringstream ss;
    ss << "ERROR: Incorrect pulse1_width_ticks value is " << m_pulse1_width_ticks << ", which is greater than the limit of 255!!!"
       << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

    if (m_pulse2_width_ticks > 255) {
    std::stringstream ss;
    ss << "ERROR: Incorrect pulse2_width_ticks value is " << m_pulse2_width_ticks << ", which is greater than the limit of 255!!!"
       << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (m_pulse_bias_percent_270nm > 4095) {
    std::stringstream ss;
    ss << "ERROR: Incorrect pulse_bias_percent_270nm value is " << m_pulse_bias_percent_270nm << ", which is greater than 100 percent!!!"
       << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  if (m_pulse_bias_percent_367nm > 4095) {
    std::stringstream ss;
      ss << "ERROR: Incorrect pulse_bias_percent_367nm value is " << m_pulse_bias_percent_367nm << ", which is greater than 100 percent!!!"
	 << std::endl;
      TLOG() << ss.str();
      throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::validate_config complete.";
}

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_SSPLEDCALIBWRAPPER_CPP_
