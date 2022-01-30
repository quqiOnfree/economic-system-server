### 本项目开发原因
- 没事干，想试试与别人做一个`mc`+`c++`插件

### 本项目开夫注意事项
- 本项目是结合mc服务端插件一起用的，不能单独使用，如果硬要单独使用也可以

### 本项目可以单独用作库的文件
- [head_msg.hpp](./learning_7_socket/head_msg.hpp)这个可以方便地制作消息头
- [log_system.hpp](./learning_7_socket/log_system.hpp)这个是log系统（可能会很难用）
- [inicpp.hpp](./learning_7_socket/inicpp.hpp)这个是ini写入和读取系统（可能会很难用）

### 本项目使用方法
- 先编译
- 执行exe文件
- 填写config中的data文件
```
{
   "server_address" : "127.0.0.1",   //设置服务端的ip地址
   "server_msg_head_mode" : false,   //是否打开消息头模式，默认是开的
   "server_port" : 1234,             //设置服务端的端口
   "server_thread_number" : 102400,  //设置最大线程数
   "server_whitelist" : ["127.0.0.1"]//设置白名单（以后会增加开启和关闭白名单）
}
```
- 填写完毕之后，重新打开exe文件
- 服务端可以执行用户输入的指令，具体指令请输入`help`回车查看
- 用网络传输`json格式的数据`方法传输数据给服务端(`编码格式：gb2312`)
```
//注意事项：数值要转换为字符串，不然服务端不会认

{"get_version":{}}
//获取版本

{"creat_new_money":{"name":"创建的货币名","value":"基准数值(int/double)","creator":"创建者"}}
//添加货币

{"creat_new_user":{"name":"创建的用户名","creator":"创建者","uuid":"用户名的uuid（可以不用写）"}}
//添加用户

{"delete_money":{"name":"删除的货币名"}}
//删除货币

{"change_user_money":{"name":"用户名","money_name":"货币名","add":"增加的数值(int/double)"}}
{"change_user_money":{"name":"用户名","money_name":"货币名","set":"设置的数值(int/double)"}}
{"change_user_money":{"name":"用户名","money_name":"货币名","remove":"减少的数值(int/double)"}}
//改变用户货币

{"get_user_info":{"name":"用户名"}}
获取用户货币

{"get_exchange_rate":{"money_name1":"货币1","money_name2":"货币2"}}
//获取汇率
```
