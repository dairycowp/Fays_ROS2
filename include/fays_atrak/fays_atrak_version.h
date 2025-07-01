/**
 * @file fays_atrak_version.h
 * @brief Provides functions to retrieve version information of the SDK.
 */

#ifndef _FAYS_ATRAK_SDK_VERSION_H_
#define _FAYS_ATRAK_SDK_VERSION_H_


#include "fays_tracker_version.h"


#define FAYS_ATRAK_VERSION_MAJOR 0
#define FAYS_ATRAK_VERSION_MINOR 7 
#define FAYS_ATRAK_VERSION_PATCH 0

#define FAYS_ATRAK_VERSION_STRING                 \
  FAYS_TO_STRING(FAYS_ATRAK_VERSION_MAJOR) "."    \
  FAYS_TO_STRING(FAYS_ATRAK_VERSION_MINOR) "."    \
  FAYS_TO_STRING(FAYS_ATRAK_VERSION_PATCH)

#endif /* _FAYS_ATRAK_SDK_VERSION_H_ */