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
		  FailedLEDCalibration,
		  "LED calibration failed to configure",
		  ERS_EMPTY)

} // namespace dunedaq

#endif // SSPMODULES_SRC_SSPISSUES_HPP_
