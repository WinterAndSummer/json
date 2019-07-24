#include "json.h"
#include "xtest.h"
#include <stdio.h>

//  完成使用场景的测试
TEST(test, scene)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    JSON *basic = json_new(JSON_OBJ);
    ASSERT_TRUE(basic != NULL);

    EXPECT_EQ(0, json_obj_set_value(json, "basic", basic));

    EXPECT_EQ(0, json_obj_set_value(basic, "enable", json_new_bool(TRUE)));
    EXPECT_EQ(TRUE, json_obj_get_bool(basic, "enable"));

    EXPECT_EQ(0, json_obj_set_value(basic, "port", json_new_num(389)));
    EXPECT_EQ(389, json_obj_get_num(basic, "port", 0));
    
    EXPECT_EQ(0, json_obj_set_value(basic, "timeout", json_new_num(10)));
    EXPECT_EQ(10, json_obj_get_num(basic, "timeout", 0));

    EXPECT_EQ(0, json_obj_set_value(basic, "basedn", json_new_str("aaa")));

    EXPECT_EQ(0, json_obj_set_value(basic, "fd", json_new_num(-1)));
    EXPECT_EQ(0, json_obj_set_value(basic, "maxcnt", json_new_num(133333333333)));

    
    JSON *dns = json_new(JSON_ARR);
    JSON *dns0 = json_new_str("200.200.0.1");
    JSON *dns1 = json_new_str("200.0.0.254");
    json_arr_add_value(dns, dns0);
    json_arr_add_value(dns, dns1);
  
	EXPECT_EQ(0, json_obj_set_value(basic, "dns", dns));
	EXPECT_EQ(dns1, json_get_element(dns,1));
    
    
    //advance
    JSON *advance = json_new(JSON_OBJ);
    //ASSERT_TRUE(advance != NULL);
    
     json_obj_set_value(json, "advance", advance);
    //EXPECT_EQ(0, json_obj_set_value(advance, "advance", advance));
    
    JSON *dnsa = json_new(JSON_ARR);
    JSON *dnsa0 = json_new(JSON_OBJ);
    JSON *dnsa1 = json_new(JSON_OBJ);
    json_obj_set_value(dnsa0, "name", json_new_str("huanan"));
    json_obj_set_value(dnsa0, "ip", json_new_str("200.200.0.1"));

    json_obj_set_value(dnsa0, "name", json_new_str("huabei"));
    json_obj_set_value(dnsa0, "ip", json_new_str("200.0.0.254"));
    
    json_arr_add_value(dnsa, dnsa0);
    json_arr_add_value(dnsa, dnsa1);
    json_obj_set_value(advance, "dns", dns);
    
    JSON *portpool = json_new(JSON_ARR);
    JSON *portpool0 = json_new_num(130);
    JSON *portpool1 = json_new_num(131);
    JSON *portpool2 = json_new_num(132);
    json_arr_add_value(portpool, portpool0);
    json_arr_add_value(portpool, portpool1);
    json_arr_add_value(portpool, portpool2);
    
    json_obj_set_value(advance, "portpool", portpool);
    
    json_obj_set_value(advance, "url", json_new_str("http://200.200.0.4/main"));
    
    json_obj_set_value(advance, "url", json_new_str("/etc/sinfors"));
    
    json_obj_set_value(advance, "url", json_new_num(3.14));
    
    
    
    json_obj_set_value(basic, "enable", json_new_bool(TRUE));
    //EXPECT_EQ(0, json_obj_set_value(advance, "dns", json_new_bool(TRUE)));
    
    
    //JSON *test = json_new_str("200.200.0.1");
	json_print_value(json, "/mnt/hgfs/workspace/json-data-structure/result.yaml",0);
    json_arr_del_value(dns, dns0);
    json_arr_del_value(dns, dns1);
	
    json_obj_del_value(basic, "port");
//...
    EXPECT_EQ(0, json_obj_set_value(basic, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(basic, "ip", NULL);
    ASSERT_STRCASEEQ("200.200.3.61", ip);
    
    
    
	
    json_free(json);
}

//  测试键值对存在的情况
TEST(json_obj_get_str, exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);

    EXPECT_EQ(0, json_obj_set_value(json, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(json, "ip", NULL);
    ASSERT_TRUE(ip != NULL);
    ASSERT_STRCASEEQ("200.200.3.61", ip);
	
    json_free(json);
}

//  测试键值对不存在的情况
TEST(json_obj_get_str, notexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);

    EXPECT_EQ(0, json_obj_set_value(json, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(json, "ip2", NULL);
    ASSERT_TRUE(ip == NULL);

    ip = json_obj_get_str(json, "ip3", "default");
    ASSERT_TRUE(ip != NULL);
    ASSERT_STRCASEEQ("default", ip);

    json_free(json);
}

int main(int argc, char **argv)
{
	return xtest_start_test(argc, argv);
}


//---------------------------------------------------------------------------
//  方案1
//---------------------------------------------------------------------------
#if 0
int main(int argc, char *argv[])
{
    int ret = 0;
    JSON *json = json_new(JSON_OBJ);
    if (!json)
        goto failed_;
    JSON *basic = json_new(JSON_OBJ);
    if (!basic) {
        json_free(json);
        goto failed_;
    }

    json_obj_set_value(json, "basic", basic);
    json_obj_set_value(basic, "enable", json_new_bool(TRUE));
    json_obj_set_value(basic, "port", json_new_num(389));
	
//...
    ret = json_obj_set_value(basic, "ip",json_new_str("200.200.3.61"));	
	JSON **dnsp=(JSON**)malloc(sizeof(JSON*)*2);
	dnsp[0]=json_new_str("200.200.0.1");
	dnsp[1]=json_new_str("200.0.0.254");
	JSON* dns=json_new_arr(dnsp,2);
	json_get_element(dns,1);
	json_get_member(basic, "ip");
	json_obj_get_num(json,"port",1.0);
	json_obj_get_bool(json, "enable");




    if (ret != 0)
        goto failed_;
//...
    /* const char *ip = json_obj_get_str(basic, "ip", NULL);
    if (!ip)
        goto failed_; */
	json_print_value(json);
//...
	json_free(json);
    return 0;
failed_:
    json_free(json);
    return 1;
}
#endif
