
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dlfcn.h>
#include "try_dll.h"
#include <signal.h>
#include "gas_cam_lib.h"
#include "command_handler.h"
#define SIGINT 2
//#include "queue.h"
using namespace std;
gas_api *p_gas;
p_handler  handler;

gas_api* load_gas_lib(){
    void* libHandle=dlopen("/home/bitya/amazon-project/gascam/build-gaz_cam_lib-Desktop-Debug/libgaz_cam_lib.so.1.0.0",RTLD_LAZY);
    assert(libHandle);
    p_gas=(gas_api*)dlsym(libHandle,"gas_api_lib");
    assert(p_gas);
    return p_gas;
}

void handle_sigint(int sig)
{
    if(sig==SIGINT){
        printf("Caught signal %d\n", sig);
        p_gas->stop_record(handler);
    }
}


int main()
{
    signal(SIGINT, handle_sigint);
    fprintf(stderr,"hello\n");
    p_gas=load_gas_lib();
    handler=p_gas->init_dll();
    record_t r;
    snapshot_t t;
    p_gas->start_record(handler,r);
    //p_gas->do_snapshot(handler,t);

    return 1;
}
