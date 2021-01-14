import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf
import tensorflow.keras as keras
from tensorflow.keras.models import load_model
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, LSTM, Conv1D
from tensorflow.keras.activations import sigmoid
from scipy.io import wavfile

# Developer Note:
#
# This script was used for developing a custom forward pass through the LSTM model
# using numpy. This script reads in a .wav file and compares the output
# of the Keras model and the custom python model. The python functions
# were then converted to c++ for the plugin.

# Reference:
# https://dtransposed.github.io/blog/Under-the-hood-LSTM.html


## Define parameters ##

input_size = 100        # Defines how long in time each batch is; one time step is one point of observation in the batch.
no_of_layers = 1        # Number of hidden LSTM layers in the network. (for this application will always be 1)
no_of_units = 36        # Number of units in every LSTM layer.

def pre_emphasis_filter(x, coeff=0.95):
    return tf.concat([x, x - coeff * x], 1)
    
def error_to_signal(y_true, y_pred): 
    y_true, y_pred = pre_emphasis_filter(y_true), pre_emphasis_filter(y_pred)
    return K.sum(tf.pow(y_true - y_pred, 2), axis=0) / K.sum(tf.pow(y_true, 2), axis=0) + 1e-10

def save_wav(name, data):
    if name.endswith('.wav') == False:
        name = name + '.wav'
    wavfile.write(name, 44100, data.flatten().astype(np.float32))
    print("Predicted wav file generated: "+name)

## Define weights matrices ##

'''
This function allows us to extract weights matrices in form of numpy arrays from a model in Keras. 
Key of the dictionary - name of a matrix
e.g. LSTM1_i_W is the matrix W of the input gate for the first LSTM layer.
Value of the key - numpy array associated with the matrix
'''
def import_weights(no_of_layers, hidden_units):
    layer_no = 0
    print("length of model_weights",len(model_weights))
    weights_dictionary["W_conv1d"] = model_weights[layer_no]
    weights_dictionary["b_conv1d"] = model_weights[layer_no + 1]
    layer_no += 2
    weights_dictionary["W_conv1d_1"] = model_weights[layer_no]
    weights_dictionary["b_conv1d_1"] = model_weights[layer_no + 1]
    layer_no += 2

    for index in range(1, no_of_layers+1):
        for matrix_type in ['W', 'U', 'b']:   # W = "kernel:0" or W_i;  U = "recurrent_kernel:0" or W_h
            if matrix_type != 'b':
                if matrix_type == 'W':
                    print('W',model_weights[layer_no][:].shape)
                    weights_dictionary["W"] = model_weights[layer_no][:]
                else:
                    print('U',model_weights[layer_no][:].shape)
                    weights_dictionary["U"] = model_weights[layer_no][:]
                layer_no = layer_no + 1
            else:
                weights_dictionary["b"] = model_weights[layer_no][:]
                layer_no = layer_no + 1
                
    weights_dictionary["W_dense"] = model_weights[layer_no]
    weights_dictionary["b_dense"] = model_weights[layer_no + 1]
    
## Define LSTM networks ##
    
'''
Keras_LSTM creates an LSTM network (Keras implementation)
custom_LSTM creates a single LSTM layer ('custom-made' implementation)
'''

class LSTM_Keras(object):  
    def __init__(self, hidden_units, input_size):
        self.input_size = input_size
        self.hidden_units = hidden_units       
        model = Sequential()
        model.add(Conv1D(16, 12,strides=12, activation=None, padding='same',input_shape=(self.input_size,1)))
        model.add(Conv1D(16, 12,strides=12, activation=None, padding='same'))
        model.add(LSTM(units = self.hidden_units, return_sequences = False))
        model.add(Dense(1, activation=None))
        self.model = model
    
class custom_LSTM(object):
    
    def __init__(self, input_size, no_of_units):
        self.input_size = input_size            # c++ code needs to handle a variable number of input_size (default is 100)
        self.hidden_units = no_of_units         # c++ code needs to handle a variable number of hidden_units (default is 36)
        self.hidden = np.zeros((self.input_size, self.hidden_units),dtype = np.float32)
        self.cell_state = np.zeros((self.input_size, self.hidden_units),dtype = np.float32)
        self.output_array = []

    def tanh(self, x):
        return np.tanh(x)
        
    def layer(self, xt, W, U, b):
        # This is the main function needed to be converted to c++ (the conv1d layer is already mostly implemented in c++ already)
        #
        # Optimized Version ( 2 bigger matrix multiplications instead of 8 smaller matrix multiplications ) 
        # Reference "Algorithm 1" on page 5 of research paper, that version
        #   might be easier to implement in c++.  It does the same thing as below, but it
        #   updates each index "for each i in [0 -> N"]) where "N" is the hidden size (self.hidden_units)
        #      Ignore line 2 from Algorithm 1 "if b_cond was given..", not implemented here
        #
        #  Note: Since this is not a stateful LSTM, the hidden and cell_states don't need to be updated with each pass
        #        This also simplifies the equations since the self.hidden and self.cell_state are always zero matrices
        #
        # The below matrix shapes are the default for the ts9_model.h5, the sizes are determined by the parameters listed beside the defaults
        # The c++ code will need to be able to account for variations in these parameters ("filters" and "hidden_units" can vary depending on the model parameters)
        #
        #   xt shape = (16,)        (filters,) <-- the number of filters in the conv1d_1 layer
        #   W shape =  (16, 144)    (filters, hidden_units*4) <-- the number of filters in the conv1d_1 layer, and hidden_units of LSTM * 4
        #   U shape =  (36, 144)    (hidden_units, hidden_units*4) <-- hidden_units in LSTM, and hidden_units of LSTM * 4
        #   bias shape = (144,)     (hidden_units*4) <-- hidden_units of LSTM * 4
        HS = self.hidden_units # Hidden size
        bias = b

        #gates = np.dot(xt, W) + np.dot(hidden, U) + bias
        gates = np.dot(xt, W) + bias # This can be simplified since hidden is always a zero matrix
        i_t, f_t, g_t, o_t = (
            sigmoid(gates[:, :HS]), # input  
            sigmoid(gates[:, HS:HS*2]), # forget
            self.tanh(gates[:, HS*2:HS*3]), 
            sigmoid(gates[:, HS*3:]), # output
        )
        #c_t = f_t * self.cell_state + i_t * g_t # Cell state
        c_t = i_t * g_t # Cell state can be simplified because self.cell_state is always a zero matrix
        h_t = o_t * self.tanh(c_t) # Hidden state (output of LSTM layer)

        return h_t


    def reset_state(self):
        self.hidden = np.zeros((self.input_size, self.hidden_units),dtype = np.float32)
        self.cell_state = np.zeros((self.input_size, self.hidden_units),dtype = np.float32)
        
    def dense(self, x, weights, bias):
        result = np.dot(x, weights)+bias
        self.result=result[0]
        return result[0]

    def output_array_append(self):
        self.output_array.append(self.result[0])


    def pad(self, x, kernel_size, stride):   # c++ code needs to handle a variable number of kernel_size (default = 12) and stride (default = 12)
        """'same' padding
        Pads the input data at the beginning and end with 0's so that "size input data" = "size output data"
        """
        seq_len, channels = x.shape
        if seq_len % stride == 0:
            pad_width = max(kernel_size - stride, 0)
        else:
            pad_width = max(kernel_size - (seq_len % stride), 0)
        pad_left = pad_width // 2
        pad_right = pad_width - pad_left
        return np.concatenate([np.zeros((pad_left, channels)), x, np.zeros((pad_right, channels))])

    def unfold(self, x, kernel_size, stride):
        return np.stack([[c for c in x[i*stride:i*stride+kernel_size]] for i in range(len(x)//stride)])

    def conv1d(self, x, weight, bias, stride):
        kernel_size = weight.shape[0]
        out = self.pad(x, kernel_size, stride)
        out = self.unfold(out, kernel_size, stride)
        # can use np.tensordot or np.einsum here (they do the same thing in this case)
        #out = np.einsum("lkc,kco->lo", out, weight) + bias  # Note: l = sequence, k = kernel, c = input channels, o = output channels
        out = np.tensordot(out, weight) + bias
        self.result=out
        return self.result    


## Main ##

########################################################### Load and process the audio input data
in_rate, in_data = wavfile.read('data/ts9_test1_in_FP32.wav')
X = in_data.astype(np.float32).flatten()  #read input wav file to np array

indices = np.arange(input_size) + np.arange(len(in_data)-input_size+1)[:,np.newaxis] 
X_ordered = tf.gather(in_data,indices) 
x_first_part, x_last_part = np.split(X_ordered, [int(len(X)*.8)])
input_to_keras = x_last_part[0:22050, 0:, np.newaxis] # Take 1/2 second of the input data to test with
print("Shape of input: ", x_last_part.shape)
print("Type of input: ", type(x_last_part))
########################################################### End Load and process the audio input data

    
#create a model in Keras by loading the pre trained ts9_model.h5 (ts9 is the Ibanez Tubescreamer overdrive pedal)
keras_model = load_model("ts9_model.h5", custom_objects={'error_to_signal' : error_to_signal})

model_weights = keras_model.get_weights()                                       #get weights of a network from the model (i.e. matrices of the trained network for each layer)
weights_dictionary = {}                                                         #create an empty dictionary
import_weights(no_of_layers, no_of_units)                                       #fill the dictionary with weights matrices from Keras

LSTM_layer_1 = custom_LSTM(input_size, no_of_units)

print("Shape of input_to_keras", input_to_keras.shape)

## Prediction step using custom-made LSTM ##
print("Number of batches:", input_to_keras.shape[0])
for batch in range(input_to_keras.shape[0]):

    # Layer 1: conv1d (1-D Convolutional layer)
    LSTM_layer_1.conv1d(input_to_keras[batch,:,:], weights_dictionary['W_conv1d'], weights_dictionary['b_conv1d'], stride=12)

    # Layer 2: conv1d_1 (second 1-D Convolutional layer)
    LSTM_layer_1.conv1d(LSTM_layer_1.result, weights_dictionary['W_conv1d_1'], weights_dictionary['b_conv1d_1'], stride=12)

    # Layer 3: LSTM (Long short term memory layer)
    output_from_LSTM_1 = LSTM_layer_1.layer(LSTM_layer_1.result, weights_dictionary['W'], weights_dictionary['U'], weights_dictionary['b'])
    
    # Layer 4: Dense layer (Fully connected layer)
    LSTM_layer_1.dense(output_from_LSTM_1, weights_dictionary['W_dense'], weights_dictionary['b_dense'])
    LSTM_layer_1.output_array_append()

## Compare custom-made implementation  ##
    
result_custom = LSTM_layer_1.output_array
print(keras_model.summary())
result_keras = keras_model.predict(input_to_keras)

save_wav("keras_lstm.wav", np.array(result_keras))
save_wav("custom_lstm.wav", np.array(result_custom))

dummy_axis = list(range(len(result_custom)))
plt.plot(dummy_axis, result_keras, label ='Keras LSTM', marker='x')
plt.plot(dummy_axis, result_custom, label ='Custom-made LSTM')
plt.legend(loc='best')
plt.xlabel('Data point')
plt.ylabel('Value')
plt.title('Comparison of two methods')
plt.show()
