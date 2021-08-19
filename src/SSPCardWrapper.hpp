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
  //using module_conf_t = dunedaq::sspmodules::felixcardreader::Conf;

  // Constants
  //static constexpr size_t m_max_links_per_card = 6;
  // static constexpr size_t m_margin_blocks = 4;
  // static constexpr size_t m_block_threshold = 256;
  //static constexpr size_t m_block_size = 4096; // felix::packetformat::BLOCKSIZE;
  //static constexpr size_t m_dma_wraparound = FLX_DMA_WRAPAROUND;

  // Card
  void open_card();
  void close_card();

  //module_conf_t m_cfg;

};

} // namespace dunedaq::sspmodules

#endif // SSPMODULES_SRC_CARDWRAPPER_HPP_
