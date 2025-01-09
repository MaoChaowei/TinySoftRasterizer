# 软光栅器和层次Z-Buffer的实现

# 1、项目概述

## 1.1 功能及代码结构概述

项目从底层实现了一个基础的软光栅器，支持扫描线转换、层次zbuffer等剔除算法，采用BVH算法挖掘景物空间的连贯性，具体功能包括：

- 借助GLFW和OpenGL后端，搭建了实时可交互的windows界面。界面支持用户自定义相关渲染设置，如：渲染算法、shader种类、场景文件、bvh加速结构的可视化、bvh细粒度定义，上述修改均支持实时更新与反馈；界面会实时反馈每一帧的渲染开销等相关重要信息。
- 实现渲染管线。采用类似OpenGL的状态机思想管理渲染管线的上下文，管线主要分为几何处理和光栅化阶段。其中几何处理阶段负责：MVP矩阵变换、背面剔除、视锥体裁剪算法，实现模型到屏幕空间稳定的映射；光栅化阶段根据遍历对象的顺序的不同，分为顺序遍历和基于BVH结构启发式遍历两种方式，不同的渲染算法对应交互界面中的Naive、Bvh_hzb、Easy_hzb、Scan_convert四种选项。
- 实现了obj模型文件和纹理数据的读取和管理，采用BVH双层加速结构管理场景文件。所谓双层加速的BVH结构，其中底层加速结构(BLAS)管理唯一的obj模型，同一BLAS可以实例化为不同的对象（Instance），而顶层加速结构（TLAS）管理所有实例对象的所有属性。该结构一方面实现同一obj模型的内存复用，另一方面避免BLAS的冗余构建，且便于实时交互时的Instance属性动态更新。
- 实现多种基础的fragment shader，可渲染深度图、法向图、blinn-phone、线框图等，采用类似g-buffer的方式渲染多光源图。
- 实现不同的剔除算法。本项目的层次zbuffer算法能利用BVH的启发式遍历顺序，有效挖掘景物空间的连贯性，从而快速拒绝大量被遮挡景物。BVH结构在世界空间构建一次，随后，实时渲染只需基于相机视角视角，维护屏幕空间的各个BVH结点的AABB盒。在光栅化阶段，总是选择屏幕空间z值更小的BVH节点作为优先遍历对象。

---

项目的结构如下所示：

```cpp
SOFTRASTERIZER
├── assets                    // 模型文件和图片等资产
│   └── model/ 
├── bin                       // 编译输出(after compilation)
│   └── ...
├── build                     // 工程文件等(after compilation)
│   └── ...
├── external                  // 第三方库
│   └── ...
├── src                       // 项目源代码
│   ├── common                // 公共结构、工具函数等
|   │   └── ...
│   ├── softrender            // 软光栅器的核心渲染文件
|   │   ├── clipper.cpp       // 裁剪算法
|   |   ├── interface.cpp     // 实现 game loop
|   |   ├── interface.h       // 渲染管线与窗口的交互信号
|   |   ├── render.cpp
|   |   ├── render.h          // 项目的顶层类，管理pipeline状态
|   |   ├── scanline.cpp  
|   |   ├── scanline.h        // 扫描线转换算法
|   |   ├── shader.cpp
|   |   └── shader.h          // Vertex shader和Fragment shader
│   ├── as.h                  // 加速结构的声明
│   ├── buffer.cpp            
│   ├── buffer.h              // 帧缓存和深度缓存
│   ├── bvhbuilder.cpp        
│   ├── bvhbuilder.h          // bvh构建的具体实现
│   ├── camera.cpp            
│   ├── camera.h              // 相机
│   ├── hzb.h                 // 层次zbuffer
│   ├── light.h               // 光源定义
│   ├── main.cpp
│   ├── material.h            // 材质定义
│   ├── object.cpp
│   ├── object.h              // 描述物体
│   ├── scene_loader.cpp      
│   ├── scene_loader.h        // 场景管理的顶层类
│   ├── scenedemo.cpp         // 场景demo数据
│   ├── texture.cpp
│   ├── texture.h             // 纹理定义
│   ├── vertex.h              // 顶点定义
│   ├── window.cpp
│   └── window.h              // 可视化窗口定义
├── CMakeLists.txt
├── readme.md
├── setup_debug.bat
└── setup_release.bat

```

## 1.2 交互说明

截个图描述一下

## 1.3 开发环境说明

easy

## 1.4 编译与运行

easy

# 2、 主要数据结构及算法设计

## 2.1 总体设计及算法主流程

画个各个类的交互关系图，画个算法流程图

render和io的交互 各个第三方库在干嘛 都在图里面摆放好

## 2.2 场景读取及双层加速结构的构建

说明一下as结构的设计思想

## 2.3 渲染管线的设计与实现

### 2.3.1 几何变换阶段

主要函数： mvp 裁剪 到 屏幕空间

都一样

### 2.3.2 光栅化阶段

主要函数概述

由遍历inst方式不同所以分了两条路

主要说明easy naive和 scanline三种

## 2.4 层次zbuffer算法的具体实现

### 2.4.1 BVH的构建与维护

### 2.4.2 HZB的构建与维护

### 2.4.3 算法实现

# 3、算法性能分析

## 3.1 各算法在多场景下的性能对比分析

列举对比

分析-由具体的例子截图来有力说明

## 3.2 加速策略总结

# 4、总结与展望





## coding style
- 类名采用大驼峰，类的数据成员采用"\_"开头，用"\_"分割，类的成员函数用小驼峰命名。
- 以release版本构建和运行
  ```bash
    mkdir build
    cmake -B ./build -DCMAKE_BUILD_TYPE=Release
    cmake --build ./build --config Release
  ```


