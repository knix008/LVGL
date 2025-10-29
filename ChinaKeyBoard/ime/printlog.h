#include <common.h>
#ifndef PRINT_LOG_H_
#define PRINT_LOG_H_

#define ERROR_LOG_OK			0
#define ERROR_LOG_SET_TYPE		-1

enum __PRINT_LOG_TYPE
{
	LOG_TYPE_NULL		= 0x0,			/* 不打印调试信息 */
	LOG_TYPE_ERROR		= (1 << 0),		/* 打印错误调试信息 */
	LOG_TYPE_WARNING	= (1 << 1),		/* 打印警告调试信息 */
	LOG_TYPE_INFO		= (1 << 2),		/* 打印提示调试信息 */
	LOG_TYPE_DEBUG		= (1 << 3),		/* 打印调试信息 */
};
	
enum __ANSI_COLOR_TYPE
{
	ANSI_BLACK			=	0,
	ANSI_RED			=	1,
	ANSI_GREEN			=	2,
	ANSI_YELLOW			=	3,
	ANSI_BLUE			=	4,
	ANSI_AMARANTH		=	5,	//紫紅色
	ANSI_ULTRAMARINE	=	6,  //青藍色
	ANSI_WRITE			=	7,	
};

enum __ANSI_DISPLY_TYPE
{
	ADT_DEFAULT			=	0,	// 终端默认设置
	ADT_HIGHT_LIGHT		=	1,	//高亮显示
	ADT_UNDER_LINE		=	4,	//使用下划线
	ADT_SHINE			=	5,	//闪烁
	ADT_INVERT			=	7,	//反白
	ADT_INVISIBLE		=	8,	//不可见
};


int SetImeLogType(char logType, BOOL enable);

int SetImeDynamicName(const char *pDynamicName);

int GetImeLogType(void);


void PrintImeColorLog(enum __PRINT_LOG_TYPE logType, const char *pFile, const char *pFunction,
		int line, const char *pFormat, ...);

#define _PrintLog_(type, fmt...) \
		PrintImeColorLog(type, __FILE__, __FUNCTION__, __LINE__, fmt)

/* 打印不同类型信息 */

#define PrintImeErr(fmt...)		_PrintLog_(LOG_TYPE_ERROR, fmt)
#define PrintImeWarn(fmt...)		_PrintLog_(LOG_TYPE_WARNING, fmt)
#define PrintImeInfo(fmt...)		_PrintLog_(LOG_TYPE_INFO, fmt)
#define PrintImeDebug(fmt...)		_PrintLog_(LOG_TYPE_DEBUG, fmt)

#define printfmy(fmt...)  __printf_my(__FILE__, __FUNCTION__, __LINE__, fmt)

#endif /* DEBUGLOG_H_ */
