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

// From STD
#include <chrono>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>

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
    //  , m_ssp_processor(0)
    , m_run_marker{ false }
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper constructor called." << std::endl;
  TLOG_DEBUG(TLVL_FULL_DEBUG)
    << "Constructor doesn't actually do anything but initialize conf paramters to none function values." << std::endl;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper constructor complete." << std::endl;
}

SSPLEDCalibWrapper::~SSPLEDCalibWrapper()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper destructor called." << std::endl;
  //close_card();
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper destructor complete." << std::endl;
}

void
SSPLEDCalibWrapper::init(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::init called." << std::endl;
  // REMEMBER: the init method isn't supposed to do any frontend board configuration in the new DUNE-DAQ framework
  //          ALL of the frontend board configuration is to be done in the conf call and it is likely that the
  //          configuration parameters that you're looking for in args aren't available since the args you're
  //          getting here is likely only *::Init data from the json file

  m_device_interface = new dunedaq::sspmodules::DeviceInterface(dunedaq::detdataformats::ssp::kEthernet);
  m_device_interface->Initialize(args);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::init complete.";
}

void
SSPLEDCalibWrapper::configure(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::configure called.";

  m_cfg = args.get<dunedaq::sspmodules::sspledcalibmodule::Conf>();
  if (m_cfg.board_ip == "default") {
    TLOG() << "SSPLEDCalibWrapper::configure: This Board IP value in the Conf is set to: default" << std::endl
           << "As we currently only deal with SSPs on ethernet, this means that either the Board IP was " << std::endl
           << "NOT set, or the args.get<Conf> call failed to find parameters." << std::endl;
    throw ConfigurationError(ERS_HERE, "Used default Board IP value");
  }

  m_board_id = m_cfg.board_id;
  m_instance_name_for_metrics = "SSP LED Calib " + std::to_string(m_board_id);
  m_partition_number =
    m_cfg.partition_number; // this should be 0-3

  m_timing_address = m_cfg.timing_address; // 0x20 is default for 101, 0x2B for 304, and 0x36 for 603
  if (m_timing_address > 0xff) {
    std::stringstream ss;
    ss << "Error: Incorrect timing address set (" << m_timing_address << ")!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Board ID is listed as: " << m_cfg.board_id << std::endl
                              << "Partition Number is: " << m_cfg.partition_number << std::endl
                              << "Timing Address is: " << m_cfg.timing_address << std::endl
                              << "Module ID is: " << m_cfg.module_id << std::endl;

  m_device_interface->SetPartitionNumber(m_partition_number);
  m_device_interface->SetTimingAddress(m_timing_address);
  m_module_id = m_cfg.module_id;
  m_device_interface->ConfigureLEDCalib(args); //This sets up the ethernet interface and make sure that the pdts is synched
  m_device_interface->SetRegisterByName("module_id", m_module_id);
  m_device_interface->SetRegisterByName("eventDataInterfaceSelect", m_cfg.interface_type);

  if ( m_cfg.pulse_mode == "single") {
    m_single_pulse = true;
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: I think that you want SSP LED Calib module to be in single pulse..." << std::endl;
  } else if ( m_cfg.pulse_mode == "burst") {
    m_burst_mode = true;
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: I think that you want SSP LED Calib module to be in BURST MODE..." << std::endl;
  } else if (m_cfg.pulse_mode == "double") {
    m_double_pulse = true;
  }

  if ( (m_single_pulse && m_double_pulse) ||
       (m_double_pulse && m_burst_mode) ||
       (m_single_pulse && m_burst_mode) ) {
    std::stringstream ss;
    ss << "ERROR: SOMEHOW ENDED UP WITH SEVERAL PULSE MODES SET ON!!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
    
  if (m_burst_mode) {
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Configuring for BURST MODE..." << std::endl;
    this->configure_burst_mode(args);
  } else if (m_single_pulse) {
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Configuring for single pulse mode..." << std::endl;
    this->configure_single_pulse(args);
  } else if (m_double_pulse) {
    this->configure_double_pulse(args);
  } else {
    std::stringstream ss;
    ss << "ERROR: SOMEHOW ENDED UP WITH NO PULSE MODES SET ON!!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }  
  //these seven lines should be commented out once the flag between burst mode and single pulse mode is sorted out
  //m_device_interface->SetRegister(0x80000448, m_burst_count, 0xFFFFFFFF); //pdts_cmd_delay_14 cal_count
  //m_device_interface->SetRegister(0x800003DC, 0x90000078, 0xFFFFFFFF); //cal_config_7
  //m_device_interface->SetRegister(0x800003E0, 0x90000078, 0xFFFFFFFF); //cal_config_8
  //m_device_interface->SetRegister(0x800003E4, 0x90000078, 0xFFFFFFFF); //cal_config_9
  //m_device_interface->SetRegister(0x800003E8, 0x90000078, 0xFFFFFFFF); //cal_config_10
  //m_device_interface->SetRegister(0x800003EC, 0x90000078, 0xFFFFFFFF); //cal_config_11
  //the above seven lines should be commented out once the flag between burst mode and single pulse mode is sorted out

  //if there are "literal" entries in the configuration they are explicit writes to the specified register with given value
  //these literal entries are paresed and applied last after any other parameters so this method call needs to be after the
  //other configuration calls
  this->manual_configure_device(args);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::configure complete.";
}

void
SSPLEDCalibWrapper::start(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Start pulsing SSPLEDCalibWrapper of card " << m_board_id << "...";
  if (m_run_marker.load()) {
    TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Run Marker says that SSPLEDCalibWrapper card " << m_board_id << " is already pulsing...";
    return;
  }

  m_cfg = args.get<dunedaq::sspmodules::sspledcalibmodule::Conf>();
  unsigned int channel_mask = m_cfg.channel_mask;
  unsigned int pulse_width = m_cfg.pulse_width_ticks;
  unsigned int pulse_bias_setting_270nm = 4095 * m_cfg.pulse_bias_percent_270nm/100;
  unsigned int pulse_bias_setting_367nm = 4095 * m_cfg.pulse_bias_percent_367nm/100;
  
  for (unsigned int counter = 0; counter < 5; counter++) { //switch this to 12 for a 12 channel SSP
    unsigned int bias_regAddress =  0x4000035C + 0x4*(counter);
    unsigned int bias_regVal = 0x00040000;
    unsigned int width_regAddress =  0x800003DC + 0x4*(counter);
    unsigned int width_regVal = 0xF0000000;
    if ( channel_mask & (1 << counter) ) {
      bias_regVal = bias_regVal + pulse_bias_setting_270nm;
      width_regVal = width_regVal + pulse_width;
      m_device_interface->SetRegister(bias_regAddress, bias_regVal); //BIAS_DAC_CONFIG_N
      m_device_interface->SetRegister(width_regAddress, width_regVal); //cal_CONFIG_N
    } else {
      m_device_interface->SetRegister(bias_regAddress, bias_regVal); //BIAS_DAC_CONFIG_N
      m_device_interface->SetRegister(width_regAddress, width_regVal); //cal_CONFIG_N
    }
  }
  
  // for (unsigned int counter = 6; counter < 12; counter++) { 
  //   unsigned int bias_regAddress =  0x4000035C + 0x4*(counter);
  //   unsigned int bias_regVal = 0x00040000;
  //   unsigned int width_regAddress =  0x800003DC + 0x4*(counter);
  //   unsigned int width_regVal = 0xF0000000;
  //   if ( channel_mask & (1 << counter) ) {
  //     bias_regVal = bias_regVal + pulse_bias_setting_367nm;
  //     width_regVal = width_regVal + pulse_width;
  //     m_device_interface->SetRegister(bias_regAddress, bias_regVal); //BIAS_DAC_CONFIG_N
  //     m_device_interface->SetRegister(width_regAddress, width_regVal); //cal_CONFIG_N
  //   } else {
  //     m_device_interface->SetRegister(bias_regAddress, bias_regVal); //BIAS_DAC_CONFIG_N
  //     m_device_interface->SetRegister(width_regAddress, width_regVal); //cal_CONFIG_N
  //   }
  // }

  unsigned int regAddress = 0x40000300;
  unsigned int regVal = 0x1;
  unsigned int readVal = 0x0;

  m_device_interface->SetRegister(regAddress, 0x1);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Incorrect register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
  }

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

  for (unsigned int counter = 0; counter < 5; counter++) { //switch this to 12 for a 12 channel SSP
    unsigned int bias_regAddress =  0x4000035C + 0x4*(counter);
    unsigned int width_regAddress =  0x800003DC + 0x4*(counter);
    m_device_interface->SetRegister(bias_regAddress, 0x00040000); //BIAS_DAC_CONFIG_N
    m_device_interface->SetRegister(width_regAddress, 0xF0000000); //cal_CONFIG_N
  }

  unsigned int regAddress = 0x40000300;
  unsigned int regVal = 0x0;
  unsigned int readVal = 0xFFFFFFFF;

  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Incorrect register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  m_run_marker = false;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stop pulsing SSPLEDCalibWrapper of card " << m_board_id << " complete.";
}

void
SSPLEDCalibWrapper::configure_single_pulse(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse called.";
  m_cfg = args.get<sspledcalibmodule::Conf>();

  m_device_interface->SetRegister(0x80000464, 0x00000200); //pdts_cmd_control_1
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
  
  unsigned int regAddress = 0x40000300;
  unsigned int regVal = 0x0; //turn off the bias at the time of configuration
  unsigned int readVal = 0xFFFFFFFF;
  m_device_interface->SetRegister(regAddress, regVal); //bias_control
  m_device_interface->ReadRegister(regAddress, readVal); //bias_control
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Incorrect register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
    }

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse completed.";
}

void
SSPLEDCalibWrapper::configure_double_pulse(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse called.";
  m_cfg = args.get<sspledcalibmodule::Conf>();
  unsigned int double_pulse_delay_ticks = m_cfg.double_pulse_delay_ticks;

  m_device_interface->SetRegister(0x80000464, 0x00000200); //pdts_cmd_control_1
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
  
  unsigned int regAddress = 0x40000300;
  unsigned int regVal = 0x0; //turn off the bias at the time of configuration
  unsigned int readVal = 0xFFFFFFFF;
  m_device_interface->SetRegister(regAddress, regVal); //bias_control
  m_device_interface->ReadRegister(regAddress, readVal); //bias_control
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Incorrect register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
    }

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse completed.";
}

void
SSPLEDCalibWrapper::configure_burst_mode(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureBurstMode called.";
  m_cfg = args.get<dunedaq::sspmodules::sspledcalibmodule::Conf>();
  unsigned int burst_count = 1;
  burst_count = m_cfg.burst_count;

  m_device_interface->SetRegister(0x80000464, 0x00000200); //pdts_cmd_control_1
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
  m_device_interface->SetRegister(0x80000448, burst_count); //cal_count
  
  unsigned int regAddress = 0x40000300;
  unsigned int regVal = 0x0; //turn off the bias control at the time of configuration
  unsigned int readVal = 0xFFFFFFFF;
  m_device_interface->SetRegister(regAddress, regVal); //bias_control
  m_device_interface->ReadRegister(regAddress, readVal); //bias_control
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Incorrect register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
  }

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureBurstMode completed.";
}

void
SSPLEDCalibWrapper::manual_configure_device(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDevice called.";
  std::vector<dunedaq::sspmodules::sspledcalibmodule::RegisterValues> m_hardware_configuration =
    m_cfg.hardware_configuration;
  TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Processing the Hardware Configuration list..." << std::endl;
  TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Hardware configuration seq has size : " << m_hardware_configuration.size()
                        << std::endl;
  // for (uint i = 0; i < m_hardware_configuration.size() ; ++i ) {
  for (auto regValuesIter = m_hardware_configuration.begin(); regValuesIter != m_hardware_configuration.end();
       ++regValuesIter) {
    std::string m_name = regValuesIter->regname;
    std::vector<unsigned int> m_hexvalues = regValuesIter->hexvalues;
    TLOG(TLVL_FULL_DEBUG) << "SSPLEDCalibWrapper: Hardware configuration for regsiter name: " << m_name
                          << " being processed." << std::endl;
    if (m_hexvalues.size() == 0) {
      TLOG() << "ERROR: Hardware configuration for regsiter name: " << m_name
             << " does not have any hexvalues associated with it" << std::endl;
      continue;
    }

    // Expect to see a Literals section; take any name starting with "Literal" and parse as hex values: regAddress,
    // regValue, regMask
    if (!m_name.compare(0, 7, "Literal")) {              // NOLINT(readability/braces)
      unsigned int regAddress = m_hexvalues[0];
      unsigned int regVal = m_hexvalues[1];
      unsigned int regMask = m_hexvalues.size() > 2 ? m_hexvalues[2] : 0xFFFFFFFF;
      TLOG(TLVL_FULL_DEBUG) << "Preparing to write to literal register address: 0x" << std::hex << regAddress
                            << " with value: 0x" << regVal << " and mask: 0x" << regMask << std::dec << std::endl;
      m_device_interface->SetRegister(regAddress, regVal, regMask);
    } // End Processing of Literals
  }
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDevice complete.";
} // NOLINT(readability/fn_size)

  /*void
SSPLEDCalibWrapper::build_channel_control_registers(const std::vector<std::pair<std::string, unsigned int>> entries,
                                                std::vector<unsigned int>& reg)
{
  for (auto ccIter = entries.begin(); ccIter != entries.end(); ++ccIter) {
    // External trigger mode
    if (!ccIter->first.compare("ChannelControl_ExtTriggerMode")) {
      unsigned int val = ccIter->second;
      switch (val) {
        case 0: // No external trigger
          for (unsigned int i = 0; i < 12; ++i) {
            reg[i] = reg[i] & 0xFFFF0FFF;
          }
          break;
        case 1: // Edge trigger on front panel
          for (unsigned int i = 0; i < 12; ++i) {
            reg[i] = (reg[i] & 0xFFFF0FFF) + 0x00006000;
          }
          break;
        case 2: // Use front panel as gate
          for (unsigned int i = 0; i < 12; ++i) {
            reg[i] = (reg[i] & 0xFFFF0FFF) + 0x00005000;
          }
          break;
        case 3: // Timestamp trigger
          for (unsigned int i = 0; i < 12; ++i) {
            reg[i] = (reg[i] & 0xFFFF0FFF) + 0x0000E000;
          }
          break;
        default:
          // DAQLogger::LogError("SSP_SSP_generator")<<"Error: invalid value for external trigger source
          // setting!"<<std::endl;
          // throw SSPDAQ::EDAQConfigError("");
          break;
      }
    } else if (!ccIter->first.compare("ChannelControl_LEDTrigger")) {
      unsigned int val = ccIter->second;
      switch (val) {
        case 1: // Negative edge
          for (unsigned int i = 0; i < 12; ++i) {
            reg[i] = (reg[i] & 0x7FFFF3FF) + 0x80000800;
          }
          break;
        case 2: // Positive edge
          for (unsigned int i = 0; i < 12; ++i) {
            reg[i] = (reg[i] & 0x7FFFF3FF) + 0x80000400;
          }
          break;
        case 0:
          for (unsigned int i = 0; i < 12; ++i) { // Disabled
            reg[i] = reg[i] & 0x7FFFF3FF;
          }
          break;
        default:
          // DAQLogger::LogError("SSP_SSP_generator")<<"Error: invalid value for trigger polarity
          // setting!"<<std::endl;
          // throw SSPDAQ::EDAQConfigError("");
          break;
      }
    } else if (!ccIter->first.compare("ChannelControl_TimestampRate")) {
      unsigned int val = ccIter->second;
      if (val > 7) {
        // DAQLogger::LogError("SSP_SSP_generator")<<"Error: invalid value for timestamp rate setting!"<<std::endl;
        // throw SSPDAQ::EDAQConfigError("");
      }
      for (unsigned int i = 0; i < 12; ++i) {
        reg[i] = (reg[i] & 0xFFFFFF1F) + 0x20 * val;
      }
    }
  }
  }*/


void
SSPLEDCalibWrapper::validate_config(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::validate_config called.";
  m_cfg = args.get<sspledcalibmodule::Conf>();

  if (m_cfg.channel_mask > 4095) {
    std::stringstream ss;
    ss << "ERROR: Incorrect channel_maks value " << m_cfg.channel_mask << " is higher than the limit of 4095!!!" << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (!( (m_cfg.pulse_mode == "single") || (m_cfg.pulse_mode == "double") || (m_cfg.pulse_mode == "burst") ) ) {
    std::stringstream ss;
    ss << "ERROR: Incorrect pulse_mode value is " << m_cfg.pulse_mode << ", it must be single, double, or burst."
       << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  if (m_cfg.double_pulse_delay_ticks > 1000) {
    std::stringstream ss;
    ss << "ERROR: Strange!! double_pulse_delay_ticks value is " << m_cfg.double_pulse_delay_ticks << ", which is greater than a single drift readout window"
       << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (m_cfg.burst_count > 10000) {
    std::stringstream ss;
    ss << "ERROR: Strange!! burst_count value is " << m_cfg.burst_count << ", which is more time than in a drift readout window"
       << std::endl;
    TLOG() << ss.str();
    //throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (m_cfg.pulse_width_ticks > 1000000) {
    std::stringstream ss;
    ss << "ERROR: Incorrect pulse_width_ticks value is " << m_cfg.pulse_width_ticks << ", which is greater than a single drift readout window"
       << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  if (m_cfg.pulse_bias_percent_270nm > 100) {
    std::stringstream ss;
    ss << "ERROR: Incorrect pulse_bias_percent_270nm value is " << m_cfg.pulse_bias_percent_270nm << ", which is greater than 100 percent!!!"
       << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  if (m_cfg.pulse_bias_percent_367nm > 100) {
    std::stringstream ss;
      ss << "ERROR: Incorrect pulse_bias_percent_367nm value is " << m_cfg.pulse_bias_percent_367nm << ", which is greater than 100 percent!!!"
	 << std::endl;
      TLOG() << ss.str();
      throw ConfigurationError(ERS_HERE, ss.str());
  }
  
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::validate_config complete.";
}

} // namespace sspmodules
} // namespace dunedaq

   // {
   //      "data": {
   //          "modules": [
   //              {
   //                  "data": {
   //                      "card_id": 999,
   //                      "board_id": 999,
   //                      "module_id": 999,
   //                     "interface_type": 1,
    //                     "board_ip": "10.73.137.81",
    //                     "partition_number": 0,
    //                     "timing_address": 32,
    //                     "channel_mask": 4095, //this is a binary bit mask, but has to be given in decimal
    //                     "pulse_mode": "burst","single","double"
    //                     "double_pulse_delay_ticks": 100, //number of ticks between pulses when in double mode
    //                     "burst_count": 1000, //count of pulses to give in burst mode
    //                     "pulse_width_ticks": 100, //one tick is ~4 ns
    //                     "pulse_bias_percent_270nm": 100, //this is percentage of the bias that is supplied to the SSP card
    //                     "pulse_bias_percent_367nm": 100,  //note that the bias might be anything 0V - 35V
                        // "hardware_configuration": [ //included to be able to overwrite default config values
                        //     {
                        //         "regname": "Literal",
                        //         "hexvalues": [2147484776,2147483648] //this is 0x80000468 with value 0x80000000 which is pdts_cmd_control_2 and turns on the front panel LEDs
                        //     }
			// ]
    //                 },
    //                 "match": "ssp_led_calib"
    //             }
    //         ]
    //     },
    //     "entry_state": "INITIAL",
    //     "exit_state": "CONFIGURED",
    //     "id": "conf"
    // }
#endif // SSPMODULES_SRC_SSPLEDCALIBWRAPPER_CPP_
