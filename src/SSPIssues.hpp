/**
 * @file SSPIssues.hpp SSP related ERS issues
 *
 * This is part of the DUNE DAQ , copyright 2021.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_SSPISSUES_HPP_
#define SSPMODULES_SRC_SSPISSUES_HPP_

#include <ers/Issue.hpp>

#include <string>

namespace dunedaq {

ERS_DECLARE_ISSUE(sspmodules, ConfigurationError, "SSP Configuration Error: " << conferror, ((std::string)conferror))

ERS_DECLARE_ISSUE(sspmodules,
		  FailedLEDCalibrationInit,
		  "LED calibration failed to initialize",
		  ERS_EMPTY)

ERS_DECLARE_ISSUE(sspmodules,
                  FailedLEDCalibrationConf,
                  "LED calibration failed to configure",
                  ERS_EMPTY)

ERS_DECLARE_ISSUE(sspmodules,
                  DeviceInterfacePDTSStatus,
                  "Endpoint failed to reach 0x8",
                  ERS_EMPTY)

} // namespace dunedaq

#endif // SSPMODULES_SRC_SSPISSUES_HPP_
