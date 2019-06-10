# [Doudizhu Endgame](https://github.com/YunyanDeng/doudizhu_endgame) ![Language](https://img.shields.io/badge/Language-C%2B%2B11-ff69b4.svg) ![Python 3.6](https://img.shields.io/badge/python-3.6-blue.svg) ![License](https://img.shields.io/badge/license-GPL--3.0-blue.svg)

这是一个用于求解斗地主残局的项目。使用C++实现（也有一个简单的python实现），运用了负极大值算法(Negamax)并使用多线程和置换表来加速搜索。

- 实现的一些要点可以看[这里](https://blog.csdn.net/qq_24038061/article/details/91128402)

## 如何使用

```C++
#include <iostream>

#include "solution.h"

int main()
{
    using doudizhu_endgame::Solution;

    Solution solution;
    solution.start();

    return 0;
}
```

cmake直接构建

```
git clone https://github.com/YunyanDeng/doudizhu_endgame.git
mkdir doudizhu_endgame/build
cd doudizhu_endgame/build
cmake ../doudizhu/
make
```

## 其他

+ 有什么问题欢迎指正。

