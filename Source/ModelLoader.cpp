#include "ModelLoader.h"
#include <iostream>

#include <nlohmann/json.hpp>

//==============================================================================
ModelLoader::ModelLoader()
{
}

nc::NdArray<float> ModelLoader::vector_to_nc(std::vector<float> in_vec)
{
    nc::NdArray<float> out_vec = nc::random::rand<float>(nc::Shape(1, in_vec.size()));;

    for (int i = 0; i < in_vec.size(); i++)
    {
        out_vec[i] = in_vec[i]; 
    }
    return out_vec;
}

nc::NdArray<float> ModelLoader::matrix_to_nc(std::vector<std::vector<float>> in_mat)
{
    nc::NdArray<float> out_mat = nc::random::rand<float>(nc::Shape(in_mat.size(), in_mat[0].size()));

    for (int i = 0; i < in_mat.size(); i++)
    {
        for (int j = 0; j < in_mat[0].size(); j++)
        {
            out_mat(i, j) = in_mat[i][j];
        }
    }
    return out_mat;
}


std::vector<nc::NdArray<float>> ModelLoader::matrix3d_to_nc(nlohmann::json in_mat)
{
    std::vector<nc::NdArray<float>> out_mat;
    nc::NdArray<float> nc_2D_matrix = nc::random::rand<float>(nc::Shape(in_mat[0].size(), in_mat[0][0].size()));
    for (int i = 0; i < in_mat.size(); i++)
    {
        for (int j = 0; j < in_mat[0].size(); j++)
        {
            for (int k = 0; k < in_mat[0][0].size(); k++)
            {
                nc_2D_matrix(j, k) = in_mat[i][j][k];
            }
        }
        out_mat.push_back(nc_2D_matrix);
    }
   
    return out_mat;
}



void ModelLoader::load_json(const char *filename)
{
    // read a JSON file
    std::ifstream i2(filename);
    nlohmann::json weights_json;
    i2 >> weights_json;
    
    input_size_loader = weights_json["/input_size"_json_pointer];

    std::vector<float> conv1d_bias = weights_json["/conv1d/bias:0"_json_pointer];
    conv1d_bias_nc = vector_to_nc(conv1d_bias);
    conv1d_kernel_nc = matrix3d_to_nc(weights_json["/conv1d/kernel:0"_json_pointer]);
    conv1d_stride_loader = weights_json["/conv1d_stride"_json_pointer];

    conv1d_kernel_size = conv1d_kernel_nc.size();
    conv1d_num_channels = conv1d_bias_nc.size();
    
    std::vector<float> conv1d_1_bias = weights_json["/conv1d_1/bias:0"_json_pointer];
    conv1d_1_bias_nc = vector_to_nc(conv1d_1_bias);
    conv1d_1_kernel_nc = matrix3d_to_nc(weights_json["/conv1d_1/kernel:0"_json_pointer]);
    conv1d_1_stride_loader = weights_json["/conv1d_1_stride"_json_pointer];

    conv1d_1_kernel_size = conv1d_1_kernel_nc.size();
    conv1d_1_num_channels = conv1d_1_bias_nc.size();

    std::vector<float> lstm_bias = weights_json["/lstm/bias:0"_json_pointer];
    std::vector<std::vector<float>> lstm_kernel = weights_json["/lstm/kernel:0"_json_pointer];
    lstm_bias_nc = vector_to_nc(lstm_bias);
    lstm_kernel_nc = matrix_to_nc(lstm_kernel);

    hidden_size = lstm_bias.size() / 4;

    std::vector<float> dense_bias = weights_json["/dense/bias:0"_json_pointer];
    std::vector<std::vector<float>> dense_kernel = weights_json["/dense/kernel:0"_json_pointer];
    dense_bias_nc = vector_to_nc(dense_bias);
    dense_kernel_nc = matrix_to_nc(dense_kernel);
}