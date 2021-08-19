/**
 * @file SSPCardWrapper.cpp SSP library wrapper implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
// From Module
#include "SSPCardWrapper.hpp"

#include "logging/Logging.hpp"
//#include "packetformat/block_format.hpp"

// From STD
#include <chrono>
#include <iomanip>
#include <memory>

/**
 * @brief TRACE debug levels used in this source file
 */
enum
{
  TLVL_ENTER_EXIT_METHODS = 5,
  TLVL_WORK_STEPS = 10,
  TLVL_BOOKKEEPING = 15
};

namespace dunedaq {
namespace sspmodules {

SSPCardWrapper::SSPCardWrapper()
{}

SSPCardWrapper::~SSPCardWrapper() 
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper destructor called. First stop check, then closing card.";
  close_card();
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSPCardWrapper destroyed.";
}

void
SSPCardWrapper::init(const data_t& /*args*/)
{
}

void
SSPCardWrapper::configure(const data_t& args)
{

}

void
SSPCardWrapper::start(const data_t& /*args*/)
{
}

void
SSPCardWrapper::stop(const data_t& /*args*/)
{
}

void
SSPCardWrapper::set_running(bool should_run)
{
}

void
SSPCardWrapper::open_card()
{
}

void
SSPCardWrapper::close_card()
{
}

} // namespace sspmodules
} // namespace dunedaq
