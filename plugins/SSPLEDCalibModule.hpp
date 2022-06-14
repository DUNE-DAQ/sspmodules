/**
 * @file SSPLEDCalibModule.hpp SSP card reader DAQ Module.
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_HPP_
#define SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_HPP_

#include "appfwk/app/Nljs.hpp"
#include "appfwk/cmd/Nljs.hpp"
#include "appfwk/cmd/Structs.hpp"

// From appfwk
#include "appfwk/DAQModule.hpp"
#include  "iomanager/Sender.hpp"
#include "utilities/WorkerThread.hpp"
//#include "appfwk/ThreadHelper.hpp"

// From readout
#include "readoutlibs/utils/ReusableThread.hpp"

#include "SSPLEDCalibWrapper.hpp"

#include <future>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dunedaq::sspmodules {

class SSPLEDCalibModule : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief SSPLEDCalibModule Constructor
   * @param name Instance name for this SSPLEDCalibModule instance
   */
  explicit SSPLEDCalibModule(const std::string& name);

  SSPLEDCalibModule(const SSPLEDCalibModule&) = delete;            ///< SSPLEDCalibModule is not copy-constructible
  SSPLEDCalibModule& operator=(const SSPLEDCalibModule&) = delete; ///< SSPLEDCalibModule is not copy-assignable
  SSPLEDCalibModule(SSPLEDCalibModule&&) = delete;                 ///< SSPLEDCalibModule is not move-constructible
  SSPLEDCalibModule& operator=(SSPLEDCalibModule&&) = delete;      ///< SSPLEDCalibModule is not move-assignable

  void init(const data_t& args) override;

private:
  // Types
  using module_conf_t = dunedaq::sspmodules::sspledcalibmodule::Conf;

  // Commands
  void do_configure(const data_t& args);
  void do_start(const data_t& args);
  void do_stop(const data_t& args);
  void do_start_pulses(const data_t& args);
  void do_stop_pulses(const data_t& args);
  void get_info(opmonlib::InfoCollector& ci, int level);
  void do_single_pulse_config(const data_t& args);
  void do_burst_mode_config(const data_t& args);

  // Configuration
  module_conf_t m_cfg;
  bool m_configured;
  int m_card_id;

  // SSP Cards
  std::unique_ptr<SSPLEDCalibWrapper> m_card_wrapper;
};

} // namespace dunedaq::sspmodules

#endif // SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_HPP_
