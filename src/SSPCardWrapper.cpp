/**
 * @file SSPCardWrapper.cpp SSP library wrapper implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_SSPCARDWRPPER_CPP_
#define SSPMODULES_SRC_SSPCARDWRPPER_CPP_

// From Module
#include "SSPCardWrapper.hpp"
//#include "dune-raw-data/Overlays/SSPFragment.hh"
//#include "dune-raw-data/Overlays/SSPFragmentWriter.hh"
//#include "packetformat/block_format.hpp"

// From STD
#include <chrono>
#include <iomanip>

/**
 * @brief TRACE debug levels used in this source file
 */
#define TRACE_NAME "SSPCardWrapper" // NOLINT

enum
{
  TLVL_ENTER_EXIT_METHODS = 5,
  TLVL_WORK_STEPS = 10,
  TLVL_BOOKKEEPING = 15,
  TLVL_FULL_DEBUG = 63
};


namespace dunedaq {
namespace sspmodules {

  SSPCardWrapper::SSPCardWrapper():
    m_device_interface(0),
    m_ssp_processor(0),
    m_run_marker{ false },
    m_board_id(0),
    m_partition_number(0),
    m_timing_address(0),
    m_pre_trig_length(0),
    m_post_trig_length(0),
    m_use_external_timestamp(2),
    m_trigger_write_delay(0),
    m_trigger_latency(0),
    m_dummy_period(-1),
    m_hardware_clock_rate_in_MHz(0),
    m_trigger_mask(0),
    m_fragment_timestamp_offset(0)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper constructor called."<<std::endl;
  TLOG_DEBUG(TLVL_FULL_DEBUG) << "Constructor doesn't actually do anything but initialize conf paramters to none function values."<<std::endl;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper constructor complete."<<std::endl;

}

SSPCardWrapper::~SSPCardWrapper()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper destructor called."<<std::endl;
  close_card();
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper destructor complete."<<std::endl;
}

void
SSPCardWrapper::init(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::init called."<<std::endl;
  //REMEMBER: the init method isn't supposed to do any frontend board configuration in the new DUNE-DAQ framework
  //          ALL of the frontend board configuration is to be done in the conf call and it is likely that the
  //          configuration parameters that you're looking for in args aren't available since the args you're
  //          getting here is likely only *::Init data from the json file

  m_device_interface=new dunedaq::sspmodules::DeviceInterface(dunedaq::dataformats::kEthernet);
  m_device_interface->Initialize(args);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::init complete.";
}

void
SSPCardWrapper::configure(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::configure called.";

  m_cfg = args.get<dunedaq::sspmodules::sspcardreader::Conf>();
  if (m_cfg.board_ip == "default") {
    TLOG() << "SSPCardWrapper::configure: This Board IP value in the Conf is set to: default" << std::endl
	     << "As we currently only deal with SSPs on ethernet, this means that either the Board IP was " << std::endl
	     << "NOT set, or the args.get<Conf> call failed to find parameters." <<std::endl ;
    //throw SSPDAQ::EDAQConfigError("");
    exit(424);
  }

  m_board_id = m_cfg.board_id;
  m_instance_name_for_metrics = "SSP " + std::to_string(m_board_id);
  m_partition_number = m_cfg.partition_number;  //3 is default; //I think this should be less than 4, but I'm using what Giovanna sent me

  m_timing_address = m_cfg.timing_address; //0x20 is default for 101, 0x2B for 304, and 0x36 for 603
  if(m_timing_address > 0xff){
    TLOG() << "Error: Invalid timing address set ("<<m_timing_address<<")!"<<std::endl;
    //throw SSPDAQ::EDAQConfigError("");
    exit(424);
  }

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Board ID is listed as: " << m_cfg.board_id << std::endl
					<< "Partition Number is: " << m_cfg.partition_number << std::endl
					<< "Timing Address is: " << m_cfg.timing_address << std::endl;

  m_device_interface->SetPartitionNumber(m_partition_number);
  m_device_interface->SetTimingAddress(m_timing_address);
  m_device_interface->Configure(args);

  this->configure_daq(args);
  this->configure_device(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::configure complete.";
}

void
SSPCardWrapper::start(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Starting SSPCardWrapper of card " << m_board_id << "...";
  if (!m_run_marker.load()) {
    m_num_zero_fragments=0;
    m_num_fragments_sent=0;
    m_num_read_event_calls=0;
    set_running(true);
    m_device_interface->Start();
    m_ssp_processor.set_work(&SSPCardWrapper::process_ssp, this);
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Started SSPCardWrapper of card " << m_board_id << "...";
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPCardWrapper of card " << m_board_id << " is already running!";
  }
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Starting SSPCardWrapper of card " << m_board_id << " complete.";
}

void
SSPCardWrapper::stop(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stopping SSPCardWrapper of card " << m_board_id << "...";
  if (m_run_marker.load()) {
    set_running(false);
    m_device_interface->Stop();
    while (!m_ssp_processor.get_readiness()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Stopped SSPCardWrapper of card " << m_board_id << "!";
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPCardWrapper of card " << m_board_id << " is already stopped!";
  }
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stoping SSPCardWrapper of card " << m_board_id << " complete.";
}

void
SSPCardWrapper::set_running(bool should_run)
{
  bool was_running = m_run_marker.exchange(should_run);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPCardWrapper Active state was toggled from " << std::boolalpha << was_running << " to " << should_run;
}

void
SSPCardWrapper::open_card()
{
}

void
SSPCardWrapper::close_card()
{
}

void
SSPCardWrapper::configure_device(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::ConfigureDevice called.";

  m_module_id = m_cfg.module_id;
  std::vector<dunedaq::sspmodules::sspcardreader::RegisterValues> m_hardware_configuration = m_cfg.hardware_configuration;
  std::vector<unsigned int> chControlReg(12,0);
  // bool haveChannelControl=false;
  std::vector< std::pair<std::string,unsigned int>> channelControlEntries;

  /*for (auto regValuesIter=m_hardware_configuration.begin();regValuesIter!=m_hardware_configuration.end();++regValuesIter){
    std::string name = regValuesIter->regname;
    std::vector<unsigned int> hexvalues = regValuesIter->hexvalues;

    if (!name.compare(0,14,"ChannelControl")) {
      channelControlEntries.push_back(make_pair(name,hexvalues[0]));
      haveChannelControl=true;
    }
    //Expect to see a Literals section; take any name starting with "Literal" and parse as hex values: regAddress, regValue, regMask
    else if(!name.compare(0,7,"Literal")){
      unsigned int regAddress=hexvalues[0];
      unsigned int regVal=hexvalues[1];
      unsigned int regMask=hexvalues.size()>2?hexvalues[2]:0xFFFFFFFF;
      m_device_interface->SetRegister(regAddress,regVal,regMask);
    }//End Processing of Literals
    //Intercept channel_control setting so that we can replace bits with logical values later...
    else if(!name.substr(4).compare("channel_control")){
      if(!name.substr(0,4).compare("ELE_")){ //The format expected is ELE_channel_control: register_number, regsiter_value
        //std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
        chControlReg[hexvalues[0]]=hexvalues[1];
      }
      else if(!name.substr(0,4).compare("ALL_")){ //All array elements set to same value e.g. ALL_channel_control: 0xDEADBEEF and the register must be one with 12 entries
        for(unsigned int i=0;i<12;++i){
          chControlReg[i]=hexvalues[i];
        }
      }
      else if(!name.substr(0,4).compare("ARR_")){ //All array elements individually e.g. ARR_channel_control: 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc
        for(unsigned int i=0;i<12;++i){
          chControlReg[i]=hexvalues[i];
        }
      }
    }
    else if(!name.substr(0,4).compare("ELE_")){ //Single array element
      //std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
      m_device_interface->SetRegisterElementByName(name.substr(4,std::string::npos),hexvalues[0],hexvalues[1]);
    }
    else if(!name.substr(0,4).compare("ALL_")){ //All array elements set to same value
      unsigned int val=hexvalues[0];
      m_device_interface->SetRegisterArrayByName(name.substr(4,std::string::npos),val);
    }
    else if(!name.substr(0,4).compare("ARR_")){ //All array elements individually
      std::vector<unsigned int> vals;
      for (unsigned int i=0; i<hexvalues.size(); ++i){
	vals[i]=hexvalues[i];
      }
      m_device_interface->SetRegisterArrayByName(name.substr(4,std::string::npos),vals);
    }
    else{ //Individual register not in an array
      m_device_interface->SetRegisterByName(name,hexvalues[0]);
    }
  }
  //Modify channel control registers and send to hardware
  if(haveChannelControl){
    this->build_channel_control_registers(channelControlEntries,chControlReg);
  }
  m_device_interface->SetRegisterArrayByName("channel_control",chControlReg);

  //this is all just doing it hardcoded

  //unsigned int val=hardwareConfig.get<unsigned int>(*hcIter);
  //m_device_interface->SetRegisterArrayByName(hcIter->substr(4,std::string::npos),val);// for ALL configs not channel_control
  //std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
  //m_device_interface->SetRegisterArrayByName(hcIter->substr(4,std::string::npos),vals); //for ARR configs not channel_control
  //unsigned int val=hardwareConfig.get<unsigned int>(*hcIter);
  //m_device_interface->SetRegisterByName(*hcIter,val); //for things without ARR, ALL, or ELE
  */

  //std::vector<unsigned int> chControlReg(12,0);
  for(unsigned int i=0;i<12;++i){
    chControlReg[i]=0x00000401;
  }
  m_device_interface->SetRegisterArrayByName("channel_control",chControlReg); //set all channel control to 0x00000401
  m_device_interface->SetRegisterArrayByName("channel_control", 0x00000401);
  m_device_interface->SetRegisterArrayByName("readout_window", 2000);
  m_device_interface->SetRegisterArrayByName("readout_pretrigger", 50);
  m_device_interface->SetRegisterArrayByName("cfd_parameters", 0x1800 );
  m_device_interface->SetRegisterArrayByName("p_window", 0x20 );
  m_device_interface->SetRegisterArrayByName("i2_window", 1200 );
  m_device_interface->SetRegisterArrayByName("m1_window", 10 );
  m_device_interface->SetRegisterArrayByName("m2_window", 10 );
  m_device_interface->SetRegisterArrayByName("d_window", 20 );
  m_device_interface->SetRegisterArrayByName("i1_window", 40 );
  m_device_interface->SetRegisterArrayByName("disc_width", 10 );
  m_device_interface->SetRegisterArrayByName("baseline_start", 0x0000);
  //end of the ALL register sets from fcl file

  std::vector<unsigned int> vals;
  vals = {0xFF000000, 0x00000000, 0x00000FFF};
  m_device_interface->SetRegisterArrayByName("pdts_cmd_control",vals);
  vals.clear();

  vals = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
  m_device_interface->SetRegisterArrayByName("led_threshold",vals);
  vals.clear();
  //end of the ARR register sets from fcl file

  m_device_interface->SetRegisterByName(  "eventDataInterfaceSelect", 0x00000001);
  m_device_interface->SetRegisterByName("module_id", m_module_id);
  m_device_interface->SetRegisterByName("trigger_input_delay", 0x00000020);
  m_device_interface->SetRegisterByName("baseline_delay", 5);
  m_device_interface->SetRegisterByName("qi_config", 0x0FFF1300);
  m_device_interface->SetRegisterByName("qi_delay", 0x00000000);
  m_device_interface->SetRegisterByName("qi_pulse_width", 0x00008000);
  m_device_interface->SetRegisterByName("qi_dac_config", 0x00000000);
  m_device_interface->SetRegisterByName("external_gate_width", 0x00008000);
  m_device_interface->SetRegisterByName("gpio_output_width", 0x00001000);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::ConfigureDevice complete.";
}

void
SSPCardWrapper::build_channel_control_registers(const std::vector< std::pair<std::string,unsigned int>> entries, std::vector<unsigned int>& reg)
{
  for(auto ccIter=entries.begin();ccIter!=entries.end();++ccIter){
      //External trigger mode
      if(!ccIter->first.compare("ChannelControl_ExtTriggerMode")){
	unsigned int val = ccIter->second;
	switch(val){
	case 0:                          //No external trigger
	  for(unsigned int i=0;i<12;++i){
	    reg[i]=reg[i]&0xFFFF0FFF;
	  }
	  break;
	case 1:                          //Edge trigger on front panel
	  for(unsigned int i=0;i<12;++i){
	    reg[i]=(reg[i]&0xFFFF0FFF)+0x00006000;
	  }
	  break;
	case 2:                          //Use front panel as gate
	  for(unsigned int i=0;i<12;++i){
	    reg[i]=(reg[i]&0xFFFF0FFF)+0x00005000;
	  }
	  break;
	case 3:                          //Timestamp trigger
	  for(unsigned int i=0;i<12;++i){
	    reg[i]=(reg[i]&0xFFFF0FFF)+0x0000E000;
	  }
	  break;
	default:
	  try {
	    //DAQLogger::LogError("SSP_SSP_generator")<<"Error: invalid value for external trigger source setting!"<<std::endl;
	  } catch (...) {}
	  //throw SSPDAQ::EDAQConfigError("");
	}
      }
      else if(!ccIter->first.compare("ChannelControl_LEDTrigger")){
	unsigned int val=ccIter->second;
	switch(val){
	case 1:                          //Negative edge
	  for(unsigned int i=0;i<12;++i){
	    reg[i]=(reg[i]&0x7FFFF3FF)+0x80000800;
	  }
	  break;
	case 2:                          //Positive edge
	  for(unsigned int i=0;i<12;++i){
	    reg[i]=(reg[i]&0x7FFFF3FF)+0x80000400;
	  }
	  break;
	case 0:
	  for(unsigned int i=0;i<12;++i){//Disabled
	    reg[i]=reg[i]&0x7FFFF3FF;
	  }
	  break;
	default:
	  try {
	    //DAQLogger::LogError("SSP_SSP_generator")<<"Error: invalid value for trigger polarity setting!"<<std::endl;
	  } catch (...) {}
	  //throw SSPDAQ::EDAQConfigError("");
	}
      }
      else if(!ccIter->first.compare("ChannelControl_TimestampRate")){
	unsigned int val=ccIter->second;
	if(val>7){
	  try {
	    //DAQLogger::LogError("SSP_SSP_generator")<<"Error: invalid value for timestamp rate setting!"<<std::endl;
	  } catch (...) {}
	  //throw SSPDAQ::EDAQConfigError("");
	}
	for(unsigned int i=0;i<12;++i){
	  reg[i]=(reg[i]&0xFFFFFF1F)+0x20*val;
	}
      }
    }
}

void
SSPCardWrapper::configure_daq(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::ConfigureDAQ called.";
  m_cfg = args.get<sspcardreader::Conf>();
  m_pre_trig_length = m_cfg.pre_trig_length; //unsigned int preTrigLength=337500;
  if(m_pre_trig_length == 0){
    TLOG() << "Error: PreTrigger sample length (pre_trig_length) not defined in SSP DAQ configuration!" << std::endl;
    exit(424);
  }
  m_post_trig_length = m_cfg.post_trig_length; //unsigned int postTrigLength=412500;
  if(m_post_trig_length == 0){
    TLOG() << "Error: PostTrigger sample length (post_trig_length) not defined in SSP DAQ configuration!" << std::endl;
    exit(424);
    //throw SSPDAQ::EDAQConfigError("");
  }
  m_use_external_timestamp = m_cfg.use_external_timestamp; //unsigned int useExternalTimestamp=1;
  if(m_use_external_timestamp > 1){
    TLOG() << "Error: Timestamp source not definite (use_external_timestamp) , or invalidly defined in SSP DAQ configuration!" << std::endl;
    exit(424);
    //throw SSPDAQ::EDAQConfigError("");
  }
  m_trigger_write_delay = m_cfg.trigger_write_delay; //unsigned int triggerWriteDelay=1000;
  if(m_trigger_write_delay == 0){
    TLOG() << "Error: trigger write delay (trigger_write_delay) not defined in SSP DAQ configuration!" << std::endl;
    exit(424);
    //throw SSPDAQ::EDAQConfigError("");
  }
  m_trigger_latency = m_cfg.trigger_latency;//unsigned int trigLatency=0; //not sure about this.
  m_dummy_period = m_cfg.dummy_period;//int dummyPeriod=-1;//default to off which is set with a value of -1
  m_hardware_clock_rate_in_MHz = m_cfg.hardware_clock_rate_in_MHz; //unsigned int hardwareClockRate=150; //in MHz

  if(m_hardware_clock_rate_in_MHz == 0){
    TLOG() << "Error: Hardware clock rate (hardware_clock_rate_in_MHz) not defined in SSP DAQ configuration!"<<std::endl;
    exit(424);
    //throw SSPDAQ::EDAQConfigError("");
  }
  m_trigger_mask = m_cfg.trigger_mask; //unsigned int triggerMask=0x2000;
  m_fragment_timestamp_offset = m_cfg.fragment_timestamp_offset;//m_fragment_timestamp_offset=988;

  m_device_interface->SetPreTrigLength(m_pre_trig_length);
  m_device_interface->SetPostTrigLength(m_post_trig_length);
  m_device_interface->SetUseExternalTimestamp((bool)m_use_external_timestamp);
  m_device_interface->SetTriggerWriteDelay(m_trigger_write_delay);
  m_device_interface->SetTriggerLatency(m_trigger_latency);
  m_device_interface->SetDummyPeriod(m_dummy_period);
  m_device_interface->SetHardwareClockRateInMHz(m_hardware_clock_rate_in_MHz);
  m_device_interface->SetTriggerMask(m_trigger_mask);
  m_device_interface->SetFragmentTimestampOffset(m_fragment_timestamp_offset);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper::ConfigureDAQ complete.";
}

void
SSPCardWrapper::process_ssp()
{
  TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPCardWrapper starts processing blocks...";
  while (m_run_marker.load()) {

    bool hasSeenSlice = false;

    while (!hasSeenSlice) {

      std::vector<unsigned int> millislice;
      // JCF, Mar-8-2016
      // Could I just wrap this in a try-catch block?
      m_device_interface->ReadEvent(millislice);
      if (m_device_interface->exception()) {
        //set_exception(true);
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
      hasSeenSlice=true;

      TLOG_DEBUG(5) <<m_device_interface->GetIdentifier()
                         <<"Generator sending fragment "<<m_num_fragments_sent
                         <<", calls to GetNext "<<m_num_read_event_calls
                         <<", of which returned null "<<m_num_zero_fragments<<std::endl;

      // std::size_t dataLength = millislice.size()-dunedaq::dataformats::MillisliceHeader::sizeInUInts;

      //SSPFragment::Metadata metadata;
      //metadata.sliceHeader=*((SSPDAQ::MillisliceHeader*)(void*)millislice.data());
      //auto timestamp = (metadata.sliceHeader.triggerTime + fFragmentTimestampOffset_) / 3 ;
      //TLOG_DEBUG(TLVL_WORK_STEPS) << "SSP millislice w/ timestamp is " << timestamp
      //                                     << " millislice counter is "<< std::to_string(ncalls_with_millislice);

    }

    //SSPFragment::Metadata metadata;
    //metadata.sliceHeader=*((SSPDAQ::MillisliceHeader*)(void*)millislice.data());
    // We'll use the static factory function
    // artdaq::Fragment::FragmentBytes(std::size_t payload_size_in_bytes, sequence_id_t sequence_id,
    //  fragment_id_t fragment_id, type_t type, const T & metadata)
    // which will then return a unique_ptr to an artdaq::Fragment
    // object.
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // fix 5ms initial poll
  }
}

} // namespace sspmodules
} // namespace dunedaq

/*ssp101_standard: @local::ssp_standard
ssp101_standard.fragment_receiver.fragment_id: 11
ssp101_standard.fragment_receiver.board_id: 11
ssp101_standard.fragment_receiver.timing_address: 0x20
ssp101_standard.fragment_receiver.board_ip: "10.73.137.56"
ssp101_standard.fragment_receiver.HardwareConfig.module_id: 11
ssp101_standard.metrics.dim.IDName: "ssp101"

ssp304_standard: @local::ssp_standard
ssp304_standard.fragment_receiver.fragment_id: 34
ssp304_standard.fragment_receiver.board_id: 34
ssp304_standard.fragment_receiver.timing_address: 0x2B
ssp304_standard.fragment_receiver.board_ip: "10.73.137.79"
ssp304_standard.fragment_receiver.HardwareConfig.module_id: 34
ssp304_standard.metrics.dim.IDName: "ssp304"

ssp603_standard: @local::ssp_standard
ssp603_standard.fragment_receiver.fragment_id: 63
ssp603_standard.fragment_receiver.board_id: 63
ssp603_standard.fragment_receiver.timing_address: 0x36
ssp603_standard.fragment_receiver.board_ip: "10.73.137.74"
ssp603_standard.fragment_receiver.HardwareConfig.module_id: 63
ssp603_standard.metrics.dim.IDName: "ssp603" */

#endif // SSPMODULES_SRC_SSPCARDWRPPER_CPP_
