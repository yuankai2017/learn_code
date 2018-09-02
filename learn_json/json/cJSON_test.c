#include "json.h"
#include "cJSON_test.h"
#include "cJSON.h"


/*几点说明
1.cJOSN结构体为一个双向列表，并可通过child指针访问下一层。
2.type变量决定数据项类型（键的类型），数据项可以是字符串可以是整形，也可以是浮点型。如果是整形值的话可从valueint，如果是浮点型的话可从valuedouble取出，以此类推。
3.string可理解为节点的名称，综合此处的第2点可理解为“键”的名称。
*/

//3.解析JSON数据包
/*
例如在yeelink平台中读取一个开关量的结果，向yeelink平台请求之后可以获得以下JSON数据包：
{"timestamp":"2013-11-19T08:50:11","value":1}
在这个JSON数据包中有两个数据项（键值对），一个是时间戳，该时间戳为字符串形式；另一个是开关值，该开关值为整型。该例子主要用于模拟向yeelink平台请求开关量数据。
参考代码
*/

#include <stdio.h>  
#include <stdlib.h>  
#include "cJSON.h"  
// 被解析的JSON数据包  
char text[] = "{\"timestamp\":\"2013-11-19T08:50:11\",\"value\":1}";  
int main (int argc, const char * argv[])  
{  
    cJSON *json , *json_value , *json_timestamp;  
    // 解析数据包  
    json = cJSON_Parse(text);  
    if (!json)  
    {  
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
    }  
    else  
    {  
        // 解析开关值  
        json_value = cJSON_GetObjectItem( json , "value");  
        if( json_value->type == cJSON_Number )  
        {  
            // 从valueint中获得结果  
            printf("value:%d\r\n",json_value->valueint);  
        }  
        // 解析时间戳  
        json_timestamp = cJSON_GetObjectItem( json , "timestamp");  
        if( json_timestamp->type == cJSON_String )  
        {  
            // valuestring中获得结果  
            printf("%s\r\n",json_timestamp->valuestring);  
        }  
        // 释放内存空间  
        cJSON_Delete(json);  
    }  
    return 0;  
}  
/*
运行结果
value:1
2013-11-19T08:50:11
*/



/*
若干说明
1.cJSON_Parse函数负责解析JSON数据包，并按照cJSON结构体的结构序列化整个数据包。使用该函数
会通过malloc函数在内存中开辟一个空间，使用完成需要手动释放。
2.cJSON_GetObjectItem函数可从cJSON结构体中查找某个子节点名称（键名称），如果查找成功可把该子节点序列化到cJSON结构体中。
3.如果需要使用cJSON结构体中的内容，可通过cJSON结构体中的valueint和valuestring取出有价值的内容（即键的值）
4.可通过cJSON_Delete释放内存空间。
*/


//4.组装JSON数据包
/*
组装数据包的过程和解析数据包的过程相反，下面的例子描述如何组装以下数据包，该数据包只有一个数据项（键值对）。该例子主要用于模拟向yeelink平台上传传感器数据。
{
 "value": 123.400000
}
*/

#include <stdio.h>  
#include <stdlib.h>  
#include "cJSON.h"  
int main (int argc, const char * argv[])  
{  
    // 创建JSON Object  
    cJSON *root = cJSON_CreateObject();  
    // 加入节点（键值对），节点名称为value，节点值为123.4  
    cJSON_AddNumberToObject(root,"value",123.4);  
    // 打印JSON数据包  
    char *out = cJSON_Print(root);  
    printf("%s\n",out);  
    // 释放内存  
    cJSON_Delete(root);  
    free(out);  
    return 0;  
}  

/*
若干说明

1. cJSON_CreateObject函数可创建一个根数据项，之后便可向该根数据项中添加string或int等内容
2. cJSON_AddNumberToObject向节点中添加子节点，例如此处添加value节点，节点值为123.4
3. cJSON_Print函数可以打印根数据项，加入制表符换行符等标识符使得JSON数据包更易阅读
4. 使用free函数释放被out占用的内存空间

*/


/*
1.重要函数说明
【1】两个创建
【创建JSON对象】cJSON *cJSON_CreateObject(void);
【创建JSON数组】cJSON *cJSON_CreateArray(void);

【2】两种添加
【向对象中添加】voidcJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item);
【向数组中添加】void cJSON_AddItemToArray(cJSON *array, cJSON *item);

【3】常用几招
【向对象中增加数字】cJSON_AddItemToObject(root, "value", cJSON_CreateNumber(value));
【向对象中增加文件】cJSON_AddItemToObject(root, "string", cJSON_CreateString(string));

【4】JSON嵌套
【向对象中增加数组】cJSON_AddItemToObject(root, "rows", rows = cJSON_CreateArray());
【向数组中增加对象】cJSON_AddItemToArray(rows, row = cJSON_CreateObject());
*/



/*
2.创建各种各样的JSON数据包
  在这里通过代码举几个例子，更多的内容请查看代码仓库中的相关文件。   
*/
//[1]JSON数字
void create_single_number(void) 
{  
    cJSON *root;  
    char *out;  
    int value = 24;  
  
    root = cJSON_CreateObject(); // 创建根  
  
    cJSON_AddItemToObject(root, "value", cJSON_CreateNumber(value));  
  
    // 打印并释放  
    out = cJSON_Print(root); cJSON_Delete(root); printf("%s\n",out); free(out);  
  
    // 控制台输出  
#if 0  
    {  
        "value": 24  
    }  
#endif  
}  
/*
【简单说明】
【1】cJSON_AddItemToObject(root, "value", cJSON_CreateNumber(value)); 
【2】cJSON_AddNumberToObject(root, "value", value); 
【1】和【2】效果完全相同。
*/

//[2]JSON字符串
void create_single_string(void)
{  
    cJSON *root;  
    char *out;  
    char *name = "xukai871105";  
  
    root = cJSON_CreateObject(); // 创建根  
    // 方法 使用cJSON_AddItemToObject，推荐使用  
    cJSON_AddItemToObject(root, "name", cJSON_CreateString(name));  
  
    // 打印并释放  
    out = cJSON_Print(root); cJSON_Delete(root); printf("%s\n",out); free(out);  
  
    // 控制台输出  
#if 0  
    {  
        "name": "xukai871105"  
    }  
#endif  
}  
/*
【简单说明】
【1】 cJSON_AddItemToObject(root, "name", cJSON_CreateString(name));
【2】 cJSON_AddStringToObject(root, "name",name);
【1】和【2】效果完全相同。
*/


// 【3】JSON布尔类型
void create_bool(void)
{  
    cJSON *root;  
    char *out;  
  
    root = cJSON_CreateObject(); // 创建根  
  
    cJSON_AddItemToObject(root, "success", cJSON_CreateFalse());  
  
    // 打印并释放  
    out = cJSON_Print(root); cJSON_Delete(root); printf("%s\n",out); free(out);  
  
    // 控制台输出  
#if 0  
    {  
        "success": false  
    }  
#endif  
}  
/*
[简单说明]
[1]布尔类型不需要加引号。
*/



/*
3.JSON格式嵌套
JSON格式在使用时往往存在嵌套，例如JSON对象中嵌套JSON数组而JSON数组中嵌套JSON对象，下面
就通过几个简单的例子说明问题。
*/

//[1]JSON简单嵌套
void create_simple(void)
{  
    cJSON *root;  
    char *out;  
    int list[4] = {5,6,7,8};  
  
    root = cJSON_CreateObject(); // 创建根  
    cJSON_AddItemToObject(root, "lists", cJSON_CreateIntArray(list, 4));  
  
    // 打印并释放  
    out = cJSON_Print(root); cJSON_Delete(root); printf("%s\n",out); free(out);  
  
    // 控制台输出  
#if 0  
    {  
        "lists": [5, 6, 7, 8]  
    }  
#endif  
}  


//[2]JSON复杂嵌套
void create_complex(void)
{  
    cJSON *root, *rows, *row;  
    char *out;  
    int i = 0;  
      
    char *title[3] = { "树莓派学习笔记——索引博文",  
                       "树莓派学习笔记——GPIO功能学习",  
                       "物联网学习笔记——索引博文"};  
    char *url[3] = { "http://blog.csdn.net/xukai871105/article/details/23115627",  
                     "http://blog.csdn.net/xukai871105/article/details/12684617",  
                     "http://blog.csdn.net/xukai871105/article/details/23366187"};  
      
    root = cJSON_CreateObject(); // 创建根  
    cJSON_AddNumberToObject(root, "total", 3);  
      
    // 在object中加入array  
    cJSON_AddItemToObject(root, "rows", rows = cJSON_CreateArray());  
      
    for(i = 0; i < 3; i++) 
    {  
        // 在array中加入object  
        cJSON_AddItemToArray(rows, row = cJSON_CreateObject());  
        cJSON_AddItemToObject(row, "title", cJSON_CreateString(title[i]));  
        cJSON_AddItemToObject(row, "url", cJSON_CreateString(url[i]));  
    }  
      
    // 打印并释放  
    out = cJSON_Print(root); cJSON_Delete(root); printf("%s\n",out); free(out);  
      
    // 控制台输出  
#if 0  
    {  
    "total": 3,  
    "rows": [{  
    "title": "树莓派学习笔记——索引博文",  
    "url": "http://blog.csdn.net/xukai871105/article/details/23115627"  
    }, {  
    "title": "树莓派学习笔记——GPIO功能学习",  
    "url": "http://blog.csdn.net/xukai871105/article/details/12684617"  
    }, {  
    "title": "物联网学习笔记——索引博文",  
    "url": "http://blog.csdn.net/xukai871105/article/details/23366187"  
    }]  
    }  
#endif  
}  
/*
[简单说明]
rows为JSON对象，rows对象中嵌套JSON数组，每一个JSON数组的元素又是一个JSON对象，该该对象名为row，row对象中具有两个键值对，分别是titile和url。
*/
    



