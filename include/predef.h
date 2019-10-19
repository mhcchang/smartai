//
//		Copyright :					@2018, ***, All Rights Reserved
//
//		Author :						王科威
//		E-mail :						wangkw531@icloud.com
//		Date :							2017-11-29
//		Description:
//
//		History:						Author										Date													Description
//											王科威										2017-11-29									创建
//

#ifndef PREDEF_H
#define PREDEF_H

#define NS_BEGIN(name, version) namespace NS_##name##_##version{
#define NS_END }
#define NS(name, version) NS_##name##_##version

#ifdef _WINDOWS
#ifndef CALLBACK
#define CALLBACK __stdcall
#endif//CALLBACK
#ifndef NULL
#define NULL 0
#endif//NULL
#else
#define CALLBACK
#endif//_WINDOWS

static const unsigned short minPortNumber = 5000;
static const unsigned short maxPortNumber = 65535;
static const unsigned int maxTransportMTUBytes = 1024 * 1024/*65530*/;

typedef struct tagBGR24Frame_t
{
	char* frameData;
	int frameBytes;
	char* jpegData;
	int jpegBytes;
	int channelIndex;
	char* NVRIp;
}BGR24Frame;

#endif//PREDEF_H
