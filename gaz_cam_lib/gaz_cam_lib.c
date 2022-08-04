#include "gaz_cam_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"
#include <time.h>
#include <errno.h>
#ifdef MY_DEBUG
#define PRINTF_DBG printf
#else
#define PRINTF_DBG(m...)
#endif

int get_status(enum ACTIVE active){
   return  1;
}

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int GAS_API_GET_DLL_VERSION(p_handler handler)
{
    return 1;
}

int GAS_API_START_RECORD(p_handler handler, record_t record)
{
    handler->status=1;
    handler->status&RECORD_ACTIVE;
    handler->record_status=1;
    pthread_create(&handler->stg_capture->thread_id, NULL, capture, handler);
    pthread_create(&handler->stg_rgb_convertor->thread_id, NULL, rgb_convertor, handler);
    pthread_create(&handler->stg_yuv_convertor->thread_id, NULL, convert_yuv, handler);
    //pthread_create(&handler->stg_decoder->thread_id,NULL,decoder,handler);
    //pthread_create(&handler->stg_write->thread_id,NULL,stg_write,handler);
    pthread_join(handler->stg_capture->thread_id,NULL);
    pthread_join(handler->stg_rgb_convertor->thread_id,NULL);
    pthread_join(handler->stg_yuv_convertor->thread_id,NULL);
    //pthread_join(handler->stg_decoder->thread_id,NULL);
    // pthread_join(handler->stg_write->thread_id,NULL);
    return 1;
}

int GAS_API_STOP_RECORD(p_handler handler)
{
    handler->status&STOP_RECORD_ACTIVE;
    printf("GAS_API_STOP_RECORD\n");
    handler->record_status=0;
    while(handler->record_status!=2);
    exit(0);
    return 1;
}
int GAS_API_START_STREAMING(p_handler handler, streaming_t streaming)
{
    //TODO:
    p_handler hand = (p_handler)handler;
    int i = hand->RGB_static_mat[0][0];
    return i;
}
int GAS_API_STOP_STREAMING(p_handler handler)
{
    p_handler hand = (p_handler)handler;
    int i = hand->RGB_static_mat[0][0];
    return i;
}
int GAS_API_GET_VIDEO_STATISTIC(p_handler handler)
{
    //TODO:
    p_handler hand = (p_handler)handler;
    int i = hand->RGB_static_mat[0][0];
    return i;
}
int GAS_API_GET_STATUS(p_handler handler)
{
    //TODO:
    p_handler hand = (p_handler)handler;
    int i = hand->RGB_static_mat[0][0];
    return i;
}
handler_t* INIT_DLL() {
    handler_t* handler = (handler_t*)calloc(1, sizeof(handler_t));
    handler->record_status=0;
    handler->stream_status=0;
    handler->status=1;
    //init threads and queues to join between the threads
    handler->stg_capture = (p_stage)calloc(1, sizeof(stage_t));
    handler->stg_capture->src_queue = NULL;
    handler->stg_capture->dest_queue = createQueue(10);

    handler->stg_rgb_convertor = (p_stage)calloc(1, sizeof(stage_t));
    handler->stg_rgb_convertor->src_queue = handler->stg_capture->dest_queue;
    handler->stg_rgb_convertor->dest_queue = createQueue(10);

    handler->stg_yuv_convertor = (p_stage)calloc(1, sizeof(stage_t));
    handler->stg_yuv_convertor->src_queue = handler->stg_rgb_convertor->dest_queue;
    handler->stg_yuv_convertor->dest_queue = createQueue(10);

    handler->stg_decoder = (p_stage)calloc(1, sizeof(stage_t));
    handler->stg_decoder->src_queue = handler->stg_yuv_convertor->dest_queue;
    handler->stg_decoder->dest_queue = createQueue(10);

    handler->stg_write = (p_stage)calloc(1, sizeof(stage_t));
    handler->stg_write->src_queue = handler->stg_decoder->dest_queue;
    handler->stg_write->dest_queue = NULL;

    initRGB_static_mat(handler);
    return handler;
}
void initRGB_static_mat(p_handler handler)
{
    //init the matrix by cold degrees
    int i = 0;
    char x = 0, y = 0, z = (char)255;
    for (i = 0; i < 80; i++) {
        handler->RGB_static_mat[0][i] = x;
        handler->RGB_static_mat[1][i] = y;
        handler->RGB_static_mat[2][i] = z;
        z -= 3;
    }
}
int GAS_API_DO_SNAPSHOT(p_handler handler, snapshot_t snapshot)
{
    handler->status=1;
    handler->status&DO_SNAPSHOT_ACTIVE;
    char* mat = snapshot_capture(handler, snapshot);
    char* RGB_mat = snapshot_RGB(handler, mat);
    //ppm_image struct to write the snapshot to the file
    ppm_image ppm = { ppm.height = LENGTH,ppm.width = WIDTH,ppm.data = RGB_mat,ppm.size = LENGTH * WIDTH * 3 };
    FILE* snapshot_file = fopen(SNAPSHOTFILEPATH, "wb+");
    if (!snapshot_file)exit(1);
    ppm_save(&ppm, snapshot_file);
    fclose(snapshot_file);
    return 1;
}
char* snapshot_capture(handler_t* handler, snapshot_t snapshot) {
    int i, j;
    char* mat = (char*)calloc(WIDTH * LENGTH, sizeof(char));
    if (!mat)
        exit(1);
    //check if there is a record playing now - take a ready matrix from the queue
    if (handler->stg_capture->is_active) {
        mat = (char*)(top(handler->stg_capture->dest_queue)->data);
        return mat;
    }
    //if the record is not playing now - fill the matrix
    for (i = 0; i < LENGTH; i++) {
        for (j = 0; j < WIDTH; j++) {
            *(mat + (i * WIDTH) + j) = rand() % 79;
        }
    }
    return mat;
}
char* snapshot_RGB(handler_t* handler, char* base_mat) {
    int i, j, heat_degrees = 255;
    //declare a mat for the RGB format
    char* mat = (char*)calloc(WIDTH * LENGTH * 3, sizeof(char));
    if (!mat)exit(1);
    //fill mat by RGB_Static_mat fields
    for (i = 0; i < LENGTH; i++) {
        for (j = 0; j < WIDTH; j++) {
            *(mat + (i * WIDTH * 3) + j * 3) = handler->RGB_static_mat[0][(int)*(base_mat + i * WIDTH + j)];
            *(mat + (i * WIDTH * 3) + j * 3 + 1) = handler->RGB_static_mat[1][(int)*(base_mat + i * WIDTH + j)];
            *(mat + (i * WIDTH * 3) + j * 3 + 2) = handler->RGB_static_mat[2][(int)*(base_mat + i * WIDTH + j)];
        }
    }
    //put in the matrix static hat degrees
    for (i = 150; i < 170; i++) {
        for (j = 150, heat_degrees = 255; j < 170; j++, heat_degrees -= 2) {
            *(mat + (i * WIDTH * 3) + j * 3) = (char)heat_degrees;
        }
    }
    free(base_mat);
    return mat;
}
size_t ppm_save(ppm_image* img, FILE* outfile) {
    //write the snapshot to the file
    size_t n = 0;
    n += fprintf(outfile, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n",
                 img->width, img->height, 0xFF);
    n += fwrite(img->data, 1, img->width * img->height * 3, outfile);
    return n;
}
void* capture(void* handler) {
    p_handler hand = (p_handler)handler;
    int i, j;
    char* mat;
    hand->stg_capture->is_active = 1;
    while (hand->record_status==1) {
        mat = (char*)calloc(WIDTH * LENGTH, sizeof(char));
        if (!mat)
            exit(1);
        for (i = 0; i < LENGTH; i++) {
            for (j = 0; j < WIDTH; j++) {
                *(mat + (i * WIDTH) + j) = rand() % 79;
                printf("capture loop\n");
            }
        }
        //try to enqueue the matrix to the queue till there is an empty place in the queue
        while (enqueue(hand->stg_capture->dest_queue, createNode(&mat)));
        msleep(250);
    }
    hand->stg_capture->is_active = 0;
    return NULL;
}
void* rgb_convertor(void* handler) {
    handler_t hand = *(p_handler)handler;
    hand.stg_rgb_convertor->is_active = 1;
    char* mat;
    //declare base mat
    char* base_mat = (char*)calloc(WIDTH * LENGTH, sizeof(char));
    if (!base_mat)
        exit(1);
    while (hand.record_status==1 || !isEmpty(hand.stg_rgb_convertor->src_queue)) {
        while (isEmpty(hand.stg_rgb_convertor->src_queue));
        base_mat = *((char**)(dequeue(hand.stg_rgb_convertor->src_queue)->data));
        mat = (char*)calloc(WIDTH * LENGTH * 3, sizeof(char));

        //fill mat fieleds by RGB_Static_mat fields
        for (int i = 0; i < LENGTH; i++) {
            for (int j = 0; j < WIDTH; j++) {
                *(mat + (i * WIDTH * 3) + j * 3) = (&hand)->RGB_static_mat[0][(int)*(base_mat + i * WIDTH + j)];
                *(mat + (i * WIDTH * 3) + j * 3 + 1) = (&hand)->RGB_static_mat[1][(int)*(base_mat + i * WIDTH + j)];
                *(mat + (i * WIDTH * 3) + j * 3 + 2) = (&hand)->RGB_static_mat[2][(int)*(base_mat + i * WIDTH + j)];
                printf("RGB loop\n");
            }
        }        //try to enqueue the matrix to the queue till there is an empty place in the queue
        while (enqueue(hand.stg_rgb_convertor->dest_queue, createNode(&mat)));
    }
    hand.stg_rgb_convertor->is_active = 0;
    free(base_mat);
    return NULL;
}
void* convert_yuv(void* handler) {
    handler_t hand = *(p_handler)handler;
    unsigned char R, G, B;
    hand.stg_yuv_convertor->is_active = 1;
    char* base_mat = (char*)calloc(LENGTH * WIDTH * 3, sizeof(char));
    YUV* yuv;
    while (hand.record_status==1 || !isEmpty(hand.stg_yuv_convertor->src_queue))    {
        while (isEmpty(hand.stg_yuv_convertor->src_queue));

        base_mat = *((char**)(dequeue(hand.stg_yuv_convertor->src_queue)->data));
        yuv = (YUV*)calloc(1, sizeof(YUV));
        if (!yuv)exit(1);
        //convert from RGB format to YUV format
        for (int i = 0; i < LENGTH; i++) {
            for (int j = 0; j < WIDTH; j++) {
                R = *(base_mat + i * WIDTH * 3 + j * 3);
                G = *(base_mat + i * WIDTH * 3 + j * 3 + 1);
                B = *(base_mat + i * WIDTH * 3 + j * 3 + 2);
                yuv->y[WIDTH * i + j] = (0.257 * R) + (0.504 * G) + (0.098 * B) + 16 + '0';
                if (i % 2 == 0 && j % 2 == 0)  {
                    yuv->u[(WIDTH * i + j) / 4] = (0.439 * R) - (0.368 * G) - (0.071 * B) + 128 + '0';
                    yuv->v[(WIDTH * i + j) / 4] = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128 + '0';
                }
                printf("yuv loop\n");
            }
        }
        //try to enqueue the matrix to the queue till there is an empty place in the queue
        while(enqueue(hand.stg_yuv_convertor->dest_queue, createNode(yuv)));
        free(base_mat);
    }
    hand.record_status=2;
    hand.stg_yuv_convertor->is_active = 0;
    return NULL;
}
void* decoder(void* handler) {
    p_handler hand = (p_handler)handler;
    hand->stg_decoder->is_active = 0;
    printf("decoder");
    return NULL;
}

void* stg_write(void* handler) {
    p_handler hand = (p_handler)handler;
    hand->stg_write->is_active = 0;
    printf("write");
    return NULL;
}


gas_api gas_api_lib = {
    .get_dll_version = GAS_API_GET_DLL_VERSION,
    .start_record = GAS_API_START_RECORD,
    .stop_record = GAS_API_STOP_RECORD,
    .start_streaming = GAS_API_START_STREAMING,
    .stop_streaming = GAS_API_STOP_STREAMING,
    .do_snapshot = GAS_API_DO_SNAPSHOT,
    .get_video_statistic = GAS_API_GET_VIDEO_STATISTIC,
    .get_status = GAS_API_GET_STATUS,
    .init_dll = INIT_DLL
};
