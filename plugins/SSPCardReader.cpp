/**
 * @file SSPCardReader.cc SSPCardReader DAQModule implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
//#include "ssplibs/sspcardreader/Nljs.hpp"

//#include "CreateElink.hpp"
#include "SSPCardReader.hpp"
//#include "SspIssues.hpp"

#include "logging/Logging.hpp"

//#include "flxcard/FlxException.h"

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "SSPCardReader" // NOLINT

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

SSPCardReader::SSPCardReader(const std::string& name)
  : DAQModule(name)
  , m_configured(false)
  , m_card_id(0)
  , m_logical_unit(0)
  , m_num_links(0)
  , m_block_size(0)
//, block_ptr_sinks_{ }

{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader constructor called.";
  m_card_wrapper = std::make_unique<SSPCardWrapper>();
  register_command("conf", &SSPCardReader::do_configure);
  register_command("start", &SSPCardReader::do_start);
  register_command("stop", &SSPCardReader::do_stop);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader constructor complete.";
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
SSPCardReader::init(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader init called.";
  auto ini = args.get<appfwk::app::ModInit>();
  m_card_wrapper->init(args);
  for (const auto& qi : ini.qinfos) {
    if (qi.dir != "output") {
      // ers::error(InitializationError(ERS_HERE, "Only output queues are supported in this module!"));
      continue;
    } else {
      TLOG_DEBUG(TLVL_WORK_STEPS) << ": CardReader output queue is " << qi.inst;
    }
  };

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader init complete.";
  // Set function for the CardWrapper's block processor.
  //m_card_wrapper->set_block_addr_handler(m_block_router);
}

void
SSPCardReader::do_configure(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader do_configure called.";
  m_card_wrapper->configure(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader do_configure complete.";
}

void
SSPCardReader::do_start(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader do_start called.";
  m_card_wrapper->start(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader do_start complete.";

}

void
SSPCardReader::do_stop(const data_t& args)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader do_stop called.";
  m_card_wrapper->stop(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardReader do_stop complete.";
}

void
SSPCardReader::get_info(opmonlib::InfoCollector& ci, int level)
{
}

} // namespace sspmodules
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::sspmodules::SSPCardReader)
