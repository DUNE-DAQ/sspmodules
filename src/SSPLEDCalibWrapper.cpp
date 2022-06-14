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
    m_cfg.partition_number; // 3 is default; //I think this should be less than 4, but I'm using what Giovanna sent me

  m_timing_address = m_cfg.timing_address; // 0x20 is default for 101, 0x2B for 304, and 0x36 for 603
  if (m_timing_address > 0xff) {
    std::stringstream ss;
    ss << "Error: Invalid timing address set (" << m_timing_address << ")!" << std::endl;
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
  m_device_interface->SetRegisterByName("module_id", m_module_id);
  m_device_interface->SetRegisterByName("eventDataInterfaceSelect", m_cfg.interface_type);
  m_device_interface->ConfigureLEDCalib(args);

  //this->configure_daq(args);
  //this->configure_device(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::configure complete.";
}

void
SSPLEDCalibWrapper::start_pulses(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Start pulsing SSPLEDCalibWrapper of card " << m_board_id << "...";
  if (m_run_marker.load()) {
    TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Run Marker says that SSPLEDCalibWrapper card " << m_board_id << " is already pulsing...";
    return;
  }
    
  unsigned int regAddress = 0x800003DC;
  unsigned int regVal = 0xF0000078;
  unsigned int readVal = 0;

  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003E0;
  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003E4;
  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003E8;
  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003EC;
  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
  m_run_marker = true;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Start pulsing SSPLEDCalibWrapper of card " << m_board_id << " complete.";
}

void
SSPLEDCalibWrapper::stop_pulses(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stop pulsing SSPLEDCalibWrapper of card " << m_board_id << "...";
  if (!m_run_marker.load()) {
    TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "The run_marker says that SSPLEDCalibWrapper card " << m_board_id << " is already stopped, but stopping anyways...";
  }
  
  unsigned int regAddress = 0x800003DC;
  unsigned int regVal = 0x00000078;
  unsigned int readVal = 0x0;

  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003E0;
  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003E4;
  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003E8;
  m_device_interface->SetRegister(regAddress, regVal);
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  regAddress = 0x800003EC;
  m_device_interface->SetRegister(regAddress, regVal);  
  m_device_interface->ReadRegister(regAddress, readVal);
  if (regVal != readVal) {
    std::stringstream ss;
    ss << "ERROR: Invalid register value 0x" <<std::hex << readVal << " at regsiter address 0x" << regAddress <<"!!!" << std::dec << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  }
  m_run_marker = false;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stop pulsing SSPLEDCalibWrapper of card " << m_board_id << " complete.";
}

void
SSPLEDCalibWrapper::start(const data_t& )
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Starting SSPLEDCalibWrapper of card " << m_board_id << "...";
  /* if (!m_run_marker.load()) {
    m_num_zero_fragments = 0;
    m_num_fragments_sent = 0;
    m_num_read_event_calls = 0;
    set_running(true);
    m_device_interface->Start();
    m_ssp_processor.set_work(&SSPLEDCalibWrapper::process_ssp, this);
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Started SSPLEDCalibWrapper of card " << m_board_id << "...";
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPLEDCalibWrapper of card " << m_board_id << " is already running!";
    }*/
  TLOG_DEBUG(0) << "NOTE THAT THIS START COMMAND DOESN'T DO ANYTHING FOR THE SSP LED CALIB CARD!!!";
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Starting SSPLEDCalibWrapper of card " << m_board_id << " complete.\n BUT NOTE THAT THIS COMMAND DOESN'T DO ANYTHING!";
}

void
SSPLEDCalibWrapper::stop(const data_t& )
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stopping SSPLEDCalibWrapper of card " << m_board_id << "...";
  /*if (m_run_marker.load()) {
    set_running(false);
    m_device_interface->Stop();
    while (!m_ssp_processor.get_readiness()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Stopped SSPLEDCalibWrapper of card " << m_board_id << "!";
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPLEDCalibWrapper of card " << m_board_id << " is already stopped!";
    }*/
  TLOG_DEBUG(0) << "NOTE THAT THIS STOP COMMAND DOESN'T DO ANYTHING FOR THE SSP LED CALIB CARD!!!";
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stoping SSPLEDCalibWrapper of card " << m_board_id << " complete.\n BUT NOTE THAT THIS COMMAND DOESN'T DO ANYTHING!";
}

  /*void
SSPLEDCalibWrapper::set_running(bool should_run)
{
  //bool was_running = m_run_marker.exchange(should_run);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPLEDCalibWrapper Active state was toggled from " << std::boolalpha << was_running
                              << " to " << should_run;
}
  */

/*
void
SSPLEDCalibWrapper::open_card()
{}
*/

 /*
void
SSPLEDCalibWrapper::close_card()
{}
*/
 
void
SSPLEDCalibWrapper::configure_single_pulse(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse called.";
  //check if it's currently pulsing
     //send error message
     //abort reconfigure
  //if not currently pulsing reconfigure
  m_device_interface->SetRegister(0x80000464, 0x00000200, 0xFFFFFFFF); //pdts_cmd_control_1
  m_device_interface->SetRegister(0x80000940, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_0
  m_device_interface->SetRegister(0x80000944, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_1
  m_device_interface->SetRegister(0x80000948, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_2
  m_device_interface->SetRegister(0x8000094C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_3
  m_device_interface->SetRegister(0x80000950, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_4
  m_device_interface->SetRegister(0x80000954, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_5
  m_device_interface->SetRegister(0x80000958, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_6
  m_device_interface->SetRegister(0x8000095C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_7
  m_device_interface->SetRegister(0x80000960, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_8
  m_device_interface->SetRegister(0x80000964, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_9
  m_device_interface->SetRegister(0x80000968, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_10
  m_device_interface->SetRegister(0x8000096C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_11
  m_device_interface->SetRegister(0x80000970, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_12
  m_device_interface->SetRegister(0x80000974, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_13
  m_device_interface->SetRegister(0x80000978, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_14
  m_device_interface->SetRegister(0x8000097C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_15
  m_device_interface->SetRegister(0x80000468, 0x80000000, 0xFFFFFFFF); //pdts_cmd_control_2
  m_device_interface->SetRegister(0x80000520, 0x00000011, 0xFFFFFFFF); //pulser_mode_control
  m_device_interface->SetRegister(0x800003DC, 0xF0000078, 0xFFFFFFFF); //cal_config_7
  m_device_interface->SetRegister(0x800003E0, 0xF0000078, 0xFFFFFFFF); //cal_config_8
  m_device_interface->SetRegister(0x800003E4, 0xF0000078, 0xFFFFFFFF); //cal_config_9
  m_device_interface->SetRegister(0x800003E8, 0xF0000078, 0xFFFFFFFF); //cal_config_10
  m_device_interface->SetRegister(0x800003EC, 0xF0000078, 0xFFFFFFFF); //cal_config_11
  m_device_interface->SetRegister(0x8000035C, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_7
  m_device_interface->SetRegister(0x80000360, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_8
  m_device_interface->SetRegister(0x80000364, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_9
  m_device_interface->SetRegister(0x80000368, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_10
  m_device_interface->SetRegister(0x8000036C, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_11
  m_device_interface->SetRegister(0x80000300, 0x00000001, 0xFFFFFFFF); //pdts_cmd_delay_14 bias_control
  m_device_interface->SetRegister(0x80000448, 0x00000001, 0xFFFFFFFF); //pdts_cmd_delay_14 cal_count
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureSinglePulse completed.";
}

void
SSPLEDCalibWrapper::configure_burst_mode(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureBurstMode called.";
  //check if it's currently pulsing
     //send error message
     //abort reconfigure
  //if not currently pulsing reconfigure
  m_device_interface->SetRegister(0x80000464, 0x00000200, 0xFFFFFFFF); //pdts_cmd_control_1
  m_device_interface->SetRegister(0x80000940, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_0
  m_device_interface->SetRegister(0x80000944, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_1
  m_device_interface->SetRegister(0x80000948, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_2
  m_device_interface->SetRegister(0x8000094C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_3
  m_device_interface->SetRegister(0x80000950, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_4
  m_device_interface->SetRegister(0x80000954, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_5
  m_device_interface->SetRegister(0x80000958, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_6
  m_device_interface->SetRegister(0x8000095C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_7
  m_device_interface->SetRegister(0x80000960, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_8
  m_device_interface->SetRegister(0x80000964, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_9
  m_device_interface->SetRegister(0x80000968, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_10
  m_device_interface->SetRegister(0x8000096C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_11
  m_device_interface->SetRegister(0x80000970, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_12
  m_device_interface->SetRegister(0x80000974, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_13
  m_device_interface->SetRegister(0x80000978, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_14
  m_device_interface->SetRegister(0x8000097C, 0x00030036, 0xFFFFFFFF); //pdts_cmd_delay_15
  m_device_interface->SetRegister(0x80000468, 0x80000000, 0xFFFFFFFF); //pdts_cmd_control_2
  m_device_interface->SetRegister(0x80000520, 0x00000011, 0xFFFFFFFF); //pulser_mode_control
  m_device_interface->SetRegister(0x800003DC, 0xF0000078, 0xFFFFFFFF); //cal_config_7
  m_device_interface->SetRegister(0x800003E0, 0xF0000078, 0xFFFFFFFF); //cal_config_8
  m_device_interface->SetRegister(0x800003E4, 0xF0000078, 0xFFFFFFFF); //cal_config_9
  m_device_interface->SetRegister(0x800003E8, 0xF0000078, 0xFFFFFFFF); //cal_config_10
  m_device_interface->SetRegister(0x800003EC, 0xF0000078, 0xFFFFFFFF); //cal_config_11
  m_device_interface->SetRegister(0x8000035C, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_7
  m_device_interface->SetRegister(0x80000360, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_8
  m_device_interface->SetRegister(0x80000364, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_9
  m_device_interface->SetRegister(0x80000368, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_10
  m_device_interface->SetRegister(0x8000036C, 0x00040FFF, 0xFFFFFFFF); //BIAS_DAC_CONFIG_11
  m_device_interface->SetRegister(0x80000300, 0x00000001, 0xFFFFFFFF); //pdts_cmd_delay_14 bias_control

  unsigned int burst_count = 1000;
  m_device_interface->SetRegister(0x80000448, burst_count, 0xFFFFFFFF); //pdts_cmd_delay_14 cal_count
  m_device_interface->SetRegister(0x800003DC, 0x90000078, 0xFFFFFFFF); //cal_config_7
  m_device_interface->SetRegister(0x800003E0, 0x90000078, 0xFFFFFFFF); //cal_config_8
  m_device_interface->SetRegister(0x800003E4, 0x90000078, 0xFFFFFFFF); //cal_config_9
  m_device_interface->SetRegister(0x800003E8, 0x90000078, 0xFFFFFFFF); //cal_config_10
  m_device_interface->SetRegister(0x800003EC, 0x90000078, 0xFFFFFFFF); //cal_config_11

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureBurstMode completed.";
}

void
SSPLEDCalibWrapper::configure_device(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDevice called.";

  std::vector<dunedaq::sspmodules::sspledcalibmodule::RegisterValues> m_hardware_configuration =
    m_cfg.hardware_configuration;
  std::vector<unsigned int> chControlReg(12, 0);
  bool haveChannelControl = false;
  std::vector<std::pair<std::string, unsigned int>> channelControlEntries;
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

    if (!m_name.compare(0, 14, "ChannelControl")) {
      channelControlEntries.push_back(make_pair(m_name, m_hexvalues[0]));
      haveChannelControl = true;
    }
    // Expect to see a Literals section; take any name starting with "Literal" and parse as hex values: regAddress,
    // regValue, regMask
    else if (!m_name.compare(0, 7, "Literal")) {              // NOLINT(readability/braces)
      unsigned int regAddress = m_hexvalues[0];
      unsigned int regVal = m_hexvalues[1];
      unsigned int regMask = m_hexvalues.size() > 2 ? m_hexvalues[2] : 0xFFFFFFFF;
      TLOG(TLVL_FULL_DEBUG) << "Preparing to write to literal register address: 0x" << std::hex << regAddress
                            << " with value: 0x" << regVal << " and mask: 0x" << regMask << std::dec << std::endl;
      m_device_interface->SetRegister(regAddress, regVal, regMask);
    } // End Processing of Literals
    // Intercept channel_control setting so that we can replace bits with logical values later...
    else if (!m_name.substr(4).compare("channel_control")) {  // NOLINT(readability/braces)
      if (!m_name.substr(0, 4).compare(
            "ELE_")) { // The format expected is ELE_channel_control: register_number, regsiter_value
        // std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
        TLOG(TLVL_FULL_DEBUG) << "Found a channel control element: " << m_name
                              << " with register number: " << m_hexvalues[0] << " and value: " << m_hexvalues[1]
                              << std::endl;
        chControlReg[m_hexvalues[0]] = m_hexvalues[1];
      } else if (!m_name.substr(0, 4).compare(
                   "ALL_")) { // All array elements set to same value e.g. ALL_channel_control: 0xDEADBEEF and the
                              // register must be one with 12 entries
        TLOG(TLVL_FULL_DEBUG) << "Found a channel control ALL: " << m_name << " and value: " << m_hexvalues[0]
                              << std::endl;
        if (m_hexvalues.size() != 1) {
          TLOG() << "Trying to write to all channel control registers but listing more than one single value!!!"
                 << std::endl;
        }
        for (unsigned int i = 0; i < 12; ++i) {
          chControlReg[i] = m_hexvalues[0];
        }
      } else if (!m_name.substr(0, 4).compare("ARR_")) { // All array elements individually e.g. ARR_channel_control:
                                                         // 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc
        TLOG(TLVL_FULL_DEBUG) << "Found a channel control array: " << m_name
                              << " and first two values: " << m_hexvalues[0] << " and " << m_hexvalues[1] << std::endl;
        for (unsigned int i = 0; i < 12; ++i) {
          chControlReg[i] = m_hexvalues[i];
        }
      }
    } else if (!m_name.substr(0, 4).compare("ELE_")) { // Single array element
      TLOG(TLVL_FULL_DEBUG) << "Preparing to write to a register element with the name: "
                            << m_name.substr(4, std::string::npos) << " and element: " << m_hexvalues[0]
                            << " with value: " << m_hexvalues[1] << std::endl;
      m_device_interface->SetRegisterElementByName(m_name.substr(4, std::string::npos), m_hexvalues[0], m_hexvalues[1]);
    } else if (!m_name.substr(0, 4).compare("ALL_")) { // All array elements set to same value
      if (m_hexvalues.size() != 1) {
        TLOG() << "Trying to write to all channel control registers but listing more than one single value!!!"
               << std::endl;
      }
      TLOG(TLVL_FULL_DEBUG) << "Preparing to write to ALL registers named: " << m_name.substr(4, std::string::npos)
                            << " with value: " << m_hexvalues[0] << std::endl;
      m_device_interface->SetRegisterArrayByName(m_name.substr(4, std::string::npos), m_hexvalues[0]);
    } else if (!m_name.substr(0, 4).compare("ARR_")) { // All array elements individually
      std::vector<unsigned int> vals;
      for (unsigned int i = 0; i < m_hexvalues.size(); ++i) {
        vals.push_back(m_hexvalues[i]);
      }
      TLOG(TLVL_FULL_DEBUG) << "Preparing to write to ARRAY of registers named: " << m_name.substr(4, std::string::npos)
                            << " with " << vals.size() << " values: " << std::endl;
      for (unsigned int i = 0; i < vals.size(); ++i) {
        TLOG(TLVL_FULL_DEBUG) << "Register " << m_name.substr(4, std::string::npos) << " number " << i
                              << " value: " << vals[i] << std::endl;
      }
      m_device_interface->SetRegisterArrayByName(m_name.substr(4, std::string::npos), vals);
    } else { // Individual register not in an array
      TLOG(TLVL_FULL_DEBUG) << "Preparing to write to register named: " << m_name << " with value: " << m_hexvalues[0]
                            << std::endl;
      m_device_interface->SetRegisterByName(m_name, m_hexvalues[0]);
    }
  }

  // Modify channel control registers and send to hardware
  if (haveChannelControl) {
    this->build_channel_control_registers(channelControlEntries, chControlReg);
  }
  m_device_interface->SetRegisterArrayByName("channel_control", chControlReg);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDevice complete.";
} // NOLINT(readability/fn_size)

void
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
}

/*
void
SSPLEDCalibWrapper::configure_daq(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDAQ called.";
  m_cfg = args.get<sspledcalibmodule::Conf>();
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibWrapper::ConfigureDAQ complete.";
  }*/

/*
void
SSPLEDCalibWrapper::process_ssp()
{
  TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPLEDCalibWrapper starts processing blocks...";
  while (m_run_marker.load()) {

    bool hasSeenSlice = false;

    while (!hasSeenSlice) {

      std::vector<unsigned int> millislice;
      // JCF, Mar-8-2016
      // Could I just wrap this in a try-catch block?
      m_device_interface->ReadEvent(millislice);
      if (m_device_interface->exception()) {
        // set_exception(true);
        TLOG_DEBUG(TLVL_WORK_STEPS) << "dune::SSP::getNext_ : found device interface thread in exception state";
      }

      static size_t ncalls = 1;
      static size_t ncalls_with_millislice = 0;

      if (millislice.size() > 0) {
        ncalls_with_millislice++;
      }
      ncalls++;

      if (millislice.size() == 0) {
        if (!hasSeenSlice) {
          ++m_num_zero_fragments;
          usleep(100000);
        }
        break;
      }
      hasSeenSlice = true;

      TLOG_DEBUG(5) << m_device_interface->GetIdentifier() << "Generator sending fragment " << m_num_fragments_sent
                    << ", calls to GetNext " << m_num_read_event_calls << ", of which returned null "
                    << m_num_zero_fragments << std::endl;

      // std::size_t dataLength = millislice.size()-dunedaq::detdataformats::ssp::MillisliceHeader::sizeInUInts;

      // SSPFragment::Metadata metadata;
      // metadata.sliceHeader=*((SSPDAQ::MillisliceHeader*)(void*)millislice.data());
      // auto timestamp = (metadata.sliceHeader.triggerTime + fFragmentTimestampOffset_) / 3 ;
      // TLOG_DEBUG(TLVL_WORK_STEPS) << "SSP millislice w/ timestamp is " << timestamp
      //                                     << " millislice counter is "<< std::to_string(ncalls_with_millislice);
    }

    // SSPFragment::Metadata metadata;
    // metadata.sliceHeader=*((SSPDAQ::MillisliceHeader*)(void*)millislice.data());
    // We'll use the static factory function
    // artdaq::Fragment::FragmentBytes(std::size_t payload_size_in_bytes, sequence_id_t sequence_id,
    //  fragment_id_t fragment_id, type_t type, const T & metadata)
    // which will then return a unique_ptr to an artdaq::Fragment
    // object.
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // fix 5ms initial poll
  }
}
*/

} // namespace sspmodules
} // namespace dunedaq

// ssp101_standard: @local::ssp_standard
// ssp101_standard.fragment_receiver.fragment_id: 11
// ssp101_standard.fragment_receiver.board_id: 11
// ssp101_standard.fragment_receiver.timing_address: 0x20
// ssp101_standard.fragment_receiver.board_ip: "10.73.137.56"
// ssp101_standard.fragment_receiver.HardwareConfig.module_id: 11
// ssp101_standard.metrics.dim.IDName: "ssp101"
//

#endif // SSPMODULES_SRC_SSPLEDCALIBWRAPPER_CPP_
