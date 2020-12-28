/*
  ==============================================================================

  lstm

  ==============================================================================
*/

#include "lstm.h"
#include <boost/range/irange.hpp>
#include <iostream>

//==============================================================================
lstm::lstm()
{

}


double lstm::sigmoid(double x)
{
    return 1.0f / (1.0f + expf(-x));
}

void lstm::setParams(int hidden_size, int conv1d_kernel_size, int conv1d_1_kernel_size, int conv1d_num_channels, int conv1d_1_num_channels,
                    nc::NdArray<float> conv1d_bias_nc, nc::NdArray<float> conv1d_1_bias_nc, 
                    std::vector<nc::NdArray<float>> conv1d_kernel_nc, std::vector<nc::NdArray<float>> conv1d_1_kernel_nc,
                    nc::NdArray<float> lstm_bias_nc, nc::NdArray<float> lstm_kernel_nc, 
                    nc::NdArray<float> dense_bias_nc, nc::NdArray<float> dense_kernel_nc, int input_size_loader)
{
    input_size = input_size_loader;
    HS = hidden_size;

    conv1d_Kernel_Size = conv1d_kernel_size;
    conv1d_Num_Channels = conv1d_num_channels;
    conv1d_1_Kernel_Size = conv1d_1_kernel_size;
    conv1d_1_Num_Channels = conv1d_1_num_channels;

    nc::NdArray<float> conv1d_bias_temp = conv1d_bias_nc;
    conv1d_kernel = conv1d_kernel_nc;

    nc::NdArray<float> conv1d_1_bias_temp = conv1d_1_bias_nc;
    conv1d_1_kernel = conv1d_1_kernel_nc;

    W = lstm_kernel_nc;
    bias = lstm_bias_nc;

    dense_kernel = dense_kernel_nc;
    dense_bias = dense_bias_nc;

    h_t = nc::zeros<float>(1, HS);

    gates = nc::zeros<float>(1, HS * 4);

    nc::NdArray<float> dummy_input = nc::zeros<float>(nc::Shape(input_size, 1));

    // Set up bias matrices for calculation 
    nc::NdArray<float> padded_dummy = pad(dummy_input, conv1d_Kernel_Size, 12);  // TODO handle different strides
    int bias_shape = padded_dummy.shape().rows / 12; // TODO handle different strides
    conv1d_bias = nc::zeros<float>(nc::Shape(bias_shape, conv1d_bias_temp.shape().cols));;
    nc::NdArray<float> new_bias = conv1d_bias_temp;
    for (int i = 0; i < bias_shape - 1; i++)
    {
        new_bias = nc::append(new_bias, conv1d_bias_temp, nc::Axis::ROW);
    }
    conv1d_bias = new_bias;


    bias_shape = 1; //TODO fix having to hardcode
    conv1d_1_bias = nc::zeros<float>(nc::Shape(bias_shape, conv1d_1_bias_temp.shape().cols));;
    nc::NdArray<float> new_bias2 = conv1d_1_bias_temp;
    for (int i = 0; i < bias_shape - 1; i++)
    {
        new_bias2 = nc::append(new_bias2, conv1d_1_bias_temp, nc::Axis::ROW);
    }
    conv1d_1_bias = new_bias2;

}



nc::NdArray<float> lstm::pad(nc::NdArray<float> xt, int kernel_size, int stride)
{

    seq_len = xt.shape().rows;
    local_channels = xt.shape().cols;
    seq_mod_stride = seq_len % stride;

    if (seq_mod_stride == 0) {
        pad_width = std::max(kernel_size - stride, 0);
    } else {
        pad_width = std::max(kernel_size - seq_mod_stride, 0);
    }
    pad_left = pad_width / 2; // Integer division here, remainder is left out
    pad_right = pad_width - pad_left;
    return nc::vstack({ nc::zeros<float>(pad_left, local_channels), xt, nc::zeros<float>(pad_right, local_channels) });
}



std::vector<nc::NdArray<float>> lstm::unfold(nc::NdArray<float> padded_xt, int kernel_size, int stride, int layer_num)
{
    //std::vector<nc::NdArray<float>> unfolded_xt;

    //if (padded_xt.shape().rows == 12) {  // if 2nd conv1d layer TODO figure out how to get both to work with same code
    //if (layer_num > 1) {
    //    nc::NdArray<float> placeholder = padded_xt;
    //    unfolded_xt.push_back(placeholder);
    //} else {

    for (int i = 0; i < padded_xt.shape().rows / stride; i++)
    {
        placeholder = padded_xt(nc::Slice(i * stride, i * stride + kernel_size), 0);
        //unfolded_xt.push_back(placeholder);
        unfolded_xt[i] = placeholder;
    }
    //}

    return unfolded_xt;
}


std::vector<nc::NdArray<float>> lstm::unfold2(nc::NdArray<float> padded_xt, int kernel_size, int stride, int layer_num)
{
    //std::vector<nc::NdArray<float>> unfolded_xt2;

    //if (padded_xt.shape().rows == 12) {  // if 2nd conv1d layer TODO figure out how to get both to work with same code
    //if (layer_num > 1) {
        //placeholder = padded_xt;
    unfolded_xt[0] = padded_xt;
    //}
    /*
    else {

        for (int i = 0; i < padded_xt.shape().rows / stride; i++)
        {
            nc::NdArray<float> placeholder = padded_xt(nc::Slice(i * stride, i * stride + kernel_size), 0);
            unfolded_xt.push_back(placeholder);
        }
    }
    */

    return unfolded_xt;
}


void lstm::conv1d_layer(nc::NdArray<float> xt, std::vector<nc::NdArray<float>> weight,
                                      nc::NdArray<float> bias,int kernel_size, int channels, int stride, int layer_num)
{
    padded_xt = pad(xt, kernel_size, stride);
    unfolded_xt = unfold(padded_xt, kernel_size, stride, layer_num); // unfolded xt (9, 12, 1) .  weight shape (12, 1, 16), (tensordot(unfolded_xt, weight) = (9,16))

    out = nc::zeros<float>(nc::Shape(unfolded_xt.size(), weight[0].shape().cols)); //zeros instead of random faster?
    // Compute tensordot
    len_i = unfolded_xt.size(); //9
    len_o = weight[0].shape().cols; //16
    len_j = weight.size(); //12
    len_k = unfolded_xt[0].shape().cols; //1
    total = 0.0;
    //std::cout << len_i << " " << len_o << " " << len_j << " " << len_k << " " << std::endl;
    for (int i = 0; i < len_i; i++) 
    {
        for (int o = 0; o < len_o; o++)
        {
            total = 0.0;
            for (int j = 0; j < len_j; j++)
            {
                for (int k = 0; k < len_k; k++)
                {
                    total += unfolded_xt[i](j, k) * weight[j](k, o);
                }
            }
            out(i, o) = total; //Faster to sum all here, or in the k loop?
        }
    }

    out = out + bias;

    //return out;
}


void lstm::conv1d_layer2(nc::NdArray<float> xt, std::vector<nc::NdArray<float>> weight,
    nc::NdArray<float> bias, int kernel_size, int channels, int stride, int layer_num)
{
    padded_xt2 = pad(xt, kernel_size, stride);
    unfolded_xt2 = unfold2(padded_xt, kernel_size, stride, layer_num); // unfolded xt (9, 12, 1) .  weight shape (12, 1, 16), (tensordot(unfolded_xt, weight) = (9,16))

    out2 = nc::zeros<float>(nc::Shape(unfolded_xt.size(), weight[0].shape().cols)); //zeros instead of random faster?
    // Compute tensordot
    len_i = unfolded_xt.size(); //9
    len_o = weight[0].shape().cols; //16
    len_j = weight.size(); //12
    len_k = unfolded_xt[0].shape().cols; //1
    total = 0.0;
    //std::cout << len_i << " " << len_o << " " << len_j << " " << len_k << " " << std::endl;
    for (int i = 0; i < len_i; i++)
    {
        for (int o = 0; o < len_o; o++)
        {
            total = 0.0;
            for (int j = 0; j < len_j; j++)
            {
                for (int k = 0; k < len_k; k++)
                {
                    total += unfolded_xt[i](j, k) * weight[j](k, o);
                }
            }
            out2(i, o) = total; //Faster to sum all here, or in the k loop?
        }
    }

    out2 = out2 + bias;

    //return out2;
}

//==============================================================================
void lstm::lstm_layer(nc::NdArray<float> xt)
{

    gates = nc::dot(xt, W) + bias; 

    // Check if using slicing notation is faster here         np: a[2:5, 5:8]	NC :   a(nc::Slice(2, 5), nc::Slice(5, 8))
    for (int i = 0; i < HS; i++) {
        h_t[i] = sigmoid(gates[3 * HS + i]) * nc::tanh(sigmoid(gates[i]) * nc::tanh(gates[2 * HS + i]));
    }

    //return h_t;
}

void lstm::dense_layer(nc::NdArray<float> xt)
{
    dense_out = nc::dot(xt, dense_kernel) + dense_bias;
}