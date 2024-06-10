# Deploy TVM Module using C++ API

This IS A Way to Deploy TVM Module using C++ API

# 现状

TVM官方文档:[英文文档](https://tvm.apache.org/docs) [中文文档](https://tvm.hyper.ai/) 主要介绍了通过Python脚本和Python命令行tvmc来编译和部署TVM。但是以这两种方式部署，部署设备还需要安装Python运行环境，带来额外空间占用和开销。显然不能以这种方式部署。

TVM项目的 [howto_deploy](https://github.com/apache/tvm/tree/main/apps/howto_deploy) 目录下提供了G\++编译C\++代码部署TVM的方式。遗憾的是给的例子没有包含模型的权重.params和图结构.json的加载，也没有输入图片的加载。

这里提供了一个简单的C\+\+部署TVM工程，可以在 [TvmCppDeploy项目](https://github.com/ForCheetah/TvmCppDeploy) 找到并下载，用于你的TVM项目部署。

该项目没有使用 [TVM项目 howto_deploy](https://github.com/apache/tvm/tree/main/apps/howto_deploy) 中的Makefile，而是重写了CMakeLists.txt文件，更方便读懂和修改。

# 使用方式

下载 [TvmCppDeploy项目](https://github.com/ForCheetah/TvmCppDeploy) 到你的本地，可以通过下载zip文件后解压缩，也可以直接git：

```bash
git clone https://github.com/ForCheetah/TvmCppDeploy.git
```

进入项目根目录，进行必要的路径修改。和 [TVM项目 howto_deploy](https://github.com/apache/tvm/tree/main/apps/howto_deploy) 一样，本项目也提供了两种部署方式，所需要修改的内容也有些不同。

# 重新编译 tvm_runtime

重新编译 tvm\_runtime，和个人的C\++文件编译在一起，编译好的可执行文件可独立执行。

- 第1步：打开 `src/Resnet50_deploy.cc `文件，找到81行 `const std::string artifacts_folder("/home/xiamu/whs/temp/resnet50-tvm/");` ，将其中的`/home/xiamu/whs/temp/resnet50-tvm/`修改为自己的已经编译好的模型路径，该路径下应该存在有 `mod.so, mod.params, mod.json`。
- 第2步：还是`src/Resnet50_deploy.cc `文件， 找到132行，将其中的图片路径`/home/xiamu/whs/python/remote_tvm/imagenet_cat.bin` 改为自己的图片路径，该bin文件应当是已经转换好的float格式文件。
- 第3步：打开  `src/tvm\_runtime\_pack.cc`， 将文件中所有的路径中的 `/home/xiamu/tvm`  修改为你本地TVM工程的根目录路径。 修改完一定要检查一下对应的目录中是否有相应的文件。
- 第4步：打开 `CMakeLists.txt`, 找到第10行 `set(TVM_ROOT /home/xianmu/tvm)`，将其中的`/home/xianmu/tvm`改成你本地TVM工程的根目录路径。
- 为防止编译报错，可以将`部署方式二： tvm_runtime.so 作为动态链接库编译`对应的代码（43至63行） 注释掉（当前可能还没有对其进行修改）。
- 第5步：编译和执行，在根目录下：
```bash
mkdir build && cd build
cmake ..
make -j4
./MyRunnable
```


# tvm_runtime.so 作为动态链接库编译

tvm\_runtime.so 作为动态链接库，仅编译个人的C\++文件，运行时需要链接 libtvm\_runtime.so


这种方式的修改与第一种方式略有不同，修改如下：
- 第1步：打开 `src/Resnet50_deploy.cc `文件，找到81行 `const std::string artifacts_folder("/home/xiamu/whs/temp/resnet50-tvm/");` ，将其中的`/home/xiamu/whs/temp/resnet50-tvm/`修改为自己的已经编译好的模型路径，该路径下应该存在有 `mod.so, mod.params, mod.json`。
- 第2步：还是`src/Resnet50_deploy.cc `文件， 找到132行，将其中的图片路径`/home/xiamu/whs/python/remote_tvm/imagenet_cat.bin` 改为自己的图片路径，该bin文件应当是已经转换好的float格式文件。
- 第3步：打开 `CMakeLists.txt`, 找到第10行 `set(TVM_ROOT /home/xianmu/tvm)`，将其中的`/home/xianmu/tvm`改成你本地TVM工程的根目录路径。将62行的`${TVM_ROOT}/build` libtvm\_runtime.so路径修改为你存放libtvm\_runtime.so库的路径。
- 为防止编译报错，可以将`部署方式一： 重新编译 tvm_runtime` 对应的代码（17至38行）注释掉（当前可能还没有对其进行修改）。
- 第5步：编译和执行，在根目录下：
```bash
mkdir build && cd build
cmake ..
make -j4
./MyExcute
```







