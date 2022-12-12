/**
 * @file anlExceptions.h

 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_ANLEXCEPTIONS_HPP_
#define SSPMODULES_SRC_ANLBOARD_ANLEXCEPTIONS_HPP_

#include <stdexcept>
#include <string>

namespace dunedaq {
namespace sspmodules {

//================================//
// Bad requests from Device Manager//
//================================//

class ENoSuchDevice : public std::runtime_error
{
public:
  explicit ENoSuchDevice(const std::string& s)
    : std::runtime_error(s)
  {
  }

  ENoSuchDevice()
    : std::runtime_error("")
  {
  }
};

class EDeviceAlreadyOpen : public std::runtime_error
{
public:
  explicit EDeviceAlreadyOpen(const std::string& s)
    : std::runtime_error(s)
  {
  }

  EDeviceAlreadyOpen()
    : std::runtime_error("")
  {
  }
};

class EBadDeviceList : public std::runtime_error
{
public:
  explicit EBadDeviceList(const std::string& s)
    : std::runtime_error(s)
  {
  }

  EBadDeviceList()
    : std::runtime_error("")
  {
  }
};

//=======================================//
// Error reported by USB interface library//
//=======================================//

class EFTDIError : public std::runtime_error
{
public:
  explicit EFTDIError(const std::string& s)
    : std::runtime_error(s)
  {
  }

  EFTDIError()
    : std::runtime_error("")
  {
  }
};

//=======================================//
// Error reported by TCP interface library//
//=======================================//

class ETCPError : public std::runtime_error
{
public:
  explicit ETCPError(const std::string& s)
    : std::runtime_error(s)
  {
  }

  ETCPError()
    : std::runtime_error("")
  {
  }
};

//===============================================//
// Error receiving expected event data from device//
//===============================================//

class EEventReadError : public std::runtime_error
{
public:
  explicit EEventReadError(const std::string& s)
    : std::runtime_error(s)
  {
  }

  EEventReadError()
    : std::runtime_error("")
  {
  }
};

class EDAQConfigError : public std::runtime_error
{
public:
  explicit EDAQConfigError(const std::string& s)
    : std::runtime_error(s)
  {
  }

  EDAQConfigError()
    : std::runtime_error("")
  {
  }
};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_ANLBOARD_ANLEXCEPTIONS_HPP_
