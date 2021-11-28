# P3DKit
项目简介：
工作中自用的跨平台渲染工具，基于OPenGL开发，最GL的底层调用做了基本封装，为尽可能方便使用，包含以下功能和特点：
1、实现了简单的3D数学库，支持4x4矩阵，向量和四元数的各种几何操作(参考了部分glm代码)；
2、跨平台项目，使用CMake管理项目，支持在headless的linux环境下运行(离屏渲染，需配置EGL)；
3、使用imageUI作为UI界面，内置简单的鼠标键盘控制操作，且可自由拓展；
4、支持脚本化编程，使用json文件作为场景和渲染配置文件，详细描述建下文；
5、对embree做了简单的封装(拓展ray-tracing渲染器)，对physx做了简单封装(物理拓展)，对cudnn做了简单封装(nn模型的infernce拓展)；
6、完美支持python调用；

代码结构：
1、common目录内存放各种基本机构，基本操作和各种宏定义等，包括文件和字符串操作以及各种基础模板等；
2、engine目录内存放渲染相关文件，外层文件为一般抽象。
  (1)engine/OPenGL存放opengl实现，engine/Vul存放vulkan实现（待拓展）；
  (2)engine/loader实现模型和图片的load，大部分模型的load通过assimp实现，fbx文件则通过fbxSDK实现，支持bs，joints和骨骼动画；
  (3)engine/mgs包含各种逻辑层面的实现，包括渲染和场景管理器，渲染节点等，场景解析和渲染解析也在这里实现，脚本化渲染依赖于此；
  (4) extern包含一些拓展功能，目前的鼠标键盘控制模块由此实现，基于中介者模式，可随意添加listener实现控制功能的拓展；
3、products内包含各种中间功能模块，可依赖于common和engine模块，也可交叉依赖，baseApp也存放在此模块内；
4、testCodes和apps为各种测试代码，编译为可执行程序；

simple_demos有渲染配置和python编程的一般示例。
代码仅做展示只用，部分代码并未上传。
