/**
 * @file SSPLEDCalibWrapper.hpp SSP library wrapper
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_SSPLEDCALIBWRAPPER_HPP_
#define SSPMODULES_SRC_SSPLEDCALIBWRAPPER_HPP_

#include "sspmodules/sspledcalibmodule/Nljs.hpp"

#include "detdataformats/ssp/SSPTypes.hpp"

#include "SSPIssues.hpp"
#include "anlBoard/DeviceInterface.hpp"
#include "logging/Logging.hpp"
#include "readoutlibs/utils/ReusableThread.hpp"

#include <nlohmann/json.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace sspmodules {

class SSPLEDCalibWrapper
{
public:
  /**
   * @brief SSPLEDCalibWrapper Constructor
   * @param name Instance name for this CardWrapper instance
   */
  SSPLEDCalibWrapper();
  ~SSPLEDCalibWrapper();
  SSPLEDCalibWrapper(const SSPLEDCalibWrapper&) = delete;            ///< SSPLEDCalibWrapper is not copy-constructible
  SSPLEDCalibWrapper& operator=(const SSPLEDCalibWrapper&) = delete; ///< SSPLEDCalibWrapper is not copy-assignable
  SSPLEDCalibWrapper(SSPLEDCalibWrapper&&) = delete;                 ///< SSPLEDCalibWrapper is not move-constructible
  SSPLEDCalibWrapper& operator=(SSPLEDCalibWrapper&&) = delete;      ///< SSPLEDCalibWrapper is not move-assignable

  using data_t = nlohmann::json;
  void init(const data_t& args);
  void configure(const data_t& args);
  void start(const data_t& args);
  void stop(const data_t& args);
  
private:
  // Types
  using module_conf_t = dunedaq::sspmodules::sspledcalibmodule::Conf;
  // these are SSP configurations for this instance of the SSP LED Calib Wrapper
  dunedaq::sspmodules::DeviceInterface* m_device_interface; // instance of the SSP device interface class

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
  bool m_burst_mode = false;
  bool m_double_pulse = false;
  bool m_single_pulse = false;
  unsigned int m_channel_mask{4095};
  unsigned int m_burst_count{1};
  unsigned int m_double_pulse_delay_ticks{0};
  unsigned int m_pulse1_width_ticks{0};
  unsigned int m_pulse2_width_ticks{0};
  unsigned int m_pulse_bias_percent_270nm{0};
  unsigned int m_pulse_bias_percent_367nm{0};
  
  std::string m_instance_name_for_metrics;

  // Card
  void validate_config(const data_t& args);
  //void close_card();
  //void configure_daq(const data_t& args);
  void configure_single_pulse();
  void configure_burst_mode();
  void manual_configure_device(const data_t& args);
  //void build_channel_control_registers(const std::vector<std::pair<std::string, unsigned int>> entries,
  //                                     std::vector<unsigned int>& reg);
  //void process_ssp();
};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_SSPLEDCALIBWRAPPER_HPP_
