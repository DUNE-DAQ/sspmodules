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

namespace dunedaq {
namespace sspmodules {

SSPCardWrapper::SSPCardWrapper()
{

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

}

SSPCardWrapper::~SSPCardWrapper() 
{
  close_card();
}

void
SSPCardWrapper::init(const data_t& /*args*/)
{

}

void
SSPCardWrapper::configure(const data_t& args)
{

}

void
SSPCardWrapper::start(const data_t& /*args*/)
{
}

void
SSPCardWrapper::stop(const data_t& /*args*/)
{
}

void
SSPCardWrapper::set_running(bool should_run)
{
}

void
SSPCardWrapper::open_card()
{
}

void
SSPCardWrapper::close_card()
{
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
