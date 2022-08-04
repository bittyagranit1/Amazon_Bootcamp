#ifndef TRY_DLL_H
#define TRY_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
int (*start_record)(char * file_name);
int (*stop_record)();
int (*get_version)();
int (*start_streaming)();
int (*stop_streaming)();
int (*do_snapshot)();
int (*get_video_statistic)();
int (*get_status)();
}gasapi_t;


#ifdef __cplusplus
}
#endif

#endif // TRY_DLL_H
