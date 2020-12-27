## What is a pointer
### Introduction
在C/C++语言中，变量被声明后会被储存在内存中，每一个变量都有一个内存位置，
每一个变量都定义了可使用连字符（&）运算符访问的其内存块的首地址，
它表示了在内存中的一个地址。
而指针则用来处理内存地址相关的问题。

**指针(pointer)** 是一个变量，其值为另一个变量的地址。
即指针储存的是一个内存位置的直接地址。

理解指针需要掌握4个指针的基本概念：
指针的类型、指针所指向的类型、指针的值，以及指针所指向的值。
为方便理解，这里列举几个声明指针的例子：
```cpp
/* *** Example 1 *** */
int *ptr;  
char *ptr;  
int **ptr;  
int (*ptr)[3];  
int (*ptr)(int);
int *(*ptr)[4];
```

### 指针的类型
从语法角度看，只要把指针声明语句中的指针名字去掉，剩下的部分就是对应指针的类型。
例一中各个指针的类型如下：
```cpp
int *ptr;        // 指针的类型是int *       (整型变量的指针)
char *ptr;       // 指针的类型是char *
int **ptr;       // 指针的类型是int **
int (*ptr)[3];   // 指针的类型是int(*)[3]   (整形变量的指针的数组)
int (*ptr)(int); // 指针的类型是int(*)(int) (整形变量的指针的数组)
int *(*ptr)[4];  // 指针的类型是int *(*)[4] (整形变量的指针的指针的数组)
```

### 指针所指向的类型
当通过指针来访问指针所指向的内存区时，
指针所指向的类型决定了编译器将那块内存区里的内容当什么看待。
从语法上看，只需要把指针声明语句中的指针名字和名字左边的指针声明符\*去掉，
剩下的就是指针所指向的类型。例如：
```cpp
int *ptr;        // 指针所指向的类型是int
char *ptr;       // 指针所指向的类型是char
int **ptr;       // 指针所指向的类型是int *
int (*ptr)[3];   // 指针所指向的类型是int()[3]
int (*ptr)(int); // 指针所指向的类型是int()(int)
int *(*ptr)[4];  // 指针所指向的类型是int *()[4]
```
在指针的算术运算中，指针所指向的类型有很大的作用。

### 指针的值
指针的值是指针本身储存的数值，是一个代表内存地址长度的十六进制数，
这个值被编译器当作一个地址。
因此指针变量自身占据的内存区大小即为这个地址变量占据的大小。
在32位程序里，所有类型的指针的值都是一个32位整数，指针本身占据4个字节的长度。
**指针本身占据的内存大小常用来判断指针表达式是否是左值**。

### 指针所指向的值
指针变量指向的值即为指针所指向的内存区中储存的值。
指针所指向的内存区就是从指针的值所代表的那个内存地址开始，
长度为sizeof(指针所指向的类型)的一片内存区。
我们说一个指针的值是XX，就相当于说该指针指向了以XX为首地址的一片内存区域；
我们说一个指针指向了某块内存区域，就相当于说该指针的值是这块内存区域的首地址。


## Howtos
指针的使用方法无非就是：声明指针变量、指针变量的赋值、访问指针变量中地址的值，
以及指针的算术运算。

### Declaration
首先，就像C++中其他变量或常量一样，必须在使用指针前对其进行声明。
声明指针变量的一般方式为：
```cpp
typename *pointer_name;
```
其中，`typename`是指针的基类型，或者说是指针储存地址对应的变量的类型，
它必须是一个有效的C++数据类型；
`pointer_name`是指针变量的名称，在这个语句中星号\*是用来指定一个变量是指针。

所有指针的值的数据类型都是一样的，均为一个代表内存地址长度的十六进制数。
声明指针时用到的`typename`只是代表指针指向的变量的数据类型。
指针的原理可以理解为：
指针指向一个变量的起始地址；
指针类型(typename)用于区别这段地址的变量类型，
即告诉编译器该如何从这个起始地址开始读取内存，该读一个什么样的内存块。

### Operation
指针可以加上或减去一个整数。
指针的这种运算的意义和通常的数值的加减运算的意义是不一样的。例如：
```cpp
/* *** Example 2 *** */
char a[20];
int *ptr=a;
// Something trivial
...
ptr++;
```
在上例中，指针ptr的类型是int \*,它指向的类型是int，它被初始化为指向整形变量a。
接下来的`ptr++`命令中，指针ptr被加了1，编译器是这样处理的：
它把指针ptr的值加上了sizeof(int)，在32位程序中是被加上了4。
由于地址是用字节做单位的，
故ptr所指向的地址由原来的变量a的地址向高地址方向增加了4个字节。
而由于char类型的长度是一个字节，
所以原来ptr是指向数组a的第0号单元开始的四个字节，
此时指向了数组a中从第4号单元开始的四个字节。

基于这个思想，我们可以用一个指针和一个循环来遍历一个数组，如：
```cpp
/* *** Example 3 *** */
char a[20];
// 强制类型转换并不会改变a的类型
int *ptr=(int *)a;
// Initialization
...
for (i=0; i<20; i++) {  
  // Some operation
  ptr++；  
}
```
这个例子将整型数组中各个单元的值加1。
由于每次循环都将指针ptr加1，所以每次循环都能访问数组的下一个单元。

## Reference
1. [让你不再害怕指针——C指针详解(经典,非常详细)](https://blog.csdn.net/soonfly/article/details/51131141?utm_medium=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromMachineLearnPai2-1.channel_param&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromMachineLearnPai2-1.channel_param)
1. [C++指针|菜鸟教程](https://www.runoob.com/cplusplus/cpp-pointers.html)
1. [C++Pointer指针总结(一)](https://zhuanlan.zhihu.com/p/146989763)
1. [Pointer -- C++ Tutorials](http://www.cplusplus.com/doc/tutorial/pointers/)
1. [C++指针详解](https://www.cnblogs.com/ggjucheng/archive/2011/12/13/2286391.html)
1. [C++ 指针常见用法小结](https://blog.csdn.net/valada/article/details/79909749)
1. [C++中的指针用法汇集](https://zhuanlan.zhihu.com/p/101172033)

