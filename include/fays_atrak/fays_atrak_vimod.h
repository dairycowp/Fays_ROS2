/**
 * @file fays_atrak_vimod.h
 * @brief Used to get sensor datas and set sensor properties.
 */

#pragma once


#include "fays_atrak_types.h"


#if (defined (_WIN32) || defined(WIN64))
	#define FAYS_ATRAK_API __declspec(dllexport)
#else
    #define FAYS_ATRAK_API __attribute__((visibility("default")))
#endif


#ifdef __cplusplus
extern "C" {
#endif 


/****************************************************************************
 *  standard Fays AI Module SDK
 ****************************************************************************/

/**
 * @brief Open sensor .
 * 
 * @param[in] configuration The configuration file path.
 * @param[out] handle
 * @return Creation success or failure.
 */
FAYS_ATRAK_API      int     FAYS_ATRAK_AIM_CreateHandleWithConfig           (void** handle, const char* configPath);

/**
 * @brief Destroy the handle and turn off the sensors.
 * 
 * @param[out] handle
 * @return Destruction success or failure.
 */
FAYS_ATRAK_API      int     FAYS_ATRAK_AIM_DestroyHandle                    (void* handle);

/**
 * @brief Get grayscale camera image (The image is stitched together).
 * 
 * @param[in] handle
 * @param[out] image Stereo camera grayscale image.
 * @return Whether the image acquisition is successful or not.
 */
FAYS_ATRAK_API      int     FAYS_ATRAK_AIM_GetStereoFrames                  (void* handle, AtrakImage* Pimage);

/**
 * @brief Set the Gain of the stereo camera.
 *
 * @param[in] handle
 * @param[in] value Target gain value.
 * @note Only supports setting gain value through root.
 * @note Gain range: 1.0 ~ 16.0.
 * @return Set whether gain is successful or not.
 */
FAYS_ATRAK_API      int     FAYS_ATRAK_AIM_SetStereoGain                    (void* handle, float gainValue);

/**
 * @brief Set the exposure time of the stereo camera.
 * 
 * @param[in] handle
 * @param[in] value Target exposure value. 
 * @note Only supports setting exposure value through root.
 * @note Exposure range: 1.0 ~ 16.0.
 * @return Set whether exposure is successful or not.
 */
FAYS_ATRAK_API      int     FAYS_ATRAK_AIM_SetStereoExposure                (void* handle, double exposureValue);

/**
 * @brief Set the camera frame rate to 30FPS.
 * 
 * @param[in] handle
 * @note Only supports setting FPS through root.
 * @note Currently only supports setting to 30FPS
 * @note After the stereo camera is powered off and restarted, it will be restored to 60 FPS by default.
 * @return Set whether FPS is successful or not.
 */
FAYS_ATRAK_API      int     FAYS_ATRAK_AIM_SetStereoFPS                     (void* handle);

/**
 * @brief Get imu data.
 * 
 * @param[in] handle
 * @param[out] imu 6-axis imu data.
 * @return Get whether IMU data is successful or not.
 */
FAYS_ATRAK_API      int     FAYS_ATRAK_AIM_GetImuData                       (void* handle, AtrakIMU* Pimu);

#ifdef __cplusplus
}
#endif 

