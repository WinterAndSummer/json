#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include "json.h"

#define MAX_KEY_LEN 100
typedef struct array array;
typedef struct object object;
typedef struct value value;
typedef struct keyvalue keyvalue;




/**
 *  想想：这些结构体定义在.c是为什么？
 */

/**
 *  想想：如果要提升内存分配效率，这个结构体该作什么变化？
 */
struct array {
    value **elems;      /* 想想: 这里如果定义为'value *elems'会怎样？ */
    U32 count;
};

struct keyvalue {
    char *key;
    value *val;
};

/**
 *  想想：如果要提升内存分配效率，这个结构体该作什么变化？
 */
struct object {
    keyvalue *kvs;      //这是一个keyvalue的数组，可以通过realloc的方式扩充的动态数组
    U32 count;          //数组kvs中有几个键值对
};

//联合体怎么初始化
struct value {
    json_e type;
    union {
        double num;
        BOOL bol;
        char *str;
        array arr;
        object obj;
    };
};
/**
 * 从对象类型的JSON值中获取名字为key的成员(JSON值)
 * @param json 对象类型的JSON值
 * @param key  成员的键名
 * @return 找到的成员
 */
const JSON *json_get_member(const JSON *json, const char *key)
{
    U32 i;
    assert(json);
    if (json->type != JSON_OBJ)
        return NULL;
    assert(json->obj.count == 0 || json->obj.kvs);
    for (i = 0; i < json->obj.count; ++i) {
        if (strcmp(json->obj.kvs[i].key, key) == 0)
            return json->obj.kvs[i].val;
    }
    return NULL;
}
    

/**
 * 从数组类型的JSON值中获取第idx个元素(子JSON值)
 * @param json 数组类型的JSON值
 * @param idx  元素的索引值
 * @return 找到的元素(JSON值的指针)
 */
const JSON *json_get_element(const JSON *json, U32 idx)
{
    assert(json);
    assert(json->type == JSON_ARR);

    if (json->type != JSON_ARR)
        return NULL;
    if (idx >= json->arr.count)
        return NULL;
    return json->arr.elems[idx];
}
/**
 * 新建一个类型为type的JSON值
 * @param type 待创建JSON值的类型
 * @return JSON值
 */
JSON *json_new(json_e type)
{
    JSON *json = (JSON *)calloc(1, sizeof(JSON));
    if (!json)
        return NULL;
    json->type = type;
    return json;
}



/**
 * 释放一个JSON值
 * @param json json值
 * @details
 * 该JSON值可能含子成员，也要一起释放
 */ 
 void keyvalue_free(keyvalue* kvs, int count);
 
void json_free(JSON *json)
{
    //TODO:
	if (!json)
	{
		return;
	}
	switch (json->type)
	{
	case JSON_NONE:
        return;
        break;
	case JSON_BOL:
        break;
	case JSON_NUM:
        break;
	case JSON_STR:
        if (json->str)
        {
            free(json->str);
            json->str=NULL;
        }
        break;
	case JSON_ARR:
        for (int i=0;i<json->arr.count;i++)
        {
            json_free(json->arr.elems[i]);
        }
        break;
	case JSON_OBJ:
        keyvalue_free(json->obj.kvs, json->obj.count);
        break;
	}
	free(json);
	return ;
}

void keyvalue_free(keyvalue* kvs, int count)
{
	if (!kvs)
	{
		return;
	}
	for (int i=0; i<count; i++)
	{
		if (kvs->val)
		{
			json_free(kvs->val);
		}
	}
	free(kvs);
	kvs=NULL;
	return;
}



/**
 * 获取JSON值json的类型
 * @param json json值
 * @return json的实际类型
 */
json_e json_type(const JSON *json)
{
    assert(json);
    return json ? json->type : JSON_NONE;
}
/**
 * 设置JSON对象的成员值
 * @param json 对象类型的JSON值
 * @param key 键名
 * @param val 值
 * @details
 * 如果这个成员之前不存在，则新建一个键值对，如果存在，则修改该键值对
 */
int json_obj_set_value(JSON *json, char *key, JSON *val)
{
    //检查输入
	if (!json||!val)
	{
		goto failed_;
	}
	if (strlen(key)>MAX_KEY_LEN)
	{
		goto failed_;
	}
	if (!json->obj.kvs)
	{
		json->obj.kvs=(keyvalue*)malloc(sizeof(keyvalue));
	}
	else
	{
		json->obj.kvs=(keyvalue*)realloc(json->obj.kvs,sizeof(keyvalue)*(json->obj.count+1));
	}
	json->obj.kvs[json->obj.count].val=val;
	json->obj.kvs[json->obj.count].key=key;
	json->obj.count++; 
	return 0;
failed_:	
    return -1;
}

/**
 * 删除JSON对象的成员值
 * @param json 对象类型的JSON值
 * @param key 键名
 * @param val 值
 * @details
 * 如果这个成员之前不存在，则新建一个键值对，如果存在，则修改该键值对
 */
int json_obj_del_value(JSON *json,  char *key)
{
    assert(json);
    assert(key);
    if(json->type != JSON_OBJ){
        fprintf(stderr, "del_value failed, json value is not JSON_OBJ");
        return -1;
    }
    
    int count = json->obj.count;
    int index = 0;
    for(index = 0;index < count; index++){
        if(strcmp(json->obj.kvs[index].key, key) == 0){
            break;
        }
    }
    if(index == count){
        fprintf(stderr, "del_value failed, can not find the key");
        return -1;
    }
    for(int i = index; i < count-1; i++){
        json->obj.kvs[i] = json->obj.kvs[i+1];
    }
    json->obj.count--;
    return 0;
}

int json_arr_add_value(JSON *json, JSON* val)
{
    assert(json);
    assert(val);
    if(json->type != JSON_ARR){
        fprintf(stderr, "add_value failed,val is not JSON_ARR");    
        return -1;
    }

    json->arr.count++;
    JSON **newelems = realloc(json->arr.elems, sizeof(JSON*)*(json->arr.count));
    if(!newelems){
        fprintf(stderr, "add_value failed,realloc failed");
        return -1;
    }
    json->arr.elems = newelems;
    json->arr.elems[json->arr.count-1] = val;
    return 0;
}

int json_arr_del_value(JSON *json, JSON* val)
{
    assert(json);
    assert(val);
    if(json->type != JSON_ARR){
        fprintf(stderr, "del_value failed,son is not JSON_ARR");
        return -1;
    }
    int index = 0;
    int count = json->arr.count;
    if(count <= 1){
        fprintf(stderr, "del_value failed,arr count <1");
        return -1;
    }
    for(int index = 0; index < count; index++){
        if(json->arr.elems[index] == val){
           break;
        }    
    }
    if(index == count){
        fprintf(stderr, "del_value failed,can not find the value");
        return -1;
    }
    json_free(json->arr.elems[index]);
    for(int i = index; i < count-1; i++){
        json->arr.elems[i] = json->arr.elems[i+1];
    }
    json->arr.elems[count-1] = NULL;
    json->arr.count--;
    return 0;    
}




/**
 * 新建一个BOOL类型的JSON值
 */
JSON *json_new_bool(BOOL val)
{
    //TODO:
    JSON *json = json_new(JSON_BOL);
    if (!json)
        return json;
	json->bol = val;
    return json;
}
/**
 * 新建一个数字类型的JSON值
 */
JSON *json_new_num(double val)
{
    //TODO:
	JSON *json = json_new(JSON_NUM);
    if (!json)
        return json;
	json->num = val;
    return json;
}
/**
 * 新建一个字符串类型的JSON值
 */
JSON *json_new_str(const char *str)
{
    JSON *json = json_new(JSON_STR);
    if (!json)
        return json;
    json->str = strdup(str);
    if (!json->str) {
        fprintf(stderr, "strdup(%s) failed", str);
        json_free(json);
        return NULL;
    }
    return json;
}


/**
 * 获取名字为key，类型为expect_type的子节点（JSON值）
 * @param json 对象类型的JSON值
 * @param key   键名
 * @param expect_type 期望类型
 * @return 找到的JSON值
 */
static const JSON *get_child(const JSON *json, const char *key, json_e expect_type)
{
    const JSON *child;
    assert(json);
    assert(json_type(json) == JSON_OBJ);
    assert(key);

    child = json_get_member(json, key);
    if (!child)
        return NULL;
    if (child->type != expect_type)
        return NULL;
    return child;
}
/**
 * 获取JSON对象中键名为key的数值，如果获取不到，或者类型不对，返回def
 */
double json_obj_get_num(const JSON *json, const char *key, double def)
{
    const JSON * child = get_child(json, key, JSON_NUM);
    if (!child)
        return def;
    return child->num;
}
/**
 * 获取JSON对象中键名为key的BOOL值，如果获取不到，或者类型不对，返回false
 */
BOOL json_obj_get_bool(const JSON *json, const char *key)
{
    const JSON * child = get_child(json, key, JSON_BOL);
    if (!child)
        return FALSE;
    return child->bol;
}
/**
 * 获取JSON对象中键名为key的值，如果获取不到，则返回缺省值def
 * @param json 对象类型的JSON值
 * @param key  键名
 * @param def  找不到时返回的缺省值
 * @return 获取到的字符串结果
 * @details
 * 如果json不是对象类型，则返回def
 * 如果对应的值为其它类型，先将其转换为字符串
 * 如: 
 *  json: {"key": "str"}
 *  json_obj_get_str(json, "key", NULL) = "str"
 *  json_obj_get_str(json, "noexist", NULL) = NULL
 *  json_obj_get_str(json, "noexist", "") = ""
 *  
 */
const char *json_obj_get_str(const JSON *json, const char *key, const char *def)
{
    const JSON * child = get_child(json, key, JSON_STR);
    if (!child)
        return def;
    return child->str;    
}

int json_print_object(object *obj, const char* fname,int tab);
int json_print_array(array *arr, const char* fname, int tab);
int json_print_keyvalue(keyvalue *kvs, const char* fname, int tab);

int json_print_value(JSON *json, const char* fname,int tab)
{
	if(!json){
		goto failed_;
	}
    FILE *fp;
    fp = fopen(fname, "a+");
    if (!fp) {
        fprintf(stderr, "open file [%s] failed, errno: %d\n"
            , fname, errno);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
	switch(json->type)
	{
		case JSON_BOL:
			if(json->bol)
			{
                fprintf(fp,"true");
			}
			else
			{
                fprintf(fp,"false");
			}
			break;
		case JSON_STR:
            fprintf(fp,"%s",json->str);
			break;
		case JSON_NUM:
			fprintf(fp,"%f",json->num);
			break;
		case JSON_OBJ:
            fprintf(fp,"{");
            fprintf(fp,"%*s",2*tab,"");
            fclose(fp);
			json_print_object(&(json->obj),fname, tab+1);
            fp = fopen(fname, "a+");
            fprintf(fp,"\n");
            fprintf(fp,"%*s}",2*tab,"");
			break;
		case JSON_ARR:
            fclose(fp);
			json_print_array(&(json->arr), fname, tab+1);
            fp = fopen(fname, "a+");
			break;
		case JSON_NONE:
			break;
		
	}
    fclose(fp);
	return 0;
failed_:
	return -1;
}
int json_print_object(object *obj, const char* fname,int tab)
{
	if(!obj)
	{
		goto failed_;	
	}
    FILE *fp;
    fp = fopen(fname, "a+");
    if (!fp) {
        fprintf(stderr, "open file [%s] failed, errno: %d\n"
            , fname, errno);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
	int count=obj->count;
    fclose(fp);
	for(int i=0;i<count;i++)
	{
        json_print_keyvalue(&(obj->kvs[i]), fname,tab+1);
	}
    fp = fopen(fname, "a+");
    fseek(fp, 0, SEEK_END);
    fclose(fp);
    fp = NULL;
	return 0;
failed_:
	return -1;
}
int json_print_array(array *arr, const char* fname,int tab)
{
	if(!arr)
	{
		goto failed_;	
	}
    FILE *fp;
    fp = fopen(fname, "a+");
    fseek(fp, 0, SEEK_END);
    fprintf(fp,"[");
	for(int i=0;i<arr->count;i++)
	{
		value* tmp=arr->elems[i];
        fclose(fp);
        json_print_value(tmp, fname, tab+1);
        fp = fopen(fname, "a+");
        fseek(fp, 0, SEEK_END);
        fprintf(fp,",");
	}
    fprintf(fp,"]");
    fclose(fp);
	return 0;
failed_:
	return -1;
	
}
int json_print_keyvalue(keyvalue *kvs, const char* fname, int tab)
{
    FILE *fp;
    fp = fopen(fname, "a+");
    if (!fp) {
        fprintf(stderr, "open file [%s] failed, errno: %d\n"
        , fname, errno);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
	if(!kvs)
	{
		goto failed_;	
	}
    fprintf(fp,"\n");
    fprintf(fp,"%*s",2*tab,"");
    fprintf(fp,"%s:",kvs->key);
    fclose(fp);
    json_print_value(kvs->val, fname, tab);
	return 0;
failed_:
	return -1;
}
