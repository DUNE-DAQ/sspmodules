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

#include "sspmodules/sspledcalibmodule/Nljs.hpp"

#include "SSPLEDCalibModule.hpp"
//#include "SspIssues.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <utility>
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
  , m_configured(false)
  , m_card_id(0)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule constructor called.";
  m_card_wrapper = std::make_unique<SSPLEDCalibWrapper>();

  register_command("conf", &SSPLEDCalibModule::do_configure);
  register_command("start", &SSPLEDCalibModule::do_start);
  register_command("stop", &SSPLEDCalibModule::do_stop);

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule constructor complete.";
}

inline void
tokenize(std::string const& str, const char delim, std::vector<std::string>& out)
{
  std::size_t start;
  std::size_t end = 0;
  while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
    end = str.find(delim, start);
    out.push_back(str.substr(start, end - start));
  }
}

void
SSPLEDCalibModule::init(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule init called.";
  auto ini = args.get<appfwk::app::ModInit>();
  m_card_wrapper->init(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule init complete.";
  // Set function for the SSPLEDCalibWrapper's block processor.
  // m_card_wrapper->set_block_addr_handler(m_block_router);
}

void
SSPLEDCalibModule::do_configure(const data_t& args)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule do_configure called.";
  if (!m_configured) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << ": SSPLEDCalibModule not yet configured so digging in.";
    m_cfg = args.get<dunedaq::sspmodules::sspledcalibmodule::Conf>();
    m_card_id = m_cfg.card_id;
    m_card_wrapper->configure(args);
    TLOG_DEBUG(TLVL_WORK_STEPS) << ": SSPLEDCalibModule do_configure coming back from SSPLEDCalibWrapper configure.";
    m_configured = true;
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << ": SSPLEDCalibModule is already configured so not going to configure again.";
  }

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPLEDCalibModule do_configure complete.";
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

void
SSPLEDCalibModule::get_info(opmonlib::InfoCollector& /*ci*/, int /*level*/)
{
}

} // namespace sspmodules
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::sspmodules::SSPLEDCalibModule)

#endif // SSPMODULES_PLUGINS_SSPLEDCALIBMODULE_CPP_
