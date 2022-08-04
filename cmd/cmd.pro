QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -L/usr/lib/x86_64-linux-gnu$ -ljpeg

SOURCES += \
        command_handler.c \
        hash_table.c \
        main.cpp \
        private_cmd.c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../amazon-project/try-queue/queue.h \
    command_handler.h \
    gas_cam_lib.h \
    hash_table.h \
    private_cmd.h \
    queue.h \
    try_dll.h
