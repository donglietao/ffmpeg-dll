#TEMPLATE = app
#CONFIG += console
TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
            fftools/ffmpeg_mux_init.c  \
            fftools/ffmpeg_dec.c  \
            fftools/ffmpeg_demux.c  \
            fftools/ffmpeg_filter.c  \
            fftools/ffmpeg_enc.c  \
            fftools/ffmpeg_mux.c  \
            fftools/ffmpeg_mux.h  \
            fftools/fopen_utf8.h  \
            fftools/thread_queue.c  \
            fftools/thread_queue.h  \
            fftools/objpool.h      \
            fftools/objpool.c      \
            fftools/opt_common.c      \
            fftools/opt_common.h      \
            fftools/sync_queue.h      \
            fftools/sync_queue.c      \
            fftools/ffmpeg_utils.h      \
            fftools/ffmpeg_sched.h      \
            fftools/ffmpeg_sched.c      \
            fftools/cmdutils.h      \
            fftools/cmdutils.c      \
            fftools/ffmpeg_hw.c    \
            fftools/ffmpeg_opt.c    \
            fftools/ffmpeg.h        \
            fftools/ffmpeg.c        \

SOURCES += \
            libavutil/attributes.h      \
            libavutil/wchar_filename.h

SOURCES += \
            main.c  \
            config.h


INCLUDEPATH += D:/cpp-sdk/FFmpeg-n7.0/ffmpeg-7.0-full_build-shared/include  \

LIBS    +=  -LD:/cpp-sdk/FFmpeg-n7.0/ffmpeg-7.0-full_build-shared/lib \
            -lavcodec       \
            -lavdevice      \
            -lavfilter      \
            -lavformat      \
            -lavutil        \
            -lpostproc      \
            -lswresample    \
            -lswscale
