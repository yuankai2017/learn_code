#ifndef __JSON_C_TEST_H__
#define __JSON_C_TEST_H__

/*1. json_object_get与json_object_put函数*/ 

struct json_object* json_object_get(struct json_object *jso); // 将json对象的引用计数加1，返回jso自身

void json_object_put(struct json_object *jso); // 
将jso对象的引用计数减1。此时如果json的引用计数大于0，直接返回; 
如果json的引用计数为0，则递归的对每个子对象的引用计数减1，子对象引用计数为0时释放所占用内存。

注：如果对json对象显式调用了json_object_get，之后必须成对调用json_object_put，否则将导致该json对象所占用内存泄漏。

/*2. json_object_new_XXX系列函数 struct json_object* json_object_new_object(void);*/

struct json_object* json_object_new_int(int i);

struct json_object* json_object_new_double(double d);

struct json_object* json_object_new_array(void);

struct json_object* json_object_new_string(const char *s);

struct json_object* json_object_new_string_len(const char *s, int len);

json_object_new_XXX系列函数用来创建XXX类型的json对象，创建的json对象默认引用计数为1，因此在该对象使用完后也需要调用一次json_object_put来把引用计数置0，从而释放内存。

/*3. json_tokener_parse函数*/ 

struct json_object* json_tokener_parse(const char *str);

json_tokener_parse将符合json格式的字符串构造为一个json对象，构造的json对象默认引用计数为1，同样需要在使用完后对该对象调用一次json_object_put。

/*4. is_error宏*/

is_error(jso)

如果传入的字符串是非法的json格式，错误判断应该使用is_error宏，而非 if(NULL != jso)，CGI中目前有很多这种错误用法(虽然好像没引发什么问题)

/*5. json_object_object_XXX函数*/

void json_object_object_del(struct json_object* jso, const char *key);

从jso对象中删除键值为key的子对象，并释放该子对象及键值所占的内存（注：可能有文档说json_object_object_del只是删除而不释放内存，但实际上这是错误的）。

struct json_object* json_object_object_get(struct json_object* jso, const char *key); 
从jso中获取键值为key的子对象。错误判断同样应该用is_error(jso)宏。

void json_object_object_add(struct json_object* jso, const char *key, struct json_object *val); 
更新键值为key的子项的值。整个过程实际上是这样的：先从jso中删除并释放键值key及其值的内存，然后重新分配内存添加键值和新的值，所以json_object_object_add是包含json_object_object_de
l操作的。

/*6. json_object_get_XXX系列函数*/ 

struct lh_table* json_object_get_object(struct json_object *jso);

const char* json_object_get_string(struct json_object *jso);

int json_object_get_int(struct json_object *jso);

double json_object_get_double(struct json_object *jso); 
这类函数只是获取json对象的实际数据内容，不更新引用计数，也不分配内存。

/*7. json_object_array_XXX系列函数*/
struct json_object* json_object_array_get_sub_array(struct json_object *jso, int start_idx, int number);

这个函数用来从一个json数组对象中取数组序号start_idx开始的、总共number长度的子数组对象。分页显示功能常用到。注：返回的子数组是有重新分配内存的，所以同样要对返回的json_object*做一次json_object_put操作来释放内存。

int json_object_array_add(struct json_object *jso,struct json_object *val); 向数组中添加一个值。

int json_object_array_length(struct json_object *jso); 获取数组长度。

int json_object_array_put_idx(struct json_object *jso, int idx, struct json_object *val); 
更新数组中序号为idx那一项的值，老的值同样会先被释放。

struct json_object* json_object_array_get_idx(struct json_object *jso, int idx); 
获取数组中序号为idx那一项的json对象，不更新引用计数，也不分配内存。

struct json_object* json_object_array_sort(struct json_object *jso, const char *keyname, int sord ); 
根据键值为keyname的项的值进行升序或降序排序，只是改变数组成员的顺序，不更新引用计数，也不分配内存。

#endif



/*--------------------------------------------------------*/
一、创建json对象

（1）函数：externstruct json_object* json_object_new_object(void);

函数说明：创建一个新的json对象，引用计数1，该指针具有唯一的所有权。当使用json_object_object_add         

或者json_object_array_put_idx作用于该对象时，所有权转移到另一方。使用json_object_get作用于该对象的所有权，必须使用json_object_put释放。

（2）函数：externstruct json_object* json_object_new_array(void);

函数说明：创建的json数组类型对象

（3）//创建一个json 的bool对象。

extern struct json_object*json_object_new_boolean(json_bool b);

（4）//创建josn的int 32、64对象

externstruct json_object* json_object_new_int(int32_t i);

externstruct json_object* json_object_new_int64(int64_t i);

（5）//将c字符串转换为json字符串格式的对象，该对象堆上分配

externstruct json_object* json_object_new_string(const char *s);

externstruct json_object* json_object_new_string_len(const char *s, int len);

（6）/* 创建double类型的json对象 */

externstruct json_object* json_object_new_double(double d);


二、增加、删除、修改

（1）//给json对象增加字段

//引用计数不会增加，val的所有权转移到obj。

externvoid json_object_object_add(struct json_object* obj, const char *key,structjson_object *val);

（2）//
删除json对象的指定字段，被删除的对象引用计数减去1，如果这个val没有更多的所有者，这
个key对应

//的val被free，否则这个val的引用保存在内存中

externvoid json_object_object_del(struct json_object* obj, const char *key);

（3）//增加一个元素到json数组的末尾。

//obj引用计数不会增加，增加字段的方式更加紧凑。

//如果需要获取val的引用，需要用json_object_get来传递该对象。

externint json_object_array_add(struct json_object *obj, struct json_object *val);

（4）//替换json数组中的值

externint json_object_array_put_idx(struct json_object *obj, int idx,

structjson_object *val);

（5）//json数组的排序，这里需要自己写排序函数

externvoid json_object_array_sort(struct json_object *jso, int(*sort_fn)(const void*, const void *));

二、取值

（1）//获取json对象的长度，依据字段的数目

extern intjson_object_object_length(struct json_object* obj);

（2）//获取json对象的哈希表

extern struct lh_table*json_object_get_object(struct json_object *obj);

（3）//从obj中获取key对应的json对象，引用计数没有改变

extern struct json_object*json_object_object_get(struct json_object* obj,const char *key);

（4）//获取对象的数组列表

extern struct array_list*json_object_get_array(struct json_object *obj);

（5）//获取json的类型

externenum json_type json_object_get_type(struct json_object *obj);

（6）//获取json数组对象的长度

extern intjson_object_array_length(struct json_object *obj);

（7）//获取json对象bool值。int和double对象是0转换为FALSE，否则返回TRUE。

//非0长度的字符串返回TRUE。其他对象非空的话，返回TRUE。

extern json_booljson_object_get_boolean(struct json_object *obj);

（8）// 获取json对象的长度,如果参数不是string类型的json，返回0

extern intjson_object_get_string_len(struct json_object *obj);

（9）//按照索引获取json数组的对象

externstruct json_object* json_object_array_get_idx(struct json_object *obj, intidx);

三、类型转换

（1）      extern const char*json_object_to_json_string(struct json_object *obj);

转换json对象到c字符串格式

（2）//如果参数非json类型的字符串，返回thenthe JSON representation of the object is returned.
参数对应的json对象内存空间，当其引用计数为0时，该内存free。

obj是json对象实例；回c格式的字符串；

externconst char* json_object_get_string(struct json_object *obj);

（3）//同上，不同的是flags： formattingoptions, see JSON_C_TO_STRING_PRETTY and other constants

externconst char* json_object_to_json_string_ext(struct json_object *obj, int flags);

（4）//如果传递的非int，强制转换为int。double对象返回其int。字符串解析为int。

//如果没有转换存在返回0.并且error置为EINVVAL，0和null等价。

//整形以64位值存储，如果太小或是太大，不足以填充32位，将返回INT32_MAX，INT32_MIN

externint32_t json_object_get_int(struct json_object *obj);

（5）//获取json对象的int64_t、double值

externint64_t json_object_get_int64(struct json_object *obj);

externdouble json_object_get_double(struct json_object *obj);

（6）将字符串转换为json对象

extern struct json_object* json_tokener_parse(const char *str);


四、json对象的释放

//以下两个函数配合使用，前者获取该对象指针的所有权， 
引用计数加1，如果对象已经被释放，返回1

extern  struct json_object* json_object_get(struct json_object *obj);

//后者引用计数减1，如果对象已经被释放，返回1

extern int  json_object_put(struct json_object *obj);

五、其他

（1）//  类型判断

externint json_object_is_type(struct json_object *obj, enum json_type type);

（2）//.....这里省略了json迭代器的使用方法
/*--------------------------------------------------------*/


/*=================================================================================*/
json-c的api介绍：

json_object.h文件常用的api

/*
增加obj的引用计数
*/
struct json_object* json_object_get(struct json_object *obj);

/*
减少obj的引用计数，当引用计数为0时，释放obj实例所占用的内存
*/
int json_object_put(struct json_object *obj);

/*
检查obj是否是给定的类型
type是下列之一：
     json_type_null (i.e. obj == NULL),
     json_type_boolean,
     json_type_double,
     json_type_int,
     json_type_object,
     json_type_array,
     json_type_string,
*/
int json_object_is_type(struct json_object *obj, enum json_type type);

/*
获取obj类型
*/
enum json_type json_object_get_type(struct json_object *obj);

/*
将obj实例转化成一个json格式的字符串
等同于json_object_to_json_string_ext(obj, JSON_C_TO_STRING_SPACED)
返回一个json格式的字符串指针
*/
const char* json_object_to_json_string(struct json_object *obj);

/*
将obj实例转化成一个json格式的字符串
flags：
JSON_C_TO_STRING_PLAIN
JSON_C_TO_STRING_SPACED 
JSON_C_TO_STRING_PRETTY 
JSON_C_TO_STRING_NOZERO
*/
const char* json_object_to_json_string_ext(struct json_object *obj, int flags);
/*
构建一个json对象
*/
struct json_object* json_object_new_object(void);
/*
获取一个obj实例中项目的数量
*/
int json_object_object_length(struct json_object* obj);

/*
向obj实例中增加一个键值对
*/
void json_object_object_add(struct json_object* obj, const char *key,struct json_object *val);

/*
该函数被弃用，请用 json_object_object_get_ex
*/
struct json_object* json_object_object_get(struct json_object* obj,const char *key)

/*
从obj实例中获取键key对应的json对象，并将找到的json对象指针存放到value中，
该函数不会改变引用计数
成功返回TRUE
失败返回FALSE
*/
json_bool json_object_object_get_ex(struct json_object* obj,const char *key, struct json_object **value);

/*
删除给定的json字段，实际上只是减少引用计数，当引用计数为0时才会真正删除（释放）该
字段
*/
void json_object_object_del(struct json_object* obj, const char *key);

/*
这是一个宏定义，遍历obj实例的所有键值对
*/
json_object_object_foreach(obj,key,val)

/*
建立一个数组类型的json对象，返回json_type_array类型的对象
*/
struct json_object* json_object_new_array(void)

/*
获取obj对象中的数组对象列表，obj必须是json_type_array类型
*/
struct array_list* json_object_get_array(struct json_object *obj);

/*
获取obj对象的长度，obj必须是json_type_array类型
*/
int json_object_array_length(struct json_object *obj);

/*
增加一个元素到obj的末尾，obj必须是json_type_array类型
*/
int json_object_array_add(struct json_object *obj,struct json_object *val);

/*
插入或替换数组obj中的对应索引下的值，obj必须是json_type_array类型
*/
int json_object_array_put_idx(struct json_object *obj, int idx, struct json_object *val);

/*
获取数组obj中的对应索引下的对象，obj必须是json_type_array类型
*/
struct json_object* json_object_array_get_idx(struct json_object *obj,int idx);

/*
创建一个空的json_type_boolean类型的json对象
*/
struct json_object* json_object_new_boolean(json_bool b);

/*
获取obj对象中的值，obj必须为json_type_boolean类型
对于字符串非空或int,double非零的情况下都返回TRUE,否则返回FALSE
*/
json_bool json_object_get_boolean(struct json_object *obj);

struct json_object* json_object_new_int(int32_t i);
struct json_object* json_object_new_int64(int64_t i);
struct json_object* json_object_new_double(double d);
struct json_object* json_object_new_double_s(double d, const char *ds);
struct json_object* json_object_new_string(const char *s);
struct json_object* json_object_new_string_len(const char *s, int len);

int32_t json_object_get_int(struct json_object *obj);
int64_t json_object_get_int64(struct json_object *obj);
double json_object_get_double(struct json_object *obj);
const char* json_object_get_string(struct json_object *obj);
int json_object_get_string_len(struct json_object *obj);

json_tokener.h文件 
主要实现了json文本解析器 
主要介绍下面几个函数

/*
将一个字符串解析成json对象并返回对象指针
str:待解析的字符串
tok:先前由json_tokener_new()创建的对象
len:字符串str长度
*/

struct json_object* json_tokener_parse(const char *str); //
建议直接使用此函数，最后记得用json_object_put();释放内存
struct json_object* json_tokener_parse_ex(struct json_tokener *tok,const char *str, int len);
/*
调试用，打印错误信息
*/
json_tokener_get_error(struct json_tokener *tok)

/*
构建和销毁json文本解析器
*/
struct json_tokener* json_tokener_new(void);
void json_tokener_free(struct json_tokener *tok);

/*=================================================================================*/