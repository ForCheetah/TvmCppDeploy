cmake_minimum_required (VERSION 3.5)

project (tvm_packed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(TVM_ROOT /home/xianmu/whs/TVM/tvm)
set(DMLC_CORE ${TVM_ROOT}/3rdparty/dmlc-core)



# # ********************************************************************************************
# 部署方式一： 需要编译 tvm_runtime ，将 模型调用.cpp 和 tvm_runtime 编译成一个可执行文件
message("deploy 1:")



# 添加可执行文件
set (SRC_LIST ${PROJECT_SOURCE_DIR}/src/cpp_deploy.cc
                    ${PROJECT_SOURCE_DIR}/src/tvm_runtime_pack.cc)
add_executable (MyRunnable ${SRC_LIST})

# 添加宏定义
target_compile_definitions(MyRunnable PRIVATE DMLC_USE_LOGGING_LIBRARY=\<tvm/runtime/logging.h\>)

# 设置编译选项
target_compile_options(MyRunnable PRIVATE -O2 -fPIC)

# 添加头文件目录
target_include_directories(MyRunnable PRIVATE
                            ${TVM_ROOT}/include 
                            ${DMLC_CORE}/include
                            ${TVM_ROOT}/3rdparty/dlpack/include)
# 链接库
target_link_libraries(MyRunnable PRIVATE dl pthread)


# ********************************************************************************************
# 部署方式二： 只将 模型调用.cpp 编译成可执行文件
message("deploy 2:")

# 添加可执行文件
add_executable (MyExcute ${PROJECT_SOURCE_DIR}/src/Resnet50_deploy.cc)
# add_executable (MyExcute ${PROJECT_SOURCE_DIR}/src/cpp_deploy.cc)

# 添加宏定义
target_compile_definitions(MyExcute PRIVATE DMLC_USE_LOGGING_LIBRARY=\<tvm/runtime/logging.h\>)

# 设置编译选项
target_compile_options(MyExcute PRIVATE -O2 -fPIC)

# 添加头文件目录
target_include_directories(MyExcute PRIVATE
                            ${TVM_ROOT}/include 
                            ${DMLC_CORE}/include
                            ${TVM_ROOT}/3rdparty/dlpack/include)

# 链接库
find_library(TVMRUNNINGLIB tvm_runtime HINTS ${TVM_ROOT}/build)
target_link_libraries(MyExcute PRIVATE dl pthread ${TVMRUNNINGLIB})