
1. 运行source ./build.sh，生成可执行脚本；
2. 在当前Shell中添加环境变量；


1. ln -s /dev/ttyUSB0 /dev/ttyS110
2. gcc test.cpp -I ../include -L ../lib/ -l:MotorMaster.Sdk.so -lpthread -o test
3. export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib


## Error:
#### Segmentation fault:
可能原因:
1. 内存越界, 数组越界;
2. 不存在的内存;
3. 未使用sudo, 无法访问指定内存;

#### Error while load shared libraries: MotorMaster.Sdk.so:
可能原因:
1. CMakeList.txt中链接库命名格式有误;
2. 链接库路径有误;

