/*
  ==============================================================================

  lstm

  ==============================================================================
*/

#include "lstm.h"
#include <boost/range/irange.hpp>
#include <iostream>


lstm::lstm()
//====================================================================
// Description: Default constructor for lstm class
//
//====================================================================
{

}


double lstm::sigmoid(float x)
//====================================================================
// Description: Takes the sigmoid of a sample of audio. Used in 
//   lstm gates.
//
//====================================================================
{
    return 1.0f / (1.0f + expf(-x));
}

void lstm::setParams(int hidden_size, int conv1d_kernel_size, int conv1d_1_kernel_size, int conv1d_num_channels, int conv1d_1_num_channels,
                    nc::NdArray<float> conv1d_bias_nc, nc::NdArray<float> conv1d_1_bias_nc, 
                    std::vector<nc::NdArray<float>> conv1d_kernel_nc, std::vector<nc::NdArray<float>> conv1d_1_kernel_nc,
                    nc::NdArray<float> lstm_bias_nc, nc::NdArray<float> lstm_kernel_nc, 
                    nc::NdArray<float> dense_bias_nc, nc::NdArray<float> dense_kernel_nc, int input_size_loader, 
                    int conv1d_stride_loader, int conv1d_1_stride_loader)
//====================================================================
// Description: Sets the parameters for the lstm class from the 
//   model loader class. These are set once when the default model 
//   is loaded, and are reset when a new model is loaded. Certain
//   arrays are initialized here based on the model params.
//
//====================================================================
{
    input_size = input_size_loader;
    HS = hidden_size;

    conv1d_Kernel_Size = conv1d_kernel_size;
    conv1d_Num_Channels = conv1d_num_channels;
    conv1d_1_Kernel_Size = conv1d_1_kernel_size;
    conv1d_1_Num_Channels = conv1d_1_num_channels;
    conv1d_stride = conv1d_stride_loader;
    conv1d_1_stride = conv1d_1_stride_loader;

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
    nc::NdArray<float> padded_dummy = pad(dummy_input, conv1d_Kernel_Size, conv1d_stride);  // TODO handle different strides
    int bias_shape = padded_dummy.shape().rows / conv1d_stride; // TODO handle different strides
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



nc::NdArray<float> lstm::pad(nc::NdArray<float> xt, int kernel_size, int stride) //TODO optimize these calculations, one for each conv1d layer, only do once
//====================================================================
// Description: Used in the in conv1d layers. This function applies
//   'same' padding to the input data to the conv1d layer. The padding
//   is all zeros. The amount of padding is based on the kernel_size
//   and stride of the conv1d layer.
//
//====================================================================
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



void lstm::unfold(int kernel_size, int stride)
//====================================================================
// Description: Used in the conv1d layers. This creates and 'unfolded'
//   array from the input data, and depends on the kernel_size and
//   stride. Each unfolded array is the 'sliding window' of size
//   kernel_size used in the convolution.
//
//====================================================================
{
    // TODO Having a kernel_size and stride that are not equal for the same conv1d layer breaks here (indexing issue), figure out why
    for (int i = 0; i < padded_xt.shape().rows / stride; i++)
    {
        unfolded_xt[i] = padded_xt(nc::Slice(i * stride, i * stride + kernel_size), 0);
    }
}


void lstm::conv1d_layer(nc::NdArray<float> xt)
//====================================================================
// Description: This is the first conv1d layer. It performes a 1 dimentional
//   convolution on the input data, and applies 'same' padding.
//   This is not a full implementation of a conv1d, and is specific
//   to this project. Kernel size and stride are determined based
//   on the trained model. 
//
//====================================================================
{
    padded_xt = pad(xt, conv1d_Kernel_Size, conv1d_stride);
    unfold(conv1d_Kernel_Size, conv1d_stride); // unfolded xt (9, 12, 1) .  weight shape (12, 1, 16), (tensordot(unfolded_xt, weight) = (9,16))

    // Compute tensordot
    len_i = unfolded_xt.size(); //9
    len_o = conv1d_kernel[0].shape().cols; //16
    len_j = conv1d_kernel.size(); //12
    len_k = unfolded_xt[0].shape().cols; //1
    total = 0.0;

    for (int i = 0; i < len_i; i++) 
    {
        for (int o = 0; o < len_o; o++)
        {
            total = 0.0;
            for (int j = 0; j < len_j; j++)
            {
                for (int k = 0; k < len_k; k++)
                {
                    total += unfolded_xt[i](j, k) * conv1d_kernel[j](k, o);
                }
            }
            conv1d_out(i, o) = total; //Faster to sum all here
        }
    }

    conv1d_out = conv1d_out + conv1d_bias;
}


void lstm::conv1d_layer2()
//====================================================================
// Description: This is the second conv1d layer. It performes a 
//   1-D convolution on the input data, and applies 'same' padding.
//   This is not a full implementation of a conv1d, and is specific
//   to this project. Kernel size and stride are determined based
//   on the trained model. 
//
//====================================================================
{
    padded_xt2 = pad(conv1d_out, conv1d_1_Kernel_Size, conv1d_1_stride);
    unfolded_xt2[0] = padded_xt2;

    // Compute tensordot
    len_i = unfolded_xt2.size(); //9
    len_o = conv1d_1_kernel[0].shape().cols; //16
    len_j = conv1d_1_kernel.size(); //12
    len_k = unfolded_xt2[0].shape().cols; //1
    total = 0.0;
    for (int i = 0; i < len_i; i++)
    {
        for (int o = 0; o < len_o; o++)
        {
            total = 0.0;
            for (int j = 0; j < len_j; j++)
            {
                for (int k = 0; k < len_k; k++)
                {
                    total += unfolded_xt2[i](j, k) * conv1d_1_kernel[j](k, o);
                }
            }
            conv1d_1_out(i, o) = total; //Faster to sum all here
        }
    }
    conv1d_1_out = conv1d_1_out + conv1d_1_bias;
}

//==============================================================================
void lstm::lstm_layer()
//====================================================================
// Description: This is the lstm layer. It has been simplified from
//   the full implementation to use stateful=False (the default setting
//   in Keras). This is the third layer in the stack.
//
//====================================================================
{

    gates = nc::dot(conv1d_1_out, W) + bias;

    // Check if using slicing notation is faster here         np: a[2:5, 5:8]	NC :   a(nc::Slice(2, 5), nc::Slice(5, 8))
    for (int i = 0; i < HS; i++) {
        h_t[i] = sigmoid(gates[3 * HS + i]) * nc::tanh(sigmoid(gates[i]) * nc::tanh(gates[2 * HS + i]));
    }
    lstm_out = h_t;
}

void lstm::dense_layer()
//====================================================================
// Description: This is the dense layer, and the 4th and last layer
//   in the stack. It takes the output from the lstm layer and
//   performs a dot product to reduce the network output to a single
//   predicted sample of audio.
//
//====================================================================
{
    dense_out = nc::dot(lstm_out, dense_kernel) + dense_bias;
}



void lstm::check_buffer(int numSamples)  //TODO this is called every block, how to call just at beginning and when buffer size changes?
//====================================================================
// Description: This checks if either the model or the buffer size
//   has changed, and if so, initializes certain arrays used for
//   calculating the output audio buffer. 
//
//====================================================================
{
    //This is done at plugin start up and when a new model is loaded, or when the buffer size is changed (numSamples)
    if (old_buffer.size() != numSamples + input_size - 1) {
        std::vector<float> temp(numSamples + input_size - 1, 0.0);
        old_buffer = temp;
        new_buffer = temp;
        std::vector<std::vector<float>> temp3(numSamples, std::vector<float>(input_size, 0.0));  //vector<vector> 128, 120
        data = temp3;
        // Reset the input
        input = nc::zeros<float>(nc::Shape(input_size, 1));
        //Set initial conv1d layer 1 arrays
        padded_xt = pad(input, conv1d_Kernel_Size, conv1d_stride); //TODO ability to set stride
        unfolded_xt.clear();
        for (int i = 0; i < padded_xt.shape().rows / conv1d_stride; i++)
        {
            unfolded_xt.push_back(padded_xt(nc::Slice(i * conv1d_stride, i * conv1d_stride + conv1d_Kernel_Size), 0));
        }
        conv1d_out = nc::zeros<float>(nc::Shape(unfolded_xt.size(), conv1d_kernel[0].shape().cols));
        // Set initial conv1d layer 2 array
        unfolded_xt2.clear();
        nc::NdArray<float> placeholder_temp;
        unfolded_xt2.push_back(placeholder_temp);
        conv1d_1_out = nc::zeros<float>(nc::Shape(unfolded_xt2.size(), conv1d_1_kernel[0].shape().cols));
    }
}

void lstm::set_data(const float* chData, int numSamples)
//====================================================================
// Description: This is called for each block of audio (each buffer).
//   It updates the buffer with the previous samples of length 
//   'input_size'.  The input_size is determined by the model, and
//   is used to predict each output sample of audio. For each output
//   sample of audio, the previous 'input_size' of samples are 
//   required to run the prediction.
//
//====================================================================
{

    //const float* chData = inputData[0];

    // Move input_size-1 of last buffer to the beginning of new_buffer
    for (int k = 0; k < input_size - 1; k++)
    {
        new_buffer[k] = old_buffer[numSamples + k]; // TODO double check indexing
    }

    // Update new_buffer with current buffer data
    for (int i = 0; i < numSamples; i++)
    {
        new_buffer[i + input_size - 1] = chData[i]; // TODO double check indexing
    }

    // Build array of inputs to the network from the new_buffer
    for (int i = 0; i < numSamples; i++)
    {
        for (int j = 0; j < input_size; j++) {
            data[i][j] = new_buffer[i + j];
        }
    }

    // Set the new_buffer data to old_buffer for the next block of audio
    old_buffer = new_buffer;
}

void lstm::process(const float* inData, float* outData, int numSamples)
//====================================================================
// Description: This processes each block (buffer) of audio data. 
//   It calls the initial data preparation functions, and then 
//   runs each sample of audio through the deep learning network.
//   The output is written to the write buffer. 
//
//====================================================================
{
    check_buffer(numSamples);
    set_data(inData, numSamples);

    for (int i = 0; i < numSamples; i++)
    {

        // Set the current sample input to LSTM
        for (int j = 0; j < input_size; j++) {
            input[j] = data[i][j];
        }
        conv1d_layer(input);
        conv1d_layer2();
        lstm_layer();
        dense_layer();
        outData[i] = dense_out[0];

    }

}