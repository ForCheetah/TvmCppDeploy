
/*
    edited by Xianmu
*/

/*!
 * \brief Example code on load and run TVM module.s
 * \file cpp_deploy.cc
 */
#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

#include <cstdio>
#include <iterator>
#include <fstream>
#include <algorithm>


void DeployGraphExecutor() {
    LOG(INFO) << "Running graph executor...";
    // load in the library
    DLDevice dev{kDLCPU, 0};
    tvm::runtime::Module mod_factory = tvm::runtime::Module::LoadFromFile("/home/xiamu/whs/temp/resnet50-tvm/mod.so");
    
    // parameters in binary
    std::ifstream params_in("/home/xiamu/whs/temp/resnet50-tvm/mod.params", std::ios::binary);
    std::stringstream params_ss;
    params_ss << params_in.rdbuf();
    std::string params_data = params_ss.str();
    params_in.close();
    // parameters need to be TVMByteArray type to indicate the binary data
    TVMByteArray params_arr;
    params_arr.data = params_data.c_str();
    params_arr.size = params_data.length();
    
    // create the graph executor module
    tvm::runtime::Module gmod = mod_factory.GetFunction("default")(dev);
    tvm::runtime::PackedFunc set_input = gmod.GetFunction("set_input");
    tvm::runtime::PackedFunc get_output = gmod.GetFunction("get_output");
    tvm::runtime::PackedFunc run = gmod.GetFunction("run");
    tvm::runtime::PackedFunc load_params = gmod.GetFunction("load_params");
    load_params(params_arr);


    // Use the C++ API
    tvm::runtime::NDArray x = tvm::runtime::NDArray::Empty({1, 3, 224, 224}, DLDataType{kDLFloat, 32, 1}, dev);
    tvm::runtime::NDArray y = tvm::runtime::NDArray::Empty({1, 1000}, DLDataType{kDLFloat, 32, 1}, dev);

    for (int i = 0; i < 224*3; ++i) {
        for (int j = 0; j < 224; ++j) {
            static_cast<float*>(x->data)[i * 224 + j] = (i * 2 + j)*0.0003;
        }
    }
    // set the right input
    set_input("x", x);
    // run the code
    run();
    // get the output
    get_output(0, y);
    for(int index = 0; index<100;  index++){
        printf(" %.4f  ", static_cast<float*>(y->data)[index]);
    }
    

}

int main(void) {
    DeployGraphExecutor();
    return 0;
}
