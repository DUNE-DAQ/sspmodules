/**
 * @file SSPCardWrapper.cpp SSP library wrapper implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
// From Module
#include "SSPCardWrapper.hpp"

#include "logging/Logging.hpp"
//#include "packetformat/block_format.hpp"

// From STD
#include <chrono>
#include <iomanip>
#include <memory>

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
    m_run_marker{ false },
    m_ssp_processor(0)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper constructor called.";
  //instance_name_for_metrics_ = "SSP " + boost::lexical_cast<std::string>(board_id_);
  //unsigned int partitionNumber=ps.get<unsigned int>("partition_number",0);
  /*if(partitionNumber>3){
    try {
      DAQLogger::LogError("SSP_SSP_generator")<<"Error: Invalid partition number set ("<<partitionNumber<<")!"<<std::endl;
    } catch (...) {}
      throw SSPDAQ::EDAQConfigError("");
  }
  unsigned int timingAddress=ps.get<unsigned int>("timing_address",0);
  if(timingAddress>0xff){
    try {
      DAQLogger::LogError("SSP_SSP_generator")<<"Error: Invalid timing system address set ("<<timingAddress<<")!"<<std::endl;
    } catch (...) {}
    throw SSPDAQ::EDAQConfigError("");
  }
  unsigned int interfaceTypeCode(ps.get<unsigned int>("interface_type",999));
  switch(interfaceTypeCode){
  case 0:
    interface_type_=SSPDAQ::kUSB;
    break;
  case 1:
    interface_type_=SSPDAQ::kEthernet;
    break;
  case 2:
    interface_type_=SSPDAQ::kEmulated;
    break;
  case 999:
    throw art::Exception(art::errors::Configuration)
      <<"Interface type not defined in configuration fhicl file!\n";
  default:
    throw art::Exception(art::errors::Configuration)
      <<"Unknown interface type code "
      <<interfaceTypeCode
      <<".\n";
  }
  //Awful hack to get two devices to work together for now
  if(interface_type_!=1){
    board_id_=0;//std::stol(board_id_str);
  }
  else{
    board_id_=inet_network(ps.get<std::string>("board_ip").c_str());
    }*/
  
  //Right now we're just going to go ahead and try to hardcode the configuration of the board to be over ethernet and hardcode the board_id to be a specific ip address
  //device_interface_=new SSPDAQ::DeviceInterface(interface_type_,board_id_);//board_id_);
  //interface_type_ => SSPDAQ::kEthernet
  //board_id_ => 10.73.137.56 which is ssp101
  //10.73.137.79 is ssp304
  //10.73.137.74 is ssp603
  //I think in the old artdaq module there was a bad hack that changed the board_id_ value to be board_ip_ if the Comm_t was kEthernet, so that the board_id_ value wasn't actually used?
  board_id_=inet_network("10.73.137.56");
  interface_type_ = SSPDAQ::kEthernet;
  partitionNumber=999; //I think this should be less than 4, but I'm using what Giovanna sent me
  unsigned int timingAddress= 0x20;
  device_interface_=new SSPDAQ::DeviceInterface(interface_type_,board_id_);//board_id_);
  device_interface_->SetPartitionNumber(partitionNumber);
  device_interface_->SetTimingAddress(timingAddress);
  device_interface_->Initialize();

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper constructor complete.";
  
}

SSPCardWrapper::~SSPCardWrapper() 
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper destructor called.";
  close_card();
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper destructor complete.";
}

void
SSPCardWrapper::init(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper init called.";
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper init complete.";
}

void
SSPCardWrapper::configure(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper configure called.";
  this->ConfigureDAQ(args);
  this->ConfigureDevice(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper configure complete.";
}

void
SSPCardWrapper::start(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Starting SSPCardWrapper of card " << board_id_ << "...";
  if (!m_run_marker.load()) {
    set_running(true);
    m_ssp_processor.set_work(&SSPCardWrapper::process_SSP, this);
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Started CardWrapper of card " << board_id_ << "...";
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "CardWrapper of card " << board_id_ << " is already running!";
  }
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Starting SSPCardWrapper of card " << board_id_ << " complete.";
}

void
SSPCardWrapper::stop(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stopping SSPCardWrapper of card " << board_id_ << "...";
  if (m_run_marker.load()) {
    set_running(false);
    while (!m_ssp_processor.get_readiness()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Stopped CardWrapper of card " << board_id_ << "!";
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "CardWrapper of card " << board_id_ << " is already stopped!";
  } 
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "Stoping SSPCardWrapper of card " << board_id_ << " complete.";
}

void
SSPCardWrapper::set_running(bool should_run)
{
  bool was_running = m_run_marker.exchange(should_run);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "Active state was toggled from " << was_running << " to " << should_run;
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
SSPCardWrapper::ConfigureDevice(const data_t& args)
{
  //fhicl::ParameterSet hardwareConfig( ps.get<fhicl::ParameterSet>("HardwareConfig") );
  //std::vector<std::string> hcKeys=hardwareConfig.get_names();
  //Special case for channel_control register - first we
  //will get the literal register values, and then look
  //for logical settings in the .fcl and replace bits as appropriate.
  //std::vector<unsigned int> chControlReg(12,0);

  /*  bool haveChannelControl=false;
  for(auto hcIter=hcKeys.begin();hcIter!=hcKeys.end();++hcIter){
    
    //Deal with this later on
    if(!hcIter->compare("ChannelControl")){
      haveChannelControl=true;
    }
    //Expect to see a Literals section; parse these literal registers
    else if(!hcIter->compare("Literals")){
      fhicl::ParameterSet literalRegisters( hardwareConfig.get<fhicl::ParameterSet>("Literals") );
      std::vector<std::string> lrKeys=literalRegisters.get_names();
      for(auto lrIter=lrKeys.begin();lrIter!=lrKeys.end();++lrIter){
        std::vector<unsigned int> vals=literalRegisters.get<std::vector<unsigned int> >(*lrIter);
        unsigned int regVal=vals[0];
        unsigned int regMask=vals.size()>1?vals[1]:0xFFFFFFFF;
        unsigned int regAddress=0;
        std::istringstream(*lrIter)>>regAddress;
        
        device_interface_->SetRegister(regAddress,regVal,regMask);
      }
    }//End Processing of Literals
    //Intercept channel_control setting so that we can replace bits with logical values later...
    else if(!hcIter->substr(4).compare("channel_control")){
      if(!hcIter->substr(0,4).compare("ELE_")){
        std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
        chControlReg[vals[0]]=vals[1];
      }
      else if(!hcIter->substr(0,4).compare("ALL_")){ //All array elements set to same value
        unsigned int val=hardwareConfig.get<unsigned int>(*hcIter);
        for(unsigned int i=0;i<12;++i){
          chControlReg[i]=val;
        }
      }
      else if(!hcIter->substr(0,4).compare("ARR_")){ //All array elements individually
        std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
        for(unsigned int i=0;i<12;++i){
          chControlReg[i]=vals[i];
        }
      }
    }
    else if(!hcIter->substr(0,4).compare("ELE_")){ //Single array element
      std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
      device_interface_->SetRegisterElementByName(hcIter->substr(4,std::string::npos),vals[0],vals[1]);
    }
    else if(!hcIter->substr(0,4).compare("ALL_")){ //All array elements set to same value
      unsigned int val=hardwareConfig.get<unsigned int>(*hcIter);
      device_interface_->SetRegisterArrayByName(hcIter->substr(4,std::string::npos),val);
    }
    else if(!hcIter->substr(0,4).compare("ARR_")){ //All array elements individually
      std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
      device_interface_->SetRegisterArrayByName(hcIter->substr(4,std::string::npos),vals);
    }
    else{ //Individual register not in an array
      unsigned int val=hardwareConfig.get<unsigned int>(*hcIter);
      device_interface_->SetRegisterByName(*hcIter,val);
    }
  }
  //Modify channel control registers and send to hardware
  if(haveChannelControl){
    fhicl::ParameterSet channelControl( hardwareConfig.get<fhicl::ParameterSet>("ChannelControl"));
    this->BuildChannelControlRegisters(channelControl,chControlReg);
  }
  device_interface_->SetRegisterArrayByName("channel_control",chControlReg);
  */
  //JTH: Bit clumsy putting this here, but for now, configure the timing endpoint
  //and configure and reset dsp clock PLL...
  //Note this is hardcoding using the timing system clock. Using the internal clock
  //will be broken until this is cleaned up.
  //usleep(1000000);
  //device_interface_->SetRegisterByName("dsp_clock_control",0x31);
  //device_interface_->SetRegisterByName("dsp_clock_phase_control",0x4);
  
  //usleep(1000000);
  //device_interface_->SetRegisterByName("pdts_control",0x00080000);
  //device_interface_->SetRegisterByName("pdts_control",0x00000000);

  //this is all just doing it hardcoded

  //unsigned int val=hardwareConfig.get<unsigned int>(*hcIter);
  //device_interface_->SetRegisterArrayByName(hcIter->substr(4,std::string::npos),val);// for ALL configs not channel_control
  //std::vector<unsigned int> vals=hardwareConfig.get<std::vector<unsigned int> >(*hcIter);
  //device_interface_->SetRegisterArrayByName(hcIter->substr(4,std::string::npos),vals); //for ARR configs not channel_control
  //unsigned int val=hardwareConfig.get<unsigned int>(*hcIter);
  //device_interface_->SetRegisterByName(*hcIter,val); //for things without ARR, ALL, or ELE


  std::vector<unsigned int> chControlReg(12,0);
  for(unsigned int i=0;i<12;++i){
    chControlReg[i]=0x00000401;
  }
  device_interface_->SetRegisterArrayByName("channel_control",chControlReg); //set all channel control to 0x00000410
  device_interface_->SetRegisterArrayByName( "channel_control", 0x00000401);
  device_interface_->SetRegisterArrayByName(  "readout_window", 2000);
  device_interface_->SetRegisterArrayByName(  "readout_pretrigger", 50);
  device_interface_->SetRegisterArrayByName(  "cfd_parameters", 0x1800 );
  device_interface_->SetRegisterArrayByName(  "p_window", 0x20 );
  device_interface_->SetRegisterArrayByName(  "i2_window", 1200 );
  device_interface_->SetRegisterArrayByName(  "m1_window", 10 );
  device_interface_->SetRegisterArrayByName(  "m2_window", 10 );
  device_interface_->SetRegisterArrayByName(  "d_window", 20 );
  device_interface_->SetRegisterArrayByName(  "i1_window", 40 );
  device_interface_->SetRegisterArrayByName("disc_width", 10 );
  device_interface_->SetRegisterArrayByName("baseline_start", 0x0000); 
  //end of the ALL register sets from fcl file

  std::vector<unsigned int> vals;
  vals = {0xFF000000, 0x00000000, 0x00000FFF};
  device_interface_->SetRegisterArrayByName("pdts_cmd_control",vals);
  vals.clear();

  vals = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
  device_interface_->SetRegisterArrayByName("led_threshold",vals);
  vals.clear();
  //end of the ARR register sets from fcl file


  device_interface_->SetRegisterByName(  "eventDataInterfaceSelect", 0x00000001);
  device_interface_->SetRegisterByName("module_id", 0x00000001);
  device_interface_->SetRegisterByName("trigger_input_delay", 0x00000020);
  device_interface_->SetRegisterByName("baseline_delay", 5);
  device_interface_->SetRegisterByName("qi_config", 0x0FFF1300);
  device_interface_->SetRegisterByName("qi_delay", 0x00000000);
  device_interface_->SetRegisterByName("qi_pulse_width", 0x00008000);
  device_interface_->SetRegisterByName("qi_dac_config", 0x00000000);
  device_interface_->SetRegisterByName("external_gate_width", 0x00008000);
  device_interface_->SetRegisterByName("gpio_output_width", 0x00001000);

}

void
SSPCardWrapper::ConfigureDAQ(const data_t& args)
{
  //fhicl::ParameterSet daqConfig( ps.get<fhicl::ParameterSet>("DAQConfig") );
  //unsigned int preTrigLength=daqConfig.get<unsigned int>("PreTrigLength",0);
  unsigned int preTrigLength=337500; //Window length in ticks for packets to be included in a fragment. This is the length of the window before the trigger timestamp. look for @local::ssp_pretrigger_interval_6p67ns_ticks from important_parameters.fcl
  if(preTrigLength==0){
    
    try {
      //DAQLogger::LogError("SSP_SSP_generator")<<"Error: Pretrigger sample length not defined in SSP DAQ configuration!"<<std::endl;
    } catch (...) {}
    //throw SSPDAQ::EDAQConfigError("");
  }
  //unsigned int postTrigLength=daqConfig.get<unsigned int>("PostTrigLength",0);
  unsigned int postTrigLength=412500; //Length of the window after the trigger timestamp. Look for @local::ssp_posttrigger_interval_6p67ns_ticks from important_parameters.fcl

  if(postTrigLength==0){
    
    try {
      //DAQLogger::LogError("SSP_SSP_generator")<<"Error: Posttrigger sample length not defined in SSP DAQ configuration!"<<std::endl;
    } catch (...) {}
    //throw SSPDAQ::EDAQConfigError("");
  }
  //unsigned int useExternalTimestamp=daqConfig.get<unsigned int>("UseExternalTimestamp",2);
  unsigned int useExternalTimestamp=1;//Whether to use the external timestamp to determine whether to include packets in fragment. Both timestamps are stored in the packets anyway. 0 is front panel, 1 is NOvA style
  if(useExternalTimestamp>1){
    try{
      //DAQLogger::LogError("SSP_SSP_generator")<<"Error: Timestamp source not defined, or invalidly defined, in SSP DAQ configuration!"<<std::endl;
    } catch(...) {}
    //throw SSPDAQ::EDAQConfigError("");
  }
  //unsigned int triggerWriteDelay=daqConfig.get<unsigned int>("TriggerWriteDelay",0);
  unsigned int triggerWriteDelay=1000;
  if(triggerWriteDelay==0){
    try {
      //DAQLogger::LogError("SSP_SSP_generator")<<"TriggerWriteDelay not defined in SSP DAQ configuration!"<<std::endl;
    } catch(...) {}
    //throw SSPDAQ::EDAQConfigError("");
  }
  //unsigned int trigLatency=daqConfig.get<unsigned int>("TriggerLatency",0);
  //int dummyPeriod=daqConfig.get<int>("DummyTriggerPeriod",-1);
  //unsigned int hardwareClockRate=daqConfig.get<unsigned int>("HardwareClockRate",0);
  unsigned int trigLatency=0; //not sure about this.
  int dummyPeriod=-1;//default to off which is set with a value of -1
  unsigned int hardwareClockRate=150; //in MHz

  if(hardwareClockRate==1){
    try {
      //DAQLogger::LogError("SSP_SSP_generator")<<"Error: Hardware clock rate not defined in SSP DAQ configuration!"<<std::endl;
    } catch (...) {}
    //throw SSPDAQ::EDAQConfigError("");
  }
  //unsigned int triggerMask=daqConfig.get<unsigned int>("TriggerMask",0);
  //fFragmentTimestampOffset=daqConfig.get<int>("FragmentTimestampOffset",0);
  unsigned int triggerMask=0x2000;
  fFragmentTimestampOffset_=988;

  std::string triggerRequestAddress;
  // PAR 2019-08-21 TODO: It would be nice to use a more descriptive
  // parameter name here, but this is the parameter name that the RC
  // sets to the connection in our partition, so we have to use it
  // (for now)
  //triggerRequestAddress=daqConfig.get<std::string>("zmq_fragment_connection_out","");
  triggerRequestAddress="tcp://10.73.136.32:7123";

  device_interface_->SetPreTrigLength(preTrigLength);
  device_interface_->SetPostTrigLength(postTrigLength);
  device_interface_->SetUseExternalTimestamp(useExternalTimestamp);
  device_interface_->SetTriggerWriteDelay(triggerWriteDelay);
  device_interface_->SetTriggerLatency(trigLatency);
  device_interface_->SetDummyPeriod(dummyPeriod);
  device_interface_->SetHardwareClockRateInMHz(hardwareClockRate);
  device_interface_->SetTriggerMask(triggerMask);
  device_interface_->SetFragmentTimestampOffset(fFragmentTimestampOffset_);
  //if(triggerRequestAddress.length()){
  //device_interface_->StartRequestReceiver(triggerRequestAddress);
  //} //I believe that this is all taken care of by the new framework
  
}  

void
SSPCardWrapper::process_SSP()
{
  TLOG_DEBUG(TLVL_WORK_STEPS) << "SSPCardWrapper starts processing blocks...";
  while (m_run_marker.load()) {

    bool hasSeenSlice = false;

    while (!hasSeenSlice) {
    
      std::vector<unsigned int> millislice;
      // JCF, Mar-8-2016
      // Could I just wrap this in a try-catch block?
      device_interface_->ReadEvents(millislice);
      if (device_interface_->exception())
	{
	  //set_exception(true);
	  TLOG_DEBUG(TLVL_WORK_STEPS) << "dune::SSP::getNext_ : found device interface thread in exception state";
	}
      
      static size_t ncalls = 1;
      static size_t ncalls_with_millislice = 0;
      
      if (millislice.size() > 0) {
	ncalls_with_millislice++;
      }
      ncalls++;

      if (millislice.size()==0) {
	if (!hasSeenSlice){
	  ++fNNoFragments_;
	  usleep(100000);
	}
	break;
      }
      hasSeenSlice=true;

      TLOG_DEBUG(TLVL_FULL_DEBUG) <<device_interface_->GetIdentifier()
                         <<"Generator sending fragment "<<fNFragmentsSent_
                         <<", calls to GetNext "<<fNReadEventCalls_
                         <<", of which returned null "<<fNNoFragments_<<std::endl;                         
      
      std::size_t dataLength = millislice.size()-SSPDAQ::MillisliceHeader::sizeInUInts;

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
    std::this_thread::sleep_for(std::chrono::microseconds(5000)); // fix 5ms initial poll
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
