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
- 用网络传输方法传输数据给服务端
```
{"get_version":{}}
//获取版本

{"creat_new_money":{"name":"123123","value":10,"creator":"quqi"}}
//添加货币

{"creat_new_user":{"name":"quqi","uuid":"asdasdasd"}}
//添加用户

{"delete_money":{"name":"123123"}}
//删除货币

{"change_user_money":{"name":"quqi","money_name":"123123","add":1}}
{"change_user_money":{"name":"quqi","money_name":"123123","set":1000}}
{"change_user_money":{"name":"quqi","money_name":"123123","remove":1000}}
//改变用户货币

{"get_user_money":{"name":"quqi"}}
获取用户货币

{"get_exchange_rate":{"money_name1":"2p","money_name2":"1p"}}
//获取汇率
```
