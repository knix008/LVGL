//
//  Properties.h
//  ¶ÁĞ´ÅäÖÃÎÄ¼ş
//
 
#ifndef __Properties_h
#define __Properties_h
 
#ifdef _cplusplus
extern "C" {
#endif
     
void* InitMigConfig(const char *filepath);
int FindMigCfgIntValue(const void* handle,const char* key);
void* FindMigCfgRectValue(const void* handle,const char* key);
void ReleaseMigCfg(const void* handle);


#ifdef _cplusplus
}
#endif
 
#endif

