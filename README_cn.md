# WorkThread

[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-c++11-red.svg)](https://en.cppreference.com/)

工作线程类，以独立的线程接收委派的任务。

它是从[cpp-tbox](https://gitee.com/cpp-master/cpp-tbox)项目中提取出来的一个非常好用的模块，剔除与cpp-tbox其它模块相关的功能所得，可独立使用。  
对应[cpp-tbox](https://gitee.com/cpp-master/cpp-tbox)的源码：[work_thread.h](https://gitee.com/cpp-master/cpp-tbox/blob/master/modules/eventx/work_thread.h)，[work_thread.cpp](https://gitee.com/cpp-master/cpp-tbox/blob/master/modules/eventx/work_thread.cpp)

# 怎么集成？
将 src/ 下的 work_thread.cpp 与 work_thread.h 复制到您的工程中，与普通代码一起编译即可。

# 怎么调用？
```c++
#include "work_thread.h"
...
hevake::WorkThread worker;
...
worker.execute([]{ DoSomethingByWorker(); });
...
```
详见 [示例代码](src/main.cpp)

# 推荐
本模块只是[cpp-tbox](https://gitee.com/cpp-master/cpp-tbox)项目宝藏中的冰山一角。  
点此前往：[cpp-tbox](https://gitee.com/cpp-master/cpp-tbox)。建议关注收藏，点亮一个 Star。