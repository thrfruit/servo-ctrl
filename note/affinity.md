# Linux多线程程序为线程分配CPU
> Keywords: Linux, 进程(线程), CPU亲和性

## 背景介绍
### 实验程序简述
基于实验室已有的UR机器人伺服程序框架([UR-RT](http://www.git-hitsz.cn/jiang/ur-rt))
编写多线程程序，如果同时开启多个重量级线程，可能会出现子线程节拍混乱的问题。

在我的实验中，我编写了RM夹爪的伺服控制程序([rmctrl](http://www.git-hitsz.cn/lazyshawn/rmctrl))，
通过该程序控制一个RM夹爪、一个Realsense相机和一个读取压力传感器的数据采集卡。
该程序共三个线程，其中主线程为伺服线程，用于读取采集卡、控制夹爪，实现力控，
线程周期10ms；
一个子线程为交互线程，处理用户命令和控制程序运行状态等；
另一个子线程为图像处理线程，与Realsense通信，实时获取相机图片，
然后计算夹爪施加给物体的期望压力信号，线程周期60ms。

### 实验现象
在这三个进程中主线程和图像处理线程任务量重，
在系统中的调度可能会导致CPU伪过载，程序运行效率过低，而且子线程的周期不稳定。
使用命令行指令`top`查看发现其中一颗CPU占用率100%，
疑似单核运行导致。
实验中的具体表现如下图所示：
![uncoordinated](https://raw.githubusercontent.com/lazyshawn/servo-ctrl/dev/note/pics/uncoordinated.png)

其中黄色实线代表伺服周期中采集的传感器压力值，其变化频率即为伺服周期的频率；
蓝色虚线代表图像处理线程中计算的期望压力值，其变化频率代表图像处理周期的频率。
左图所示为问题解决之后的正常情况，
右图为处理之前的状态，从中可以明显看出图像处理周期不稳定，在2~3.5s左右尤为明显。

将两个线程分配在同一个CPU后，再查看CPU运行状态发现占用率只有20%左右；
而分配在不同CPU后则出现节拍混乱的问题。
这一现象的原因可能是由于子线程在其他处理器时读写数据耗时增加，
而主线程高频读写，导致子线程取不到数据。
具体原因在此不做深究，以介绍为线程分配CPU的方法为主。


## CPU的亲和性(Affinity)
CPU的亲和性就是绑定某一线程(进程)到特定的CPU集上，
从而使得线程在运行时不被迁移到其他处理器，也成为CPU关联性。
将线程与特定的CPU绑定后，可以人为分配任务给各个CPU核心，
避免单个进程下的多个子线程被系统分配给同一CPU处理而造成CPU负载不均，
或者子线程过于分散等问题。

这一亲和性分为软亲和性和硬亲和性两种。
Linux内核进程调度器自带软亲和性的特性，
会倾向于保持一个进程在同一个处理器上运行，而不频繁的在多个CPU之间迁移。
通常情况下，调度器会根据各个CPU的负载情况调度运行中的线程，
以减轻CPU的压力，提高所有进程的整体性能。
硬亲和性是利用Linux内核提供给用户的API，强行将进程绑定到指定的CPU运行，
从而满足特殊的性能需求。


### 原理介绍
在Linux系统内核中，所有的进程都有一个`task_struct`结构体，
其中一个元素名为`cpus_allowed`的位掩码。
这个位掩码由n位组成，与系统中的n个逻辑处理器一一对应。
即4核处理器分别对应4位，如果4核都启用了超线程那么这个位掩码就有8位。
如果一个线程的`cpus_allowed`位掩码在若干个位上置位(1)，
那么该线程就能在对应的处理器之间迁移。


### 相关的函数原型
绑定线程和进程时使用的函数分别在各自的头文件中(pthread.h与sched.h)，
函数的使用方法一致，只是函数名不同，下面仅以线程的处理为例。

* 设置亲和度之前，需要使用以下宏函数指定CPU集。
```cpp
// 这个宏对 CPU 集 set 进行初始化，将其设置为空集
void CPU_ZERO (cpu_set_t *set)
// 这个宏将 指定的 cpu 加入CPU集set 中
void CPU_SET (int cpu, cpu_set_t *set)
// 这个宏将 指定的 cpu 从 CPU 集 set 中删除
void CPU_CLR (int cpu, cpu_set_t *set)
// 如果 cpu 是 set 集的一员，这个宏就返回一个非零值(true),否则就返回零(false)
int CPU_ISSET (int cpu, const cpu_set_t *set)
```
多个加入和删除命令需要依次操作，如添加0、1号CPU进入set集的命令为：
```cpp
CPU_SET(0, *set);
CPU_SET(1, *set);
```

* 与设置线程和CPU亲和度相关的函数有两个，分别用于设定和读取线程可运行的CPU集。
其函数原型如下：
```cpp
NAME
  pthread_setaffinity_np, pthread_getaffinity_np - set/get CPU affinity of a thread.
 
SYNOPSIS
  #define _GNU_SOURCE
  #include <pthread.h>

  int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);
  int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *cpuset);

  Compile and link with -pthread.
 
RETURN VALUE
  On success, these functions return 0; on error, they return a nonzero error number
```
其中函数`pthread_setaffinity_np`的功能是
设置`thread`线程运行在`cpuset`所设定的CPU集上。
第一个参数`thread`可以在线程内通过调用函数`pthread_self()`获得，
如果值为0则表示当前线程。
第二个参数`cpusetsize`是`cpuset`所指定的数的长度，
通常设为`sizeof(cpu_set_t cpuset)`。
如果当前指定的线程没有运行在给定的CPU集上，
那么该线程会从其他CPU上迁移到CPU集中的一个处理器上运行。

函数`pthread_getaffinity_np`可以获得指定线程的CPU位掩码，
并返回到cpuset所指向的结构体中，即获得指定线程可以运行在那些CPU上。
同样`thread`值为0时，也表示当前线程。


### 示例程序
下面是一个具体的例子，在主线程中创建两个子线程，然后分别将子线程绑定到1、2号CPU上。
```cpp
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

// 获取线程pid
pid_t gettid() { return syscall(SYS_gettid); }

// 线程函数
void *start_routine(void *arg) {
  int thn = *((int *)arg);

  pid_t pid = gettid(); // LWP(lightweight process)
  pthread_t tid = pthread_self();

  // 查看CPU个数
  long cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
  if (cpu_num <= 0) {
    printf("thn=%d,pid=%d,tid=%lu,get cpu info error:%ld\n", thn, pid, tid, cpu_num);
    return ((void*) 0);
  }

  // 设置CPU集
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);
  CPU_SET(thn % cpu_num, &cpu_set);

  // 绑定线程与CPU
  int res = pthread_setaffinity_np(tid, sizeof(cpu_set), &cpu_set);
  if (res != 0) {
    printf("thn=%d,pid=%d,tid=%lu,pthread_setaffinity_np error:%d(%s)\n",
        thn, pid, tid, res, strerror(res));
    return ((void*) 0);
  }

  // 获取子线程可运行的CPU集
  res = pthread_getaffinity_np(pthread_self(), sizeof(cpu_set), &cpu_set);
  int i = 0;
  for (; i < sizeof(cpu_set); i++) {
    if (CPU_ISSET(i, &cpu_set)) {
      printf("thread_%d: pid=%d,tid=%lu,bind cpu[%d]\n", thn, pid, tid, i);
    }
  }

  sleep(1);
  return ((void*) 0);
}

int main() {
  int i0 = 0, i1 = 1;
  pthread_t tid0, tid1;

  // 开启两个线程
  pthread_create(&tid0, NULL, start_routine, &i0);
  pthread_create(&tid1, NULL, start_routine, &i1);

  // 申明CPU集并初始化，将其设置为空集
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);

  // 获取主线程可运行的CPU集
  int res = pthread_getaffinity_np(pthread_self(), sizeof(cpu_set), &cpu_set);
  if (res != 0) {
    printf("main: pid=%d,tid=%lu,pthread_getaffinity_nperror:%d(%s)\n",
        getpid(), pthread_self(), res, strerror(res));
    return 1;
  }

  // 判断主线程在哪些CPU上运行 
  int i = 0;
  for (; i < sizeof(cpu_set); i++) {
    if (CPU_ISSET(i, &cpu_set)) {
      printf("main: pid=%d,tid=%lu,bind cpu[%d]\n", getpid(), pthread_self(), i);
    }
  }

  sleep(2);
}
```
使用g++命令编译`g++ -std=c++11 ctest.cpp -lpthread -o ctest`，运行结果如下。
其中主函数的CPU集个数为默认状态，即为你电脑的CPU核数，
子线程的CPU集即为程序中分配的CPU集。
```bash
main: pid=61035,tid=139775635879744,bind cpu[0]
main: pid=61035,tid=139775635879744,bind cpu[1]
main: pid=61035,tid=139775635879744,bind cpu[2]
main: pid=61035,tid=139775635879744,bind cpu[3]
thread_0: pid=61036,tid=139775635875584,bind cpu[0]
thread_1: pid=61037,tid=139775627482880,bind cpu[1]
```

### 查看线程运行时所在的CPU
Linux系统提供了查看线程运行在哪个CPU核心上的命令行指令。
包括但不限与如下几条：
1. `top -Hp $pid`: top命令可以查看CPU状态，用户界面操作比较复杂。
2. `taskset -pc $pid`: 获取某线程与CPU核心的亲和性。
3. `ps -eLF`: 返回结果中的PSR字段即对应了线程所在的CPU序号。
在程序(如上述`ctest`)运行时，
可以通过`ps -eLo pid,lwp,args,psr | grep ctest`指令筛选包含`ctest`的命令的对应字段，
其中psr即为运行时所在的CPU序号。


## 参考资料
1. [Linux进程、线程与CPU的亲和性](https://www.cnblogs.com/wenqiang/p/6049978.html)
1. [Linux：获取、设置进程（线程）的CPU亲和性](https://blog.csdn.net/test1280/article/details/88206560)
1. [C++性能榨汁机之CPU亲和性](https://zhuanlan.zhihu.com/p/57470627)
1. [c++11 thread 线程绑定CPU方法](https://blog.csdn.net/zfjBIT/article/details/105846212)
1. [pthread与std::thread对比用法](https://blog.csdn.net/matrixyy/article/details/50929149)
1. [Linux内核初探：进程与线程](https://zhuanlan.zhihu.com/p/93553600)
1. [Linux:查看线程运行于哪个CPU核心上](https://blog.csdn.net/test1280/article/details/87993669)

