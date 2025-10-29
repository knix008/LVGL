/*
 *
 * debuglog.c
 *
 * Copyright (C) 2011-2012, ZKSoftware Inc.
 *
 * Created: 2011-11-30
 * Author: xsen
 *
 * Description:  打印信息接口
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdarg.h>

#include "printlog.h"

static char gLogType = 0/*LOG_TYPE_ERROR | LOG_TYPE_WARNING | LOG_TYPE_INFO | LOG_TYPE_DEBUG*/;
static char gDynamicName[24] = {0};

int SetImeLogType(char logType, BOOL enable)
{
	if (TRUE == enable)
	{
		gLogType = gLogType | logType;
	}
	else if (FALSE == enable)
	{
		gLogType = gLogType & (~logType);
	}
	else
	{
		return ERROR_LOG_SET_TYPE;
	}

	return ERROR_LOG_OK;
}

void SetImeLogTypesByParams(const char *pParams)
{
	if (NULL != strchr(pParams,'E'))
	{
		SetImeLogType(LOG_TYPE_ERROR, TRUE);
	}
	if (NULL != strchr(pParams,'W'))
	{
		SetImeLogType(LOG_TYPE_WARNING, TRUE);
	}
	if (NULL != strchr(pParams,'I'))
	{
		SetImeLogType(LOG_TYPE_INFO, TRUE);
	}
	if (NULL != strchr(pParams,'D'))
	{
		SetImeLogType(LOG_TYPE_DEBUG, TRUE);
	}
}

int SetImeDynamicName(const char *pDynamicName)
{
	int ret = 0;

	snprintf(gDynamicName, sizeof(gDynamicName), "%s", pDynamicName);

	return ret;
}

static int GetCurPrintDateTime(char *pOutBuf, int maxOutBufLen)
{
	time_t curTime = 0;
	char tmpYear[5];
	struct tm *pCurLocalDateTime = NULL;

	if (NULL == pOutBuf)
	{
		return -1;
	}

	memset(tmpYear, 0, sizeof(tmpYear));
	memset(pOutBuf, 0, maxOutBufLen);

	curTime = time(0);
	pCurLocalDateTime = localtime(&curTime);

	snprintf(tmpYear, sizeof(tmpYear), "%04d", pCurLocalDateTime->tm_year + 1900);

	snprintf(pOutBuf, maxOutBufLen, "%s-%02d-%02dT%02d:%02d:%02d",
			tmpYear, pCurLocalDateTime->tm_mon + 1, pCurLocalDateTime->tm_mday,
			pCurLocalDateTime->tm_hour, pCurLocalDateTime->tm_min, pCurLocalDateTime->tm_sec);

	return 0;
}

int GetImeLogType(void)
{
	return gLogType;
}

static const char * ColorOneString(const char *pText, enum __ANSI_DISPLY_TYPE att, enum __ANSI_COLOR_TYPE fontColor, enum __ANSI_COLOR_TYPE background)
{
	static char tmpStr[1024] = {0};
	snprintf(tmpStr, sizeof(tmpStr),"\033[%d;3%d;4%dm%s\033[0m", att, fontColor, background, pText);
	return tmpStr;
	
}

void PrintImeColorLog(enum __PRINT_LOG_TYPE logType, const char *pFile, const char *pFunction,
		int line, const char *pFormat, ...)
{
	va_list args;
	const char *pPreFix = NULL;

	if (0 == (logType & gLogType))
		return;

	switch (logType)
	{
		case LOG_TYPE_INFO:
		{
			//pStream = stderr;
			pPreFix = ColorOneString("Info", ADT_DEFAULT, ANSI_WRITE, ANSI_BLACK);
			break;
		}
		case LOG_TYPE_WARNING:
		{
			//pStream = stderr;
			pPreFix = ColorOneString("Warning", ADT_DEFAULT, ANSI_BLACK, ANSI_YELLOW);
			break;
		}
		case LOG_TYPE_ERROR:
		{
			//pStream = stderr;
			pPreFix = ColorOneString("Error", ADT_DEFAULT, ANSI_BLACK, ANSI_RED);
			break;
		}
		case LOG_TYPE_DEBUG:
		{
			//pStream = stderr;
			pPreFix = ColorOneString("Debug", ADT_DEFAULT, ANSI_GREEN, ANSI_BLACK);
			break;
		}
		default:
		{
			//pStream = stderr;
			pPreFix = ColorOneString("Unknown", ADT_DEFAULT, ANSI_WRITE, ANSI_BLACK);
			break;
		}
	}

	char curTime[1024];
	GetCurPrintDateTime(curTime, sizeof(curTime));

	printf("[%s:%s] %s [%s:%s()-%d]: ",
				gDynamicName, curTime, pPreFix, pFile, pFunction, line);;

	va_start(args, pFormat);
#ifdef SET_STREAM_MODE
	vfprintf(pStream, pFormat, args);
#else
	vprintf(pFormat, args);
#endif
	va_end(args);

#ifdef SET_STREAM_MODE
	fprintf(pStream, "\n");
#else
	printf("\n");
#endif

	return;
}

void __printf_my(const char *pFile, const char *pFunction, int line, const char *pFormat, ...)
{

	struct timeval tv;
	struct timezone tz;
	struct tm *pCurLocalDateTime = NULL;
	va_list args;

	gettimeofday(&tv, &tz);
	pCurLocalDateTime = localtime(&tv.tv_sec);

	printf("%04d-%02d-%02dT%02d:%02d:%02d.%03ld  %s %s [%d] ",pCurLocalDateTime->tm_year + 1900, \
			pCurLocalDateTime->tm_mon + 1, pCurLocalDateTime->tm_mday,pCurLocalDateTime->tm_hour, \
			pCurLocalDateTime->tm_min, pCurLocalDateTime->tm_sec, tv.tv_usec/1000, pFile, pFunction, line);

	va_start(args, pFormat);
	vprintf(pFormat, args);
	va_end(args);
	printf("\n");
	fflush(stdout);
}
