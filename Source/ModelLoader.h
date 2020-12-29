#include <iostream>
#include <nlohmann/json.hpp>
#include "NumCpp.hpp"
#include <string>

class ModelLoader
{

public:
	ModelLoader();
	nc::NdArray<float> vector_to_nc(std::vector<float> in_vec);
	nc::NdArray<float> matrix_to_nc(std::vector<std::vector<float>> in_mat);
    std::vector<nc::NdArray<float>> matrix3d_to_nc(nlohmann::json in_mat);
	void load_json(const char *filename);

    int input_size_loader = 0;
    int hidden_size = 0;

    int conv1d_kernel_size = 0;
    int conv1d_num_channels = 0;
    int conv1d_1_kernel_size = 0;
    int conv1d_1_num_channels = 0;
    int conv1d_stride_loader = 0;
    int conv1d_1_stride_loader = 0;

    nc::NdArray<float> conv1d_bias_nc;
    std::vector<nc::NdArray<float>> conv1d_kernel_nc;

    nc::NdArray<float> conv1d_1_bias_nc;
    std::vector<nc::NdArray<float>> conv1d_1_kernel_nc;

    nc::NdArray<float> lstm_bias_nc;
    nc::NdArray<float> lstm_kernel_nc;

    nc::NdArray<float> dense_bias_nc;
    nc::NdArray<float> dense_kernel_nc;


private:



};