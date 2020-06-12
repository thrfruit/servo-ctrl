
1. 运行source ./build.sh，生成可执行脚本；
2. 在当前Shell中添加环境变量；


* `ls -l /dev/tty*`
1. `ln -s /dev/ttyUSB0 /dev/ttyS110`
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

#### undefined reference to `std::thread::_State::~_State()@GLIBCXX_3.4.22'
可能原因:
1. 使用gcc编译出错, 改用g++即可;
```
set(CMAKE_CXX_COMPILER g++)    # 解决对'@GLIBCXX_3.4.20'未定义的引用的问题
```

#### libstdc++.so.6: version `GLIBCXX_3.4.22' not found
可能原因:
1. libstdc++库需要更新;
`strings /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep GLIBCXX`
可查看当前版本的libstdc++.so.6包含的GLIBCXX;
```
# 安装libstdc++
sudo apt-get install libstdc++6
# 若还未解决,则可终端运行如下命令
sudo add-apt-repository ppa:ubuntu-toolchain-r/test 
sudo apt-get update
sudo apt-get upgrade
sudo apt-get dist-upgrade
```

#### 在图像处理线程中读取全局结构体pSVO中的时间始终为0
可能原因:
1. 将复制结构体pSVO的命令放到图像处理函数之后(原因未明);

