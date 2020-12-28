/*
  ==============================================================================

  lstm

  ==============================================================================
*/

#include "ModelLoader.h"
#include "NumCpp.hpp"

#include <algorithm>

class lstm
{

    public:
        lstm();

        // Math and vector operations
        double sigmoid(double x);
        nc::NdArray<float> pad(nc::NdArray<float> xt, int kernel_size, int stride);
        void unfold(int kernel_size, int stride);

        // Layers
        void conv1d_layer(nc::NdArray<float> xt, int stride);
        void conv1d_layer2(int stride);
        void lstm_layer();
        void dense_layer();

        void lstm::check_buffer(int numSamples);
        void lstm::set_data(const float* inData, int numSamples);
        void lstm::process(const float* inData, float* outData, int stride, int numSamples);

        void setParams(int hidden_size, int conv1d_num_kernels, int conv1d_1_num_kernels, int conv1d_stride, int conv1d_1_stride,
            nc::NdArray<float> conv1d_bias_nc, nc::NdArray<float> conv1d_1_bias_nc,
            std::vector<nc::NdArray<float>> conv1d_kernel_nc, std::vector<nc::NdArray<float>> conv1d_1_kernel_nc,
            nc::NdArray<float> lstm_bias_nc, nc::NdArray<float> lstm_kernel_nc,
            nc::NdArray<float> dense_bias_nc, nc::NdArray<float> dense_kernel_nc, int input_size_loader);


        // Data processing
        nc::NdArray<float> input = nc::zeros<float>(nc::Shape(input_size, 1));
        std::vector<float> new_buffer;
        std::vector<float> old_buffer;
        std::vector<std::vector<float>> data;

        // Network Parameters
        int input_size = 120; // TODO read from file

        int HS = 1;
        int conv1d_Kernel_Size = 0;
        int conv1d_Num_Channels = 0;
        int conv1d_1_Kernel_Size = 0;
        int conv1d_1_Num_Channels = 0;

        int seq_len = 1;
        int local_channels = 1;
        int seq_mod_stride = 0;

        nc::NdArray<float> gates = nc::zeros<float>(1, HS * 4);
        nc::NdArray<float> h_t;

        // conv1d eigen calc
        int pad_width = 0;
        int pad_left = 0;
        int pad_right = 0;

        int len_i = 0; //9
        int len_o = 0; //16
        int len_j = 0; //12
        int len_k = 0; //1
        float total = 0.0;

        // Loaded Model Weights
        nc::NdArray<float> conv1d_bias;
        nc::NdArray<float> conv1d_1_bias;
        std::vector<nc::NdArray<float>> conv1d_kernel;
        std::vector<nc::NdArray<float>> conv1d_1_kernel;
        nc::NdArray<float> W;
        nc::NdArray<float> bias;
        nc::NdArray<float> dense_kernel;
        nc::NdArray<float> dense_bias;

        // Layer outputs
        nc::NdArray<float> conv1d_out;
        nc::NdArray<float> conv1d_1_out;
        nc::NdArray<float> lstm_out;
        nc::NdArray<float> dense_out;

        // conv1d arrays
        std::vector<nc::NdArray<float>> unfolded_xt;
        nc::NdArray<float> padded_xt;
        std::vector<nc::NdArray<float>> unfolded_xt2;
        nc::NdArray<float> padded_xt2;
        
    private:

};

