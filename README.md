[[中文]](README_CN.md)

# WorkThread

[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-c++11-red.svg)](https://en.cppreference.com/)

The worker thread class receives delegated tasks in an independent thread.

It is a very useful module extracted from the [cpp-tbox](https://github.com/cpp-main/cpp-tbox) project, which can be used independently after excluding functions related to other modules of cpp-tbox.  
The source code corresponding to [cpp-tbox](https://github.com/cpp-main/cpp-tbox)：[work_thread.h](https://github.com/cpp-main/cpp-tbox/blob/master/modules/eventx/work_thread.h)，[work_thread.cpp](https://github.com/cpp-main/cpp-tbox/blob/master/modules/eventx/work_thread.cpp)

# How to integrate it into your project?
Copy work_thread.cpp and work_thread.h under src/ to your project, and compile them together with normal code.

# How to use?
```c++
#include "work_thread.h"
...
hevake::WorkThread worker;
...
worker.execute([]{ DoSomethingByWorker(); });
...
```
See [Demo](src/main.cpp)

# Recommend
This module is just a little part of [cpp-tbox](https://github.com/cpp-main/cpp-tbox)。 There are many more useful modules out there.  
Click here to go to: [cpp-tbox](https://github.com/cpp-main/cpp-tbox)。  

# Encourage us
We would be happy if you could give us a star.
