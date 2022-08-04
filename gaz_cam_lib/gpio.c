#include "gpio.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "gaz_cam_lib.h"


void reset_flash(flash_t* ft,int num_gpio,int rate,enum STATE state){
    while(rate)
        switch (state){
        case ON:
            off_led (num_gpio);
            state=OFF;
            break;
        case OFF:
            on_led (num_gpio);
            state=ON;
            break;
        case CONST_ON:
            on_led (num_gpio);
            state=CONST_ON;
            break;
        case CONST_OFF:
            off_led (num_gpio);
            state=CONST_OFF;
            break;
        }
    msleep (rate);
}
void on_led(int num_gpio){
    // Export the desired pin by writing to /sys/class/gpio/export

    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/export");
        exit(1);
    }

    if (write(fd, &num_gpio, 2) != 2) {
        perror("Error writing to /sys/class/gpio/export");
        exit(1);
    }

    close(fd);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction

    fd = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio24/direction");
        exit(1);
    }

    if (write(fd, "out", 3) != 3) {
        perror("Error writing to /sys/class/gpio/gpio24/direction");
        exit(1);
    }

    close(fd);

    fd = open("/sys/class/gpio/gpio24/value", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/gpio24/value");
        exit(1);
    }

    // Toggle LED 50 ms on, 50ms off, 100 times (10 seconds)

    for (int i = 0; i < 100; i++) {
        if (write(fd, "1", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(1);
        }
        usleep(50000);

        if (write(fd, "0", 1) != 1) {
            perror("Error writing to /sys/class/gpio/gpio24/value");
            exit(1);
        }
        usleep(50000);
    }

    close(fd);

    // Unexport the pin by writing to /sys/class/gpio/unexport

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/unexport");
        exit(1);
    }

    if (write(fd, &num_gpio, 2) != 2) {
        perror("Error writing to /sys/class/gpio/unexport");
        exit(1);
    }

    close(fd);

    // And exit
}
void off_led(int num_gpio){

}
void * flash(void *){

}
void main_gpio(int rate,enum STATE state,enum ACTIVE active){
    int num_gpio=0;
    while(active){
    if (get_status&RECORD_ACTIVE){
        rate =200;
        state=ON;
        num_gpio=24;//red
    }
    if (get_status & STOP_RECORD_ACTIVE){
         rate=500;
         state=OFF;
         num_gpio=24;//red
    }
    if (get_status & DO_SNAPSHOT_ACTIVE){
         rate=0;
         state =CONST_ON;
         num_gpio=18;//green
    }
    reset_flash(ft,num_gpio,rate,state); //one
  }
}
