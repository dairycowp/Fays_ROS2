/**
 * @file fays_atrak_types.h
 * @brief Provide SDK structure, enumeration constant definition, data type definition.
 */

#pragma once

#include <stdint.h>

#define FAYS_ATRAK_MAX_TRACK_INSTANCES 1
#define FAYS_ATRAK_MAX_APRIL_INSTANCES 10
#define FAYS_ATRAK_MONO_MAX_BYTES (640 * 400 * 2)
#define FAYS_ATRAK_RGB_MAX_BYTES (5000 * 4000 * 3)
#if FAYS_ATRAK_MONO_MAX_BYTES > FAYS_ATRAK_RGB_MAX_BYTES
#   define FAYS_ATRAK_IMG_MAX_BYTES FAYS_ATRAK_MONO_MAX_BYTES
#else
#   define FAYS_ATRAK_IMG_MAX_BYTES FAYS_ATRAK_RGB_MAX_BYTES
#endif
#define FAYS_ATRAK_DEPTH_IMG_MAX_BYTES (1280 * 720 * sizeof(float))

typedef unsigned char        uchar;
typedef unsigned short       ushort;
typedef unsigned int         uint;
typedef unsigned long        ulong;
typedef unsigned long long   ull;

/**
 * @brief 
 * 6DOF State. 
 * 4 bits: bit0 - idle(0) / running(1), bit1 - normal(0) / error(1), 
 *         bit2 & bit3 holds pose type:
 *             01 - Initializing (external pose only in fusion mode / no pose in vio-only mode)
 *             10 - Vio pose only (external pose has been missing for a while if in fusion mode)
 *             11 - Fusion pose
 */
enum ATRAK_STATE : uint {
    AS_STOP = 0,
    AS_ERROR = 3,
    AS_INITIALIZING = 5,
    AS_VIO_POSE_ONLY = 9,
    AS_FUSION_POSE = 13
};

enum ATRAK_ERROR_CODE : int {
    AEC_NO_ERROR = 0
};

enum ATRAK_TYPE : int {
    AT_VIO = 0,
    AT_GPOSE_FUSION = 1
};

enum ATRAK_IMG_ENCODING : short
{
    AIE_BGR8 = 0,
    AIE_RGB8,
    AIE_JPEG
};

enum ATRAK_DISTORTION_MODEL : uint8_t
{
    ADM_UNKOWN = 0,
    ADM_KB8,
    ADM_RADTAN
};

enum ATRAK_CAM_MODEL : uint8_t
{
    ACM_UNKOWN = 0,
    ACM_PINHOLE,
};

/**
 * @brief 6DOF pose
 * 
 */
typedef struct AtrakPose
{
    ull timestamp;     // unit: nanosencond
    double pos[3];     // unit: meter
    double quat[4];    // Hamilton convention quaternion: w, x, y, z
    short confidence;  // -1~100
} AtrakPose;

typedef struct AtrakImage
{
    int device_id;
    int seq;
    ull timestamp;    // unit: nanosencond
    int width;
    int height;
    int channel;
    short encoding;
    int step;
    uint bytes;       // num of data bytes
    uchar* data;      // BGR format if channel is equal to 3
} AtrakImage;

typedef struct AtrakIMU
{
    ull timestamp;    // unit: nanosencond
    double gyro[3];   
    double acc[3];    
} AtrakIMU;

typedef struct AtrakGNSS
{
    ull timestamp;
    int seq;
    uchar status;
    short service;
    double latitude;
    double longitude;
    double altitude;
    double position_cov[9];
    uchar position_cov_type;
} AtrakGNSS;

typedef struct AtrakExtraPose
{
    int frame;
    int seq;
    ull timestamp;
    uchar status;
    double pos[3];
    double quat[4];  // Hamilton convention quaternion: w, x, y, z
    double cov[36];
} AtrakExtraPose;

typedef struct AtrakMap
{
    ull timestamp;
    uint point_num;
    double* pos;  // x1, y1, z1, x2, y2, z2, ...
    uchar* gray;
} AtrakMap;


/****************************************** template tracking *******************************************/ 
typedef struct AtrakTempTrackConfig
{
    ull timestamp;                    // unit: nanosencond
    int seq;
    uint cls = 0;                     // For multi templates tracking (future)
    AtrakImage template_img;          // BGR 3-channel image support only
    float physical_width = -1.F;      // physical width of the template if available. uint: meter
    float physical_height = -1.F;     // physical height of the template if available. uint: meter
} AtrakTempTrackConfig;

typedef struct AtrakObject2D
{
    int cls;                          // class id. 0~2^24-1: (customized class), others: reserved
    int id;                           // instance id if available
    float confidence;                 // detection confidence 0.~1.
    float bbox[4];                    // bounding box in image pixel coordinates: center x, center y, width, height
    uint32_t extra_info_mask;         // extra info mask. 0: no extra info; 1: position available. Bit(L-->H): position, orientation, angle in image 
    float pose[7];                    // px, py, pz, qw, qx, qy, qz
    float angle;                      // rotation angle in image if available. uint: degree
} AtrakObject2D;
                                    
typedef struct AtrakObject2DDets
{
    ull timestamp;                    // unit: nanosencond
    int seq;
    uint num_objs;
    AtrakObject2D objs[FAYS_ATRAK_MAX_TRACK_INSTANCES];
} AtrakObject2DDets;


/****************************************** AprilTag tracking *******************************************/ 

enum ATRAK_TAG_FAMILY : uint {
    ATF_36H11 = 0,
    ATF_25H9,
    ATF_16H5,
    ATF_CIRCLE21H7,
    ATF_CIRCLE49H12,
    ATF_CUSTOM48H12,
    ATF_STANDARD41H12,
    ATF_STANDARD52H13
};

typedef struct AtrakAprilTagTrackConfig
{
    ull timestamp;                     // unit: nanosencond
    int seq;
    ATRAK_TAG_FAMILY tag_family;
    float tag_size = -1.F;             // only support multiple same-size tags for now. unit: meter
} AtrakAprilTagTrackConfig;

typedef struct AtrakAprilTag
{
    ATRAK_TAG_FAMILY tag_family;       // tag family
    uint id;                           // tag id
    float corners[8];                  // tag corners in image pixel coordinates [u, v, u, v, ...]. These always wrap counter-clock wise around the tag
    bool pose_available;               // tag pose is available or not
    float pos[3];                      // tag position if available. uint: meter
    float quat[4];                     // tag orientation if available. Hamilton convention quaternion: w, x, y, z
} AtrakAprilTag;

typedef struct AtrakAprilTagDets
{
    ull timestamp;                     // unit: nanosencond
    int seq;
    uint num_tags;
    AtrakAprilTag tags[FAYS_ATRAK_MAX_APRIL_INSTANCES];
} AtrakAprilTagDets;


/****************************************** Stereo depth *******************************************/ 

typedef struct AtrakDepthImage
{
    int device_id;
    int seq;
    ull timestamp;    // unit: nanosencond
    int width;
    int height;
    int channel;
    short encoding;
    int step;
    uint bytes;       // bytes of data array (sizeof(float) * length)
    float* data;
} AtrakDepthImage;

typedef struct AtrakRectifyInfo
{
    // camera pair id
    uint8_t cam_pair[2];
    // brings points in the unrectified first camera frame to points in the rectified first camera frame
    float R0[9];
    // brings points in the unrectified second camera frame to points in the rectified second camera frame
    float R1[9];
    // projects points in the rectified first camera coordinate system into the rectified first camera's image
    float P0[12];
    // projects points in the rectified first camera coordinate system into the rectified second camera's image
    float P1[12];
} AtrakRectifyInfo;


/****************************************** Calibration info *******************************************/ 

/**
 * @brief 6-DoF transformation (rotation/translation)
 */
typedef struct AtrakCamExtrinsics {
    float rot[9];                                       // row-major rotation matrix 
    float trans[3];                                     // translation vector
} AtrakCamExtrinsics;

/**
 * @brief Camera intrinsic parameters
 * 
 */
typedef struct AtrakCamIntrinsics {
    uint8_t cam_model;
    uint32_t width;
    uint32_t height;
    float fx;
    float fy;
    float cx;
    float cy;
    float extra[4];  // support more camera models
} AtrakCamIntrinsics;

/**
 * @brief Camera distortion parameters
 * @note support Radial-Tangential and Equidistant models
 */
typedef struct AtrakCamDistortions {
    uint8_t distortion_model;
    float dis[8];
} AtrakCamDistortions;

/**
 * @brief Camera parameters
 */
typedef struct AtrakCamParam {
    uint8_t cam_id;
    uint8_t available_mask;
    AtrakCamIntrinsics intrinsic;
    AtrakCamDistortions distortion;
    AtrakCamExtrinsics T_cn_cnm1;  // bring points in {camera n-1} frame to {camera n} frame
    AtrakCamExtrinsics T_cn_imu;   // bring points in {imu} frame to {camera n} frame
    double td;
} AtrakCamParam;

/**
 * @brief Camera chain parameters
 */
typedef struct AtrakCamChainParam {
    uint32_t num_of_cams;
    uint32_t downsize_ratio;
    AtrakCamParam cameras[3];
} AtrakCamChainParam;