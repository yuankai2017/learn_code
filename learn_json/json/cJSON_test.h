#ifndef __CJSON_TEST_H__
#define __CJSON_TEST_H__

//2.JSON结构体

//熟悉使用cJSON库函数可从cJSON结构体入手，cJSON结构体如下所示：
typedef struct cJSON {  
 struct cJSON *next,*prev;     /* 如果该Json结构在数组中，则它的前后节点*/
 struct cJSON *child;          /* 如果存在子对象，其第一个子对象*/
 int type;                     /* Json对象的类型*/
 char *valuestring;            /* The item's string, if type==cJSON_String*/
 int valueint;                 /* The item's number, if type==cJSON_Number*/
 double valuedouble;           /* The item's number, if type==cJSON_Number*/
 char *string;                 /* Json对象的名字*/
} cJSON;  

//Json对象的类型
/* cJSON Types: */
#define cJSON_False   0
#define cJSON_True    1
#define cJSON_NULL    2
#define cJSON_Number  3
#define cJSON_String  4
#define cJSON_Array   5
#define cJSON_Object  6


//extern cJSON *cJSON_Parse(const char *value);
//extern char  *cJSON_Print(cJSON *item);
//extern void cJSON_Delete(cJSON *c);
//extern int cJSON_GetArraySize(cJSON *array);
//extern cJSON *cJSON_GetArrayItem(cJSON *array,int item);
//extern cJSON *cJSON_GetObjectItem(cJSON *object,const char *string);

/*
函数功能:解析函数
函数定义:	extern cJSON *cJSON_Parse(const char *value)
函数说明	:传入一个字符串，返回一个cJSON，可以通过if (!json)
         来判断解析是否出错，如果出错，可以通过printf("Error before: [%s]\n",cJSON_GetErrorPtr())打印出错位置。

函数功能	:打印cJSON
函数定义	:extern char  *cJSON_Print(cJSON *item)
函数说明	:传入一个cJSON，将会以排好版的格式打印JSON。

函数功能	:释放cJSON内存空间
函数定义	:extern void cJSON_Delete(cJSON *c)
函数说明:获得了一个cJSON，当使用完了之后要删除，因为返回的cJSON是通过malloc的方式分配的，如果用完不及时释放会造成内存泄露。

函数功能	:获取cJSON大小
函数定义	:extern int cJSON_GetArraySize(cJSON *array)
函数说明	:返回数组或对象中的大小，只要该对象下包括其他对象，各对象一般以“,”分隔。

函数功能	:以index的方式获取cJSON数组或对象相应的项
函数定义	:extern cJSON *cJSON_GetArrayItem(cJSON *array,int item)
函数说明	:返回数组或对象中相应index的项，找不到会返回NULL。

函数功能:	以名称的方式获取cJSON数组或对象相应的项
函数定义:	extern cJSON *cJSON_GetObjectItem(cJSON *object,const char *string)
函数说明	:获取当前cJSON对象下有名字的cJSON对象，找不到会返回NULL。
*/



#endif
