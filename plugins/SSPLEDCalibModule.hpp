/**
 * @file SSPLEDCalibModule.hpp SSP card reader DAQ Module.
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_HPP_
#define SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_HPP_


// From appfwk
#include "appfwk/DAQModule.hpp"
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

  void init(std::shared_ptr<appfwk::ConfigurationManager> mcfg) override;

private:

  // Commands
  void do_configure(const CommandData_t& /*args*/);
  void do_start(const CommandData_t& args);
  void do_stop(const CommandData_t& args);

  std::shared_ptr<appfwk::ConfigurationManager> m_mcfg;

  // SSP Cards
  std::unique_ptr<SSPLEDCalibWrapper> m_card_wrapper;
};

} // namespace dunedaq::sspmodules

#endif // SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_HPP_
