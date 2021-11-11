/**
 * @file SSPCardWrapper.hpp SSP library wrapper
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_SSPCARDWRAPPER_HPP_
#define SSPMODULES_SRC_SSPCARDWRAPPER_HPP_

#include "sspmodules/sspcardreader/Nljs.hpp"

#include "detdataformats/ssp/SSPTypes.hpp"

#include "SSPIssues.hpp"
#include "anlBoard/DeviceInterface.hpp"
#include "logging/Logging.hpp"
#include "readout/utils/ReusableThread.hpp"

#include <nlohmann/json.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace sspmodules {

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
  using module_conf_t = dunedaq::sspmodules::sspcardreader::Conf;

  // these are SSP configurations for this instance of the SSP Card Wrapper
  // dune::detail::FragmentType const fragment_type_; // Type of fragment (see FragmentType.hh), //KIRBY this is
  // something I don't know how to transition to the new framework
  dunedaq::sspmodules::DeviceInterface* m_device_interface; // instance of the SSP device interface class
  readout::ReusableThread m_ssp_processor; // reusable thread used to start the work done by the device interface

  // module status booleans for transition like init, conf, start, etc
  std::atomic<bool> m_run_marker;
  std::atomic<bool> m_configure;

  // all of the configure variables for the SSP
  module_conf_t m_cfg;

  // Initialization configuration variables
  unsigned int m_board_id {0};  // this is the ID of the SSP board
  unsigned long m_board_ip; // this is the ip address of the SSP board // NOLINT
  unsigned int m_partition_number {0};
  unsigned int m_timing_address {0};
  unsigned long m_module_id; // NOLINT

  // DAQ configuration variables first
  unsigned int m_pre_trig_length {0}; // Window length in ticks for packets to be included in a fragment. This is the length
                                  // of the window before the trigger timestamp.
  unsigned int m_post_trig_length {0};       // Length of the window after the trigger timestamp.
  unsigned int m_use_external_timestamp {2}; // Whether to use the external timestamp to determine whether to include
                                         // packets in fragment. Both timestamps are stored in the packets anyway. 0 is
                                         // front panel, 1 is NOvA style
  unsigned int m_trigger_write_delay {0};
  unsigned int m_trigger_latency {0};
  int m_dummy_period {-1};
  unsigned int m_hardware_clock_rate_in_MHz {0}; // clock rate on the hardware in MHz
  unsigned int m_trigger_mask {0};     // this is normally given as a hex number for what triggers to mask on or off
  int m_fragment_timestamp_offset {0}; // offset for the timestamp put into the data stream of this SSP

  // Tracking metrics for debugging and checking consistency
  unsigned long m_num_zero_fragments;   // NOLINT
  unsigned long m_num_fragments_sent;   // NOLINT
  unsigned long m_num_read_event_calls; // NOLINT
  std::string m_instance_name_for_metrics;

  // Card
  void open_card();
  void close_card();
  void configure_daq(const data_t& args);
  void configure_device(const data_t& args);
  void build_channel_control_registers(const std::vector<std::pair<std::string, unsigned int>> entries,
                                       std::vector<unsigned int>& reg);
  void process_ssp();
};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_SSPCARDWRAPPER_HPP_
