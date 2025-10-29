/**
 * FileName: Properties.c
 *
 * Copyright (C) 2014-2015, ZKSoftware Inc.
 *
 * Created: 2014-10-30
 * Author: wilsn.
 *
 * Description:
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minigui/common.h> 
#include "properties.h"
 
#define KEY_SIZE        128 // key缓冲区大小
#define VALUE_SIZE      128 // value缓冲区大小
 
#define LINE_BUF_SIZE   256 // 读取配置文件中每一行的缓冲区大小
#define BUCKETS 		150

#define HASH_GENERAL_KEY     1
#define HASH_RCT_KEY     2

typedef struct hash_key_s hash_key_t;
struct hash_key_s
{
	unsigned char *key;
	void *value;
	unsigned int flags;
	hash_key_t *next;
};

typedef struct hash_counter_entry_s hash_counter_entry_t;
struct hash_counter_entry_s
{
	unsigned int backetLen;
	hash_key_t **keys;
}; 

/* RS Hash Function */
static unsigned int hashFunc(char *str)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	char *pstr = str ;
	while (*pstr)
	{
		hash = hash * a + (*pstr++);
		a *= b;
	}

	return (hash & 0x7FFFFFFF);
}  


// 去空格
static int trimeSpace(const char *src, const int srcLen, char *dest)
{
	if (src == NULL || dest == NULL || srcLen <= 0)
	{
		printf("trimeSpace error:-1 from (src == NULL || dest == NULL)\n");
		return -1;
	}

	const char *v_start, *v_end;
	char *value = NULL;
	int line_len;
	int val_len;
	line_len = srcLen;
	v_start = src;
	v_end = &src[line_len - 1];

	while (((*v_start == ' ') || (*v_start == '\t')) && (v_start <= v_end))
	{
		v_start++;
	}

	if (*v_start == '#' || v_start > v_end)
	{
		return (-1);
	}

	while (((*v_end == ' ') || (*v_end == '\t') || (*v_end == '\n') || (*v_end == '\r')) && (v_end > v_start))
	{
		v_end--;
	}

	val_len = v_end - v_start + 1;
	memcpy(dest, v_start, val_len);
	*(dest + val_len) = '\0';

	return val_len;
}

static BOOL Is_Digit(const char *str)
{
	int len;
	if (NULL == str)
	{
		return FALSE;
	}

	len = strlen(str);
	while (len > 0)
	{
		if (*str < '0' || *str > '9')
		{
			return FALSE;
		}
		str++;
		len--;
	}
	return TRUE;
}

static void *HashFind(const void* handle, unsigned char *key, unsigned int *flag)
{
	unsigned int hash_value;
	hash_key_t *p;
	hash_counter_entry_t *ph = (hash_counter_entry_t *) handle;
	if (handle == NULL  || NULL == key || NULL == flag)
	{
		return NULL;
	}
	hash_value = hashFunc(key) % ph->backetLen;
	p = ph->keys[hash_value];

	while (p)
	{
		if ((NULL != p->key) && (0 == strcmp(key, p->key)))
		{
			*flag = p->flags;
			return p->value;
		}
		p = p->next;
	}

	return NULL;
}

static BOOL HashInsertKey(const void* handle, unsigned char *key, void *value, unsigned int flag)
{
	unsigned int hash_value;
	hash_key_t *p = NULL ,*pHash = NULL;
	hash_counter_entry_t * ph = (hash_counter_entry_t *) handle;
	if (handle == NULL || NULL == key || NULL == value)
	{
		return FALSE;
	}

	hash_value = hashFunc(key) % ph->backetLen;
	pHash = p = ph->keys[hash_value];
//	printf("key=%s,hash=%d\n",key,hash_value);
	while (p)
	{
		if ((NULL != p->key) && (0 == strcmp(key, p->key)))
		{
			break;
		}
		pHash = p;
		p = p->next;
//		printf("%d\n",hash_value);
	}
	if (p == NULL)
	{
		void *pvalue = NULL;
		hash_key_t * temP = NULL;
		if(HASH_RCT_KEY == flag)
		{
			pvalue = (void*) malloc(sizeof(RECT));
			if (pvalue == NULL)
			{
				printf("malloc in FILE[%s],FUNCTION[%s],LINE[%d]", __FILE__, __FUNCTION__, __LINE__);
				perror("error:");
				return FALSE;
			}
			memcpy(pvalue,value,sizeof(RECT));
		}
		else if(HASH_GENERAL_KEY == flag)
		{
			pvalue = (void*) malloc(sizeof(int));
			if (pvalue == NULL)
			{
				printf("malloc in FILE[%s],FUNCTION[%s],LINE[%d]", __FILE__, __FUNCTION__, __LINE__);
				perror("error:");
				return FALSE;
			}
			memcpy(pvalue,value,sizeof(int));
		}
		else
		{
			return FALSE;
		}

		temP = (hash_key_t *) malloc(sizeof(hash_key_t));
		if (temP == NULL)
		{
			printf("malloc in FILE[%s],FUNCTION[%s],LINE[%d]", __FILE__, __FUNCTION__, __LINE__);
			perror("error:");
			free(pvalue);
			return FALSE;
		}
		temP->key = strdup(key);
		temP->value = pvalue;
		temP->next = NULL;
		temP->flags = flag;
		if(NULL != pHash)
		{
//			printf("last=%s,next=%s\n",pHash->key,temP->key);
			pHash->next = temP;
		}
		else
		{
			ph->keys[hash_value] = temP;
		}
	}
	else
	{
		void *pvalue = NULL;
		if (NULL != pHash->value )
		{
			free(pHash->value);
			pHash->value = NULL;
		}

		if (HASH_RCT_KEY == flag)
		{
			pvalue = (void*) malloc(sizeof(RECT));
			if (pvalue == NULL)
			{
				printf("malloc in FILE[%s],FUNCTION[%s],LINE[%d]", __FILE__, __FUNCTION__, __LINE__);
				perror("error:");
				return FALSE;
			}
			memcpy(pvalue, value, sizeof(RECT));
		}
		else if(HASH_GENERAL_KEY == flag)
		{
			pvalue = (void*) malloc(sizeof(int));
			if (pvalue == NULL)
			{
				printf("malloc in FILE[%s],FUNCTION[%s],LINE[%d]", __FILE__, __FUNCTION__, __LINE__);
				perror("error:");
				return FALSE;
			}
			memcpy(pvalue, value, sizeof(int));
		}
		else
		{
			return FALSE;
		}
		pHash->flags = flag;
		pHash->value = pvalue;
	}

	return TRUE;
}

static void* HashInit(int backetLen)
{
	int i;
	hash_counter_entry_t *handle = NULL;
	if (backetLen <= 0)
	{
		return FALSE;
	}
	handle = (hash_counter_entry_t *) malloc(sizeof(hash_counter_entry_t));
	if (NULL == handle)
	{
		printf("malloc in FILE[%s],FUNCTION[%s],LINE[%d]", __FILE__, __FUNCTION__, __LINE__);
		perror("error:");
		return -1;
	}
	handle->keys = (hash_key_t **) malloc(sizeof(hash_key_t*) * backetLen);
	if (NULL == handle->keys)
	{
		printf("malloc in FILE[%s],FUNCTION[%s],LINE[%d]", __FILE__, __FUNCTION__, __LINE__);
		perror("error:");
		return -1;
	}
	handle->backetLen = backetLen;

	memset(handle->keys, 0, sizeof(hash_key_t*) * backetLen);

	return handle;

}

void HashDestroy(const void* handle)
{
	int i;
	hash_key_t *pentry, *p;
	hash_counter_entry_t *phandle = (hash_counter_entry_t *) handle;
	if (handle == NULL || NULL == phandle->keys)
	{
		return;
	}

	for (i = 0; i < phandle->backetLen; ++i)
	{
		pentry = phandle->keys[i];
		while (pentry)
		{
			p = pentry->next;
			if (pentry->key)
			{
				free(pentry->key);
			}
			if (pentry->value)
			{
				free(pentry->value);
			}
			free(pentry);
			pentry = p;
		}
	}

	free(phandle->keys);
	phandle->keys = NULL;
	free(phandle);

}

void* InitMigConfig(const char *filepath)
{
	FILE *fp = NULL;
	hash_counter_entry_t* pHead = NULL;
	char line[LINE_BUF_SIZE];
	char keybuff[KEY_SIZE] = { 0 };
	char valuebuff[VALUE_SIZE] = { 0 };
	char *pLine = NULL;

	if (filepath == NULL)
	{
//		printf(" in FILE[%s],FUNCTION[%s],LINE[%d] error:filepath is EMPTY \n", __FILE__, __FUNCTION__, __LINE__, filepath);
		return NULL;
	}

	fp = fopen(filepath, "r");
	if (!fp)
	{
		printf("FILE[%s],FUNCTION[%s],LINE[%d] Warning:no such file[%s] \n",__FILE__, __FUNCTION__,__LINE__,filepath);
		return NULL;
	}

	pHead = HashInit(BUCKETS);

	if (pHead == NULL)
	{
		printf("Softkeyboard FILE[%s],FUNCTION[%s],LINE[%d] error:HashInit pHead[%d] \n", __FILE__, __FUNCTION__, __LINE__, pHead);
		fclose(fp);
		return NULL;
	}

	// 读取配置文件中的所有数据
	while (!feof(fp))
	{
		void *pValue = NULL;
		RECT rValue ;
		int iValue = 0;
		int iFlag = 0;
		memset(&rValue, 0, sizeof(RECT));
		memset(keybuff, 0, KEY_SIZE);
		memset(valuebuff, 0, VALUE_SIZE);
		if (fgets(line, LINE_BUF_SIZE, fp) == NULL)
		{
			break;
		}

		if ((pLine = strstr(line, "=")) == NULL)
		{
			continue;
		}

		if (trimeSpace(line, pLine - line, keybuff) <= 0)
		{
			continue;
		}

		// 设置Value
		pLine += 1;
		if (trimeSpace(pLine, strlen(pLine), valuebuff) <= 0)
		{
			continue;
		}

		if (!Is_Digit(valuebuff))
		{
			char *k_start = valuebuff;
			if ((NULL != strstr(keybuff, "RECT_")) && (*k_start == '{'))
			{
				char* token = NULL;
				int *prv = &rValue;
				int i;
				token = strstr(k_start + 1, "}");
				if (NULL == token)
				{
					continue;
				}
				*token = '\0';
				token = strtok(k_start + 1, ",");
				for (i = 0;NULL != token && i < 4; i++)
				{
					char temstr[11] = { 0 };
					trimeSpace(token, strlen(token), temstr);
					if (!Is_Digit(temstr))
					{
						int *piv = NULL;
						int flag;
						piv = HashFind(pHead, temstr, &flag);
						if (NULL != piv && HASH_GENERAL_KEY == flag)
						{
							prv[i] = *piv;
						}
						else
						{
//							printf("keybuff=%s,temstr=%s\n",keybuff,temstr);
							break;
						}
					}
					else
					{
						prv[i] = atoi(temstr);
					}
					token = strtok(NULL, ",");
				}
				if (4 == i)
				{
					iFlag = HASH_RCT_KEY;
					pValue = prv;
				}
			}

		}
		else
		{
			iFlag = HASH_GENERAL_KEY;
			iValue = atoi(valuebuff);
			pValue = &iValue;
		}
		if (NULL != pValue)
		{
			HashInsertKey(pHead, keybuff, pValue, iFlag);
#if  0  //调试用
			pValue=HashFind(pHead, keybuff, &iFlag);
			if(pValue != NULL)
			{
				if(HASH_GENERAL_KEY == iFlag)
					printf("%s=%d\n",keybuff,*((int*)pValue));
				else if(HASH_RCT_KEY == iFlag)
					printf("%s={%d,%d,%d,%d}\n",keybuff,((PRECT)pValue)->left,((PRECT)pValue)->top,((PRECT)pValue)->right,((PRECT)pValue)->bottom);
			}
#endif
		}

	}

	fclose(fp);

	return (void*)pHead;

}

int FindMigCfgIntValue(const void* handle, const char* key)
{
	int flag = 0;
	void *pValue = NULL;
	if (handle == NULL || NULL == key)
	{
		return -1;
	}
	pValue = HashFind(handle, key, &flag);
	if (NULL == pValue || HASH_GENERAL_KEY != flag)
	{
//		printf("\033[4;31m HashFind error handle[%0x] key[%s] in FILE[%s],FUNCTION[%s],LINE[%d]\033[0m\n",handle,key, __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
//	printf("%s=%d\n",key,*((int*)pValue));

	return (*(int*) pValue);
}

void* FindMigCfgRectValue(const void* handle, const char* key)
{
	int flag = 0;
	void *pValue = NULL;

	if (handle == NULL || NULL == key)
	{
		return NULL;
	}

	pValue = HashFind(handle, key, &flag);
	if (NULL == pValue || HASH_RCT_KEY != flag)
	{
//		printf("\033[4;31m HashFind error handle[%0x] key[%s] in FILE[%s],FUNCTION[%s],LINE[%d]\033[0m\n",handle,key, __FILE__, __FUNCTION__, __LINE__);
		return NULL;
	}
//	printf("%s={%d,%d,%d,%d}\n",key,((PRECT)pValue)->left,((PRECT)pValue)->top,((PRECT)pValue)->right,((PRECT)pValue)->bottom);

	return ((RECT*) pValue);
}

void ReleaseMigCfg(const void* handle)
{
	if (handle == NULL)
	{
		return ;
	}
	HashDestroy(handle);

}

