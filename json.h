#ifndef JSON_H_
#define JSON_H_

/**
 *  想想：
 *  1. 你的JSON接口是为什么场景设计的？
 *  2. 这些场景有什么特点，会怎么影响的API设计风格？
 *  3. 你的用户怎么用这些API构建程序？
 *  4. 怎么设计API，会让用户用起来更爽？
 *  5. 怎么设计API，会让这些API更容易调试、测试？
 */

typedef enum json_e {
    JSON_NONE,
    JSON_BOL,
    JSON_STR,
    JSON_ARR,
    JSON_OBJ,
    JSON_NUM,
} json_e;

typedef unsigned int BOOL;
typedef unsigned int U32;
typedef struct value JSON;

#define TRUE 1
#define FALSE 0

/**
 *  @brief 从对象类型的json中获取名字为key的JSON值
 *  
 *  @param [in] json 对象类型的JSON值
 *  @param [in] key 键名
 *  @return 键值
 *  
 *  @details 要求json是个对象类型
 */
const JSON *json_get_member(const JSON *json, const char *key);
/**
 *  @brief 从数组类型的json中获取索引为idx的值
 *      
 *  @param [in] json 数组类型的JSON值 
 *  @param [in] idx 索引号
 *  @return 数组中第idx个位置的值
 *  
 *  @details 要求json是个数组
 */
const JSON *json_get_element(const JSON *json, U32 idx);

/**
 *  @brief 新建一个type类型的JSON值，采用缺省值初始化
 *  
 *  @param [in] type JSON值的类型，见json_e的定义
 *  @return 堆分配的JSON值
 *  
 *  @details 
 *  1. 对于数值，初始化为0
 *  2. 对于BOOL，初始化为FALSE
 *  3. 对于字符串，初始化为NULL
 *  4. 对于OBJ，初始化为空对象
 *  5. 对于ARR，初始化为空数组
 */
JSON *json_new(json_e type);
/**
 * 返回JSON值的类型
 * @param JSON值
 * @return json的实际类型
 */
json_e json_type(const JSON *json);

/**
 * 释放JSON值
 */
void json_free(JSON *json);
//-----------------------------------------------------------------------------
//  以下三种方案可任选一种，补充完善
//  也可自行设计其他方案实现
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  方案1
//-----------------------------------------------------------------------------
/**
 * 获取JSON对象中键名为key的数值，如果获取不到，或者类型不对，返回def
 */
double json_obj_get_num(const JSON *json, const char *key, double def);
/**
 * 获取JSON对象中键名为key的BOOL值，如果获取不到，或者类型不对，返回false
 */
BOOL json_obj_get_bool(const JSON *json, const char *key);
/**
 * 获取JSON对象中键名为key的值，如果获取不到，则返回缺省值def
 * 如果json不是对象类型，则返回def
 * 如果对应的值为其它类型，先将其转换为字符串
 * 如: 
 *  json: {"key": "str"}
 *  json_obj_get_str(json, "key", NULL) = "str"
 *  json_obj_get_str(json, "noexist", NULL) = NULL
 *  json_obj_get_str(json, "noexist", "") = ""
 *  
 *  想想：如果key对应的JSON值是个对象或数组，怎么办？
 */
const char *json_obj_get_str(const JSON *json, const char *key, const char *def);

double json_arr_get_num(const JSON *json, int idx, double def);
BOOL json_arr_get_bool(const JSON *json, int idx);
const char *json_arr_get_str(const JSON *json, int idx, const char *def); 

//  想想: 如果对应的键已经存在该怎么办，如果不存在该怎么办？要不要提供一个json_add_xxx版本的分别应对这两种场景？
int json_obj_set_num(JSON *json, const char *key, double val);
int json_obj_set_bool(JSON *json, const char *key, BOOL val);
int json_obj_set_str(JSON *json, const char *key, const char *val);
/**
 * 设置JSON对象的成员值
 * @param json 对象类型的JSON值
 * @param key 键名
 * @param val 值
 * @details
 * 如果这个成员之前不存在，则新建一个键值对，如果存在，则修改该键值对
 */
int json_obj_set_value(JSON *json, const char *key, JSON *val);

int json_arr_add_num(JSON *json, double val);
int json_arr_add_bool(JSON *json, BOOL val);
int json_arr_add_str(JSON *json, const char *val);
int json_arr_add_value(JSON *json, JSON *val);

/*
JSON *json = json_new(JSON_OBJ);
JSON *basic = json_new(JSON_OBJ);
JSON *dns = json_new(JSON_ARR);

json_obj_set_value(json, basic);
json_obj_set_bool(basic, "enable", true);
json_obj_set_str(basic, "ip", "200.200.3.2");
json_obj_set_value(basic, "dns", dns);

json_arr_add_str(dns, "200.200.3.254");
json_arr_add_str(dns, "200.200.1.1");

 */
//-----------------------------------------------------------------------------
//  方案2
//-----------------------------------------------------------------------------
double json_get_num(const JSON *json, double def);
BOOL json_get_bool(const JSON *json);
const char *json_get_str(const JSON *json, const char *def);

JSON *json_new_num(double val);
JSON *json_new_bool(BOOL val);
JSON *json_new_str(const char *str);

//const JSON *json_get_member(const JSON *json, const char *key);
//const JSON *json_get_element(const JSON *json, U32 idx);
//  想想：val应该是堆分配，还是栈分配？如果json_add_member失败，该由谁来释放val？

//  往对象类型的json中增加一个键值对，键名为key，值为val
JSON *json_add_member(JSON *json, const char *key, JSON *val);
//  往数组类型的json中追加一个元素
JSON *json_add_element(JSON *json, JSON *val);
/*
JSON *json = json_new(JSON_OBJ);
JSON *basic = json_new(JSON_OBJ);
JSON *dns = json_new(JSON_ARR);

json_add_member(json, "basic", basic);
json_add_member(json, "dns", dns);
json_add_element(dns, json_new_str("200.200.2.254"))；
json_add_element(dns, json_new_str("192.168.1.1"));
json_add_member(basic, "enable", json_new_bool(true));

 */
//-----------------------------------------------------------------------------
//  方案3
//-----------------------------------------------------------------------------
/**
 * 在JSON值json(对象类型)中找到路径为path的成员，将其值修改为val
 * @param json 对象类型的JSON值
 * @param path 待修改成员的路径，如：basic.dns[1]
 * @param val 新的值
 * @return <0表示失败，否则表示成功
 */
int json_set(JSON *json, const char *path, JSON *val);
/**
 * 在JSON值json(对象类型)中找到路径为path的成员
 * @param json 对象类型的JSON值
 * @param path 待修改成员的路径，如：basic.dns[1]
 * @return 路径path指示的成员值，不存在则返回NULL
 */
JSON *json_get(JSON *json, const char *path);

JSON *json_new_num(double val);
JSON *json_new_bool(BOOL val);
JSON *json_new_str(const char *str);

/*
JSON *json = json_new(JSON_OBJ);

json_set(json, "basic", json_new(JSON_OBJ));
json_set(json, "basic.enable", json_new_bool(true));
json_set(json, "basic.dns", json_new(JSON_ARR));
json_set(json, "basic.dns[0]", json_new_str("192.168.1.1"));
 */
//-----------------------------------------------------------------------------
//  TODO: 增加你认为还应该增加的接口
//-----------------------------------------------------------------------------
//
//1. 增加键值对；
//2. 增加数组元素
//3. 创建JSON
//4. 释放JSON
//...


#endif

