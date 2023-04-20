/**
 * @file EventPacket.h
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_EVENTPACKET_HPP_
#define SSPMODULES_SRC_ANLBOARD_EVENTPACKET_HPP_

#include "fddetdataformats/SSPTypes.hpp"

#include <vector>
#include <sstream>
#include <utility>

namespace dunedaq {
namespace sspmodules {


//Simple bag of data but with implementation of move methods
//to allow efficient shifting around of data between containers
class EventPacket{
public:

  //Move constructor
  EventPacket(EventPacket&& rhs){
    data=std::move(rhs.data);
    header=rhs.header;
  }

  //Move assignment operator
  EventPacket& operator=(EventPacket&& rhs){
    data=std::move(rhs.data);
    header=rhs.header;
    return *this;
  }

  //Copy constructor
  EventPacket(const EventPacket& rhs){
    data=rhs.data;
    header=rhs.header;
  }

  //Copy assignment operator
  EventPacket& operator=(const EventPacket& rhs){
    data=rhs.data;
    header=rhs.header;
    return *this;
  }

  EventPacket(){}

  //Clear data vector and set header word to 0xDEADBEEF
  void SetEmpty();

  void DumpHeader();

  void DumpEvent();

  dunedaq::fddetdataformats::ssp::EventHeader header;

  std::vector<unsigned int> data;
};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_ANLBOARD_EVENTPACKET_HPP_
