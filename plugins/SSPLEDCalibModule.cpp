/**
 * @file SSPLEDCalibModule.cpp SSPLEDCalibModule DAQModule implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_CPP_
#define SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_CPP_

#include "logging/Logging.hpp"
#include "appmodel/SSPLEDCalibModule.hpp"
#include "SSPLEDCalibModule.hpp"
//#include "SspIssues.hpp"

#include <vector>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "SSPLEDCalibModule" // NOLINT

/**
 * @brief TRACE debug levels used in this source file
 */

enum
{
  TLVL_ENTER_EXIT_METHODS = 5,
  TLVL_WORK_STEPS = 10,
  TLVL_BOOKKEEPING = 15,
  TLVL_FULL_DEBUG = 63
};

namespace dunedaq {
namespace sspmodules {

SSPLEDCalibModule::SSPLEDCalibModule(const std::string& name)
  : DAQModule(name)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule constructor called.";
  m_card_wrapper = std::make_unique<SSPLEDCalibWrapper>();

  register_command("conf", &SSPLEDCalibModule::do_configure);
  register_command("start", &SSPLEDCalibModule::do_start);
  register_command("stop", &SSPLEDCalibModule::do_stop);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule constructor complete.";
}

void
SSPLEDCalibModule::init(std::shared_ptr<appfwk::ConfigurationManager> mcfg)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule init called.";

  m_mcfg = mcfg;
  auto conf = mcfg->get_dal<appmodel::SSPLEDCalibModule>(get_name());

  m_card_wrapper->init(conf);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule init complete.";
}

void
SSPLEDCalibModule::do_configure(const data_t& /*args*/)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule conf called.";

  if (!m_mcfg) {
    std::stringstream ss;
    ss << "Error: The configuration was not properly stored!" << std::endl;
    throw ConfigurationError(ERS_HERE, ss.str());
  }

  auto conf = m_mcfg->get_dal<appmodel::SSPLEDCalibModule>(get_name());

  m_card_wrapper->conf(conf);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule conf complete.";
}

void
SSPLEDCalibModule::do_start(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule do_start called.";
  m_card_wrapper->start(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule do_start complete.";
}


void
SSPLEDCalibModule::do_stop(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule do_stop called.";
  m_card_wrapper->stop(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule do_stop complete.";
}

} // namespace sspmodules
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::sspmodules::SSPLEDCalibModule)

#endif // SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_CPP_
