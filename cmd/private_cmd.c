#include "private_cmd.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"gas_cam_lib.h"
#include"command_handler.h"

extern gas_api* p_gaz;
int GAS_API_GET_DLL_VERSION_CLI(int numParams, char** params)
{
    return 1;
}

int GAS_API_START_RECORD_CLI(int numParams, char** params)
{
    return 1;
}

int GAS_API_STOP_RECORD_CLI(int numParams, char** params)
{
    return 1;
}

int GAS_API_START_STREAMING_CLI(int numParams, char** params)
{
    return 1;
}

int GAS_API_STOP_STREAMING_CLI(int numParams, char** params)
{
    return 1;

}

int GAS_API_DO_SNAPSHOT_CLI(int numParams, char** params)
{
    return 1;
}

int GAS_API_GET_VIDEO_STATISTIC_CLI(int numParams, char** params)
{
    return 1;
}

int GAS_API_GET_STATUS_CLI(int numParams, char** params)
{
    return 1;
}
