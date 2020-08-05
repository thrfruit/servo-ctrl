## 程序记录
1. 伺服周期：约6ms。发送位置信号、读压力传感器数据、pid控制器计算夹具期望位置。
2. 图像处理周期：约50ms。采集图像、计算物体位置、根据自适应控制方程计算期望压力。


## 或许有用的命令
1. **top:** 查看CPU占用率，该占用率是按CPU总使用率来显示的，即4核最高可达400%。

## 二阶临界阻尼系统
1. 响应函数
```shell
c(t) = 1-e^(-wn*t)*(1+wn*t)
# 其中，wn为系统固有频率
```

## Linux多线程编程
### 进程、线程和调度
1. **[进程和线程-廖雪峰的官方网站](https://www.liaoxuefeng.com/wiki/1016959663602400/1017627212385376)** </br>
对于操作系统来说，一个任务就是一个进程（Process），
比如打开一个Word就启动了一个Word进程，进程是资源分配的最小单位。
有些进程还不止同时干一件事，比如Word，它可以同时进行打字、拼写检查、打印等事情。
在一个进程内部，要同时干多件事，就需要同时运行多个“子任务”，
我们把进程内的这些“子任务”称为线程（Thread），线程是最小的执行单元。

Linux系统对于线程实现非常特殊，他并不区分线程和进程，
线程只是一种特殊的进程(LWP, Lightweight Pthread)，
只是该进程与父进程共享一些资源而已。
进程(线程)的标识有三种，分别是进程pid、线程tid和线程pid。
Linux每一个进程有一个pid，类型是pid_t，由`getpid()`命令取得。
Linux下的每一个POSIX线程也有一个id，类型pthread_t，
由pthread_self()取得，该id由线程维护，其id空间是各进程间相互独立的，
即不同进程中的线程可能有相同的id(进程内唯一)。
在内核中，每一个进程都有独立id，类型pid_t，这是进程的真实pid(称为tid)，
只能通过Linux的系统调用syscall来获取，`syscall(SYS_gettid)`。



1. **调度算法** </br>
根据系统的资源分配策略所规定的资源分配算法，
（如任务A执行完后，选择哪个任务来执行），
使得某个因素（如进程的总执行时间，或者磁盘寻道时间等）最小。
对于不同的系统目标，通常采用不同的调度算法。

### 互斥量与条件变量
[参考链接](https://stackoverflow.com/questions/4544234/calling-pthread-cond-signal-without-locking-mutex)

等待条件变量命令pthread_cond_wait() 
和通知等待条件变量的线程命令pthread_cond_signal()
中传递mutex参数是为了在线程阻塞前把mutex锁释放，在线程被唤醒时重新加锁。
这样可以在线程被阻塞时，让别的线程可修改临界区，可以防止死锁和唤醒丢失等问题。
```cpp
mutex.lock();
// 一些其他程序
...

while (condition == FALSE) {  // 判断“条件”是否成立
    pthread_cond_wait()       // 等待
}

// 一些其他程序
...
mutext.unlock();
```

### 条件变量与while循环
1. 一般来说，wait肯定是在某个条件调用的，不是if就是while
2. 放在while里面，是防止出于waiting的对象被别的原因调用了唤醒方法，但是while里面的条件并没有满足（也可能当时满足了，但是由于别的线程操作后，又不满足了），就需要再次调用wait将其挂起。
3. 其实还有一点，就是while最好也被同步，这样不会导致错失信号。

### Linux单进程实现多核CPU线程分配
**参考资料** 
1. [Linux单进程如何实现多核CPU线程分配](https://zhidao.baidu.com/question/419156063.html)
2. [linux进程、线程与cpu的亲和性(affinity)](https://www.cnblogs.com/wenqiang/p/6049978.html)
3. [https://blog.csdn.net/zfjBIT/article/details/105846212](https://blog.csdn.net/zfjBIT/article/details/105846212)
4. [pthread与std::thread对比用法](https://blog.csdn.net/matrixyy/article/details/50929149)



