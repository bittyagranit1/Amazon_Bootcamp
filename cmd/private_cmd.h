#ifndef PRIVATE_CMD_H
#define PRIVATE_CMD_H


#include "gas_cam_lib.h"

int GAS_API_GET_DLL_VERSION_CLI(int, char**);
int GAS_API_START_RECORD_CLI(int, char**);
int GAS_API_STOP_RECORD_CLI(int, char**);
int GAS_API_START_STREAMING_CLI(int, char**);
int GAS_API_STOP_STREAMING_CLI(int, char**);
int GAS_API_DO_SNAPSHOT_CLI(int, char**);
int GAS_API_GET_VIDEO_STATISTIC_CLI(int, char**);
int GAS_API_GET_STATUS_CLI(int, char**);


#endif // PRIVATE_CMD_H
