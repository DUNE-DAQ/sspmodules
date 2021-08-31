/**
 * @file SSPCardWrapper.hpp SSP library wrapper
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_CARDWRAPPER_HPP_
#define SSPMODULES_SRC_CARDWRAPPER_HPP_

#include "readout/utils/ReusableThread.hpp"
//#include "dune-raw-data/Overlays/SSPFragment.hh"
#include "anlBoard/DeviceInterface.h"

//#include "packetformat/block_format.hpp"

#include <nlohmann/json.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace dunedaq::sspmodules {

class SSPCardWrapper
{
public:
  /**
   * @brief SSPCardWrapper Constructor
   * @param name Instance name for this CardWrapper instance
   */
  SSPCardWrapper();
  ~SSPCardWrapper();
  SSPCardWrapper(const SSPCardWrapper&) = delete;            ///< SSPCardWrapper is not copy-constructible
  SSPCardWrapper& operator=(const SSPCardWrapper&) = delete; ///< SSPCardWrapper is not copy-assignable
  SSPCardWrapper(SSPCardWrapper&&) = delete;                 ///< SSPCardWrapper is not move-constructible
  SSPCardWrapper& operator=(SSPCardWrapper&&) = delete;      ///< SSPCardWrapper is not move-assignable

  using data_t = nlohmann::json;
  void init(const data_t& args);
  void configure(const data_t& args);
  void start(const data_t& args);
  void stop(const data_t& args);
  void set_running(bool should_run);

private:
  // Types
  //using module_conf_t = dunedaq::sspmodules::sspcardreader::Conf;

  //these are SSP configurations for this instance of the SSP Card Wrapper
  //dune::detail::FragmentType const fragment_type_; // Type of fragment (see FragmentType.hh), //KIRBY this is something I don't know how to transition to the new framework

  unsigned int board_id_;
  SSPDAQ::Comm_t  interface_type_; //is this ethernet or USB or emulated, note that USB is needed for interfacing with the board registers
  unsigned int partitionNumber;

  SSPDAQ::DeviceInterface* device_interface_;
  readout::ReusableThread m_ssp_processor;
  std::atomic<bool> m_run_marker;

  //sspmodules::DeviceInterface* device_interface_; //KIRBY this is the thing in artdaq that did all of the heavy lifting

  // Tracking metrics for debugging and checking consistency
  unsigned long fNNoFragments_;
  unsigned long fNFragmentsSent_;
  unsigned long fNReadEventCalls_;
  int fFragmentTimestampOffset_;
  //std::string instance_name_for_metrics_;

  // Card
  void open_card();
  void close_card();
  void ConfigureDAQ(const data_t& args);
  void ConfigureDevice(const data_t& args);
  void process_SSP();

  //module_conf_t m_cfg;

};

} // namespace dunedaq::sspmodules

#endif // SSPMODULES_SRC_CARDWRAPPER_HPP_
