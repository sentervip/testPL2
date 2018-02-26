
#ifndef _XXX_CONFIG_H_
#define _XXX_CONFIG_H_
#include <stdio.h>

//#define  FFP_MERGE     0
/** MACRO define */
#define  PLAYER_DEBUG              1
#define  DBG_CREAT_YUVFILES 0
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



#endif//_XX__CONFIG_H_

