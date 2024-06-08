
/*
    edited by Xianmu
*/
#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

#include <cstdio>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <cassert>

typedef struct
{
    int id;
    float score;
} cls_score;


static void sort_cls_score(cls_score* array, int left, int right)
{
    int i = left;
    int j = right;
    cls_score key;

    if (left >= right)
        return;

    memmove(&key, &array[left], sizeof(cls_score));
    while (left < right)
    {
        while (left < right && key.score >= array[right].score)
        {
            --right;
        }
        memmove(&array[left], &array[right], sizeof(cls_score));
        while (left < right && key.score <= array[left].score)
        {
            ++left;
        }
        memmove(&array[right], &array[left], sizeof(cls_score));
    }

    memmove(&array[left], &key, sizeof(cls_score));

    sort_cls_score(array, i, left - 1);
    sort_cls_score(array, left + 1, j);
}

void print_topk(float* data, int total_num, int topk)
{
    assert(total_num >= topk);
    cls_score* cls_scores = (cls_score*)malloc(total_num * sizeof(cls_score));
    for (int i = 0; i < total_num; i++)
    {
        cls_scores[i].id = i;
        cls_scores[i].score = data[i];
    }

    sort_cls_score(cls_scores, 0, total_num - 1);
    char strline[128] = "";
    for (int i = 0; i < topk; i++)
    {
        fprintf(stderr, "value : %f\t\t index : %d\n", cls_scores[i].score, cls_scores[i].id);
    }
    free(cls_scores);
}

void DeployGraphExecutor2() {

    constexpr int dtype_code = kDLFloat;
    constexpr int dtype_bits = 32;
    constexpr int dtype_lanes = 1;
    constexpr int device_id = 0;



    const std::string artifacts_folder("/home/xiamu/whs/temp/resnet50-tvm/");

    // load in the library
    DLDevice dev{kDLCPU, 0};
    tvm::runtime::Module loaded_lib = tvm::runtime::Module::LoadFromFile(artifacts_folder + "mod.so");

    // Load JSON
    std::ifstream loaded_json(artifacts_folder + "mod.json");
    std::string json_data((std::istreambuf_iterator<char>(loaded_json)), std::istreambuf_iterator<char>());
    loaded_json.close();

    // Load params from file
    std::ifstream loaded_params(artifacts_folder + "mod.params", std::ios::binary);
    std::string params_data((std::istreambuf_iterator<char>(loaded_params)), std::istreambuf_iterator<char>());
    loaded_params.close();
    TVMByteArray params_arr;
    params_arr.data = params_data.c_str();
    params_arr.size = params_data.length();

    LOG(INFO) << "Creating graph executor...";
    // Create the graph executor module
    int device_type = dev.device_type; // Need an int, the DLDeviceType enum 
                                        // results in an ambiguity for TVMArgsSetter.

    tvm::runtime::Module mod = 
        (*tvm::runtime::Registry::Get("tvm.graph_executor.create"))(json_data,
                                                                    loaded_lib,
                                                                device_type,
                                                                    dev.device_id);

    // Load params into Graph Executor
    LOG(INFO) << "Loading params ...";
    tvm::runtime::PackedFunc load_params = mod.GetFunction("load_params");
    load_params(params_arr);

    tvm::runtime::PackedFunc set_input           = mod.GetFunction("set_input");
    tvm::runtime::PackedFunc get_output          = mod.GetFunction("get_output");
    tvm::runtime::PackedFunc run                 = mod.GetFunction("run");

    LOG(INFO) << "Initializing inputs ...";
    auto f32 = tvm::runtime::DataType::Float(32);    // 等价于 DLDataType{kDLFloat, 32, 1}
    // tvm::runtime::NDArray input = tvm::runtime::NDArray::Empty({1, 3, 224, 224}, f32, dev);
    // tvm::runtime::NDArray b = tvm::runtime::NDArray::Empty({672, 1, 1},   f32, dev);
    tvm::runtime::NDArray out = tvm::runtime::NDArray::Empty({1, 1000}, f32, dev);


    DLTensor* input;
    int in_ndim = 4;
    int64_t in_shape[4] = {1, 3, 224, 224};
    TVMArrayAlloc(in_shape, in_ndim, dtype_code, dtype_bits, dtype_lanes, device_type, device_id, &input);
    // load image data saved in binary
    std::ifstream data_fin("/home/xiamu/whs/python/remote_tvm/imagenet_cat.bin", std::ios::binary);
    data_fin.read(static_cast<char*>(input->data), 3 * 224 * 224 * 4);


    set_input("data", input);


    // run the code
    LOG(INFO) << "Running ...";
    run();

    // get the output
    get_output(0, out);

    print_topk(static_cast<float*>(out->data), 1000, 10);

        
    LOG(INFO) << "Pass";
}



int main(void) {
    // DeployGraphExecutor();
    DeployGraphExecutor2();
    return 0;
}
