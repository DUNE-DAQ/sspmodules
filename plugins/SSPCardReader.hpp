/**
 * @file SSPCardReader.hpp SSP card reader DAQ Module.
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_PLUGINS_SSPCARDREADER_HPP_
#define SSPMODULES_PLUGINS_SSPCARDREADER_HPP_

#include "appfwk/app/Nljs.hpp"
#include "appfwk/cmd/Nljs.hpp"
#include "appfwk/cmd/Structs.hpp"

//#include "sspmodules/felixcardreader/Structs.hpp"

// From appfwk
#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/ThreadHelper.hpp"

// From readout
#include "readout/utils/ReusableThread.hpp"

// FELIX Software Suite provided
//#include "packetformat/block_format.hpp"

#include "SSPCardWrapper.hpp"
//#include "ElinkConcept.hpp"

#include <future>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dunedaq::sspmodules {

class SSPCardReader : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief SSPCardReader Constructor
   * @param name Instance name for this SSPCardReader instance
   */
  explicit SSPCardReader(const std::string& name);

  SSPCardReader(const SSPCardReader&) = delete;            ///< SSPCardReader is not copy-constructible
  SSPCardReader& operator=(const SSPCardReader&) = delete; ///< SSPCardReader is not copy-assignable
  SSPCardReader(SSPCardReader&&) = delete;                 ///< SSPCardReader is not move-constructible
  SSPCardReader& operator=(SSPCardReader&&) = delete;      ///< SSPCardReader is not move-assignable

  void init(const data_t& args) override;

private:
  // Types
  using module_conf_t = dunedaq::sspmodules::sspcardreader::Conf;

  // Constants
  //static constexpr int s_elink_multiplier = 64;

  // Commands
  void do_configure(const data_t& args);
  void do_start(const data_t& args);
  void do_stop(const data_t& args);
  void get_info(opmonlib::InfoCollector& ci, int level);

  // Configuration
  module_conf_t m_cfg;
  bool m_configured;
  int m_card_id;

  // SSP Cards
  std::unique_ptr<SSPCardWrapper> m_card_wrapper;

};

} // namespace dunedaq::sspmodules

#endif // SSPMODULES_PLUGINS_SSPCARDREADER_HPP_
