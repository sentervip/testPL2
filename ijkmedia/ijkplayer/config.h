/*
 * Copyright (c) 2015 Zhang Rui <bbcallen@gmail.com>
 *
 * This file is part of ijkPlayer.
 *
 * ijkPlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ijkPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ijkPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef PLAY__CONFIG_H
#define PLAY__CONFIG_H

#include "libffmpeg/config.h"
#include <stdio.h>

//#define  FFP_MERGE     0
/** MACRO define */
#define  PLAYER_DEBUG              1
#define  DBG_CREAT_YUVFILES        0
#define PLAYER_VERSION   "V010SP01B180224R0001"

/** MACRO arch */
#define   FMT_PLATFORM_IOS     1
#define   FMT_PLATFORM_ANDROID  2
#define FMT_PLATFORM_NOW    FMT_PLATFORM_IOS  // Please set it befor compiled !!!


#if (FMT_PLATFORM_NOW == FMT_PLATFORM_ANDROID)
#include <android/log.h>
#elif (FMT_PLATFORM_NOW == FMT_PLATFORM_IOS)
#include <stdio.h>
//#include <iostream>
#else
#error please set platform befor compile！
#endif

/** MACRO debug */
#if (FMT_PLATFORM_NOW == FMT_PLATFORM_ANDROID)
#define LOG(...)     __android_log_print(ANDROID_LOG_DEBUG,"JNI",__VA_ARGS__)
#define LOGE(...)    __android_log_print(ANDROID_LOG_ERROR,"JNI",__VA_ARGS__)
#else
#define LOG(...)    printf(__VA_ARGS__)
#define LOGE(...)   printf(__VA_ARGS__)
#endif

#if PLAYER_DEBUG
#define LOG_LINE()           LOG("L=%d\n",__LINE__)
#define LOGD                 LOG
#else
#define LOG_LINE()
#define LOGD
#endif

#define  REC_STATUS_START      0xa437aa
#define  REC_STATUS_STOP       0xa437ff
#define  REC_STATUS_INVALID    0xa437ee
/** typedef  */
#ifndef TAGRECORDSW
typedef struct  strRecordSwitch{
    int  recSw;
    char fileOut[128];
}tagRecSwitch,*ptagRecSwitch;
#endif

#ifndef InputStreamType
typedef enum OpenStreamType{
    FM_STREAM_RTSP = 1,
    FM_STREAM_FILES,
    FM_STREAM_RTMP,
    FM_STREAM_HTTP
}OpenStreamType,*pOpenStreamType;
#endif


/** var */
extern OpenStreamType g_open_stream_types;
extern tagRecSwitch tagRecSw,*ptagRecSw;


/** function */
int stream2jpg(const char *in_streamName,const char *out_fileName);





// FIXME: merge filter related code and enable it
#ifdef CONFIG_AVFILTER
#undef CONFIG_AVFILTER
#endif
#define CONFIG_AVFILTER 0

#ifdef FFP_MERGE
#undef FFP_MERGE
#endif

#ifdef FFP_SUB
#undef FFP_SUB
#endif

#ifndef FFMPEG_LOG_TAG
#define FFMPEG_LOG_TAG "FFMPEG"
#endif

#endif//FFPLAY__CONFIG_H
