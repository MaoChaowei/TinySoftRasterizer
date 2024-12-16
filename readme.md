## coding style
- 类名采用大驼峰，类的数据成员采用"\_"开头，用"\_"分割，类的成员函数用小驼峰命名。
- 以release版本构建和运行
  ```bash
    mkdir build
    cmake -B ./build -DCMAKE_BUILD_TYPE=Release
    cmake --build ./build --config Release
  ```