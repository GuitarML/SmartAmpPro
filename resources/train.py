import tensorflow as tf
from tensorflow.keras import Sequential
from tensorflow.keras.layers import LSTM, Conv1D, Dense
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.backend import clear_session
from tensorflow.keras.activations import tanh, elu, relu
from tensorflow.keras.models import load_model
import tensorflow.keras.backend as K
from tensorflow.keras.utils import Sequence

import os
from scipy import signal
from scipy.io import wavfile
import numpy as np
import matplotlib.pyplot as plt
import math
import h5py
import argparse

import json
   
def pre_emphasis_filter(x, coeff=0.95):
    return tf.concat([x, x - coeff * x], 1)
    
def error_to_signal(y_true, y_pred): 
    """
    Error to signal ratio with pre-emphasis filter:
    """
    y_true, y_pred = pre_emphasis_filter(y_true), pre_emphasis_filter(y_pred)
    return K.sum(tf.pow(y_true - y_pred, 2), axis=0) / K.sum(tf.pow(y_true, 2), axis=0) + 1e-10
    
def save_wav(name, data):
    wavfile.write(name, 44100, data.flatten().astype(np.float32))

def normalize(data):
    data_max = max(data)
    data_min = min(data)
    data_norm = max(data_max,abs(data_min))
    return data / data_norm

def main(args):
    '''Ths is a similar Tensorflow/Keras implementation of the LSTM model from the paper:
        "Real-Time Guitar Amplifier Emulation with Deep Learning"
        https://www.mdpi.com/2076-3417/10/3/766/htm

        Uses a stack of two 1-D Convolutional layers, followed by LSTM, followed by 
        a Dense (fully connected) layer. Three preset training modes are available, 
        with further customization by editing the code. A Sequential tf.keras model 
        is implemented here.

        Note: RAM may be a limiting factor for the parameter "input_size". The wav data
          is preprocessed and stored in RAM, which improves training speed but quickly runs out
          if using a large number for "input_size".  Reduce this if you are experiencing
          RAM issues. Also, you can use the "--split_data" option to divide the data by the
          specified amount and train the model on each set. Doing this will allow for a higher
          input_size setting (more accurate results).
        
        --training_mode=0   Speed training (default)
        --training_mode=1   Accuracy training
        --training_mode=2   Extended training (set max_epochs as desired, for example 50+)
    '''

    name = args.name
    if not os.path.exists('models/'+name):
        os.makedirs('models/'+name)
    else:
        print("A model folder with the same name already exists. Please choose a new name.")
        return

    train_mode = args.training_mode     # 0 = speed training, 
                                        # 1 = accuracy training 
                                        # 2 = extended training
    batch_size = args.batch_size 
    test_size = 0.2
    epochs = args.max_epochs
    input_size = args.input_size

    # TRAINING MODE
    if train_mode == 0:         # Speed Training
        learning_rate = 0.01
        conv1d_strides = 12
        conv1d_1_strides = 12   
        conv1d_filters = 4
        hidden_units = 24
    elif train_mode == 1:       # Accuracy Training (~10x longer than Speed Training)
        learning_rate = 0.01 
        conv1d_strides = 4
        conv1d_1_strides = 4  
        conv1d_filters = 36
        hidden_units= 64
    else:                       # Extended Training (~60x longer than Accuracy Training)
        learning_rate = 0.0005 
        conv1d_strides = 3
        conv1d_1_strides = 3
        conv1d_filters = 36
        hidden_units= 96

    conv1d_KS = conv1d_strides 
    conv1d_1_KS = conv1d_1_strides
    # Create Sequential Model ###########################################
    clear_session()
    model = Sequential()
    model.add(Conv1D(conv1d_filters, conv1d_KS, strides=conv1d_strides, activation=None, padding='same',input_shape=(input_size,1)))
    model.add(Conv1D(conv1d_filters, conv1d_1_KS, strides=conv1d_1_strides, activation=None, padding='same'))

    model.add(LSTM(hidden_units, input_shape=(input_size,1)))
    model.add(Dense(1, activation=None))
    model.compile(optimizer=Adam(learning_rate=learning_rate), loss=error_to_signal, metrics=[error_to_signal])
    print(model.summary())

    # Load and Preprocess Data ###########################################
    
    in_rate, stereo_data = wavfile.read(args.in_file)
    
    # If a second wav file is provided, assume each file is mono and load data
    if args.out_file != "":
        out_rate, out_data = wavfile.read(args.out_file)
        in_data = stereo_data
    # Else, use the stereo wav file channel 1 and channel 2 for training data
    else:
        in_data = stereo_data.T[0]
        out_data = stereo_data.T[1]

    X_all = in_data.astype(np.float32).flatten()  
    X_all = normalize(X_all).reshape(len(X_all),1)   
    y_all = out_data.astype(np.float32).flatten() 
    y_all = normalize(y_all).reshape(len(y_all),1)   

    # If splitting the data for training, do this part
    if args.split_data > 1:
        num_split = len(X_all) // args.split_data
        X = X_all[0:num_split*args.split_data]
        y = y_all[0:num_split*args.split_data]
        X_data = np.split(X, args.split_data)
        y_data = np.split(y, args.split_data)

        # Perform training on each split dataset
        for i in range(len(X_data)):
            print("\nTraining on split data " + str(i+1) + "/" +str(len(X_data)))
            X_split = X_data[i]
            y_split = y_data[i]

            y_ordered = y_split[input_size-1:] 

            indices = np.arange(input_size) + np.arange(len(X_split)-input_size+1)[:,np.newaxis] 
            X_ordered = tf.gather(X_split,indices) 

            shuffled_indices = np.random.permutation(len(X_ordered)) 
            X_random = tf.gather(X_ordered,shuffled_indices)
            y_random = tf.gather(y_ordered, shuffled_indices)

            # Train Model ###################################################
            model.fit(X_random,y_random, epochs=epochs, batch_size=batch_size, validation_split=0.2)  


        model.save('models/'+name+'/'+name+'.h5')


    # If training on the full set of input data in one run, do this part
    else:
        y_ordered = y_all[input_size-1:] 

        indices = np.arange(input_size) + np.arange(len(X_all)-input_size+1)[:,np.newaxis] 
        X_ordered = tf.gather(X_all,indices) 

        shuffled_indices = np.random.permutation(len(X_ordered)) 
        X_random = tf.gather(X_ordered,shuffled_indices)
        y_random = tf.gather(y_ordered, shuffled_indices)

        # Train Model ###################################################
        model.fit(X_random,y_random, epochs=epochs, batch_size=batch_size, validation_split=test_size)    

        model.save('models/'+name+'/'+name+'.h5', include_optimizer=False)

    # Run Prediction #################################################
    print("Running prediction..")

    # Get the last 20% of the wav data to run prediction and plot results
    y_the_rest, y_last_part = np.split(y_all, [int(len(y_all)*.8)])
    x_the_rest, x_last_part = np.split(X_all, [int(len(X_all)*.8)])
    y_test = y_last_part[input_size-1:] 
    indices = np.arange(input_size) + np.arange(len(x_last_part)-input_size+1)[:,np.newaxis] 
    X_test = tf.gather(x_last_part,indices) 

    prediction = model.predict(X_test, batch_size=batch_size)

    save_wav('models/'+name+'/y_pred.wav', prediction)
    save_wav('models/'+name+'/x_test.wav', x_last_part)
    save_wav('models/'+name+'/y_test.wav', y_test)

    # Add additional data to the saved model (like input_size, strides)
    filename = 'models/'+name+'/'+name+'.h5'
    f = h5py.File(filename, 'a')
    grp = f.create_group("info")
    dset = grp.create_dataset("input_size", (1,), dtype='int16')
    dset2 = grp.create_dataset("conv1d_stride", (1,), dtype='int16')
    dset3 = grp.create_dataset("conv1d_1_stride", (1,), dtype='int16')
    dset[0] = input_size
    dset2[0] = conv1d_strides
    dset3[0] = conv1d_1_strides

    f.close()


    # Generate json model ################################
    filename = 'models/'+name+'/'+ args.name +'.h5'
    #json_filename = 'models/'+name+'/'+ args.name
    json_filename = args.name
    f = h5py.File(filename, 'r')
   
    # Load the model data
    data = {}
    for layer in f["model_weights"].keys():
        if layer not in data.keys():
            data[layer] = {}
        for item in f["model_weights"][layer][layer]:
            if item not in data[layer].keys():
                data[layer][item] = {}
                try:
                    data[layer][item] = f["model_weights"][layer][layer][item][:].tolist()
                except:
                    data[layer]["kernel:0"] = f["model_weights"][layer][layer][item]["kernel:0"][:].tolist()
                    data[layer]["bias:0"] = f["model_weights"][layer][layer][item]["bias:0"][:].tolist()
            else:
                try:
                    data[layer][item] = f["model_weights"][layer][layer][item][:].tolist()
                except:
                    data[layer]["kernel:0"] = f["model_weights"][layer][layer][item]["kernel:0"][:].tolist()
                    data[layer]["bias:0"] = f["model_weights"][layer][layer][item]["bias:0"][:].tolist()

    input_size = f["info"]["input_size"][0]
    conv1d_strides = f["info"]["conv1d_stride"][0]
    conv1d_1_strides = f["info"]["conv1d_1_stride"][0]
    data['input_size'] = int(input_size)
    print("input_size: ",input_size)
    data['conv1d_stride'] = int(conv1d_strides)
    data['conv1d_1_stride'] = int(conv1d_1_strides)
    f.close()

    with open(json_filename + ".json", 'w') as outfile:
        json.dump(data, outfile)
    print("SmartAmpPro model generated: ", json_filename + ".json")

    # Create Analysis Plots ###########################################
    if args.create_plots == 1:
        print("Plotting results..")
        import plot

        plot.analyze_pred_vs_actual({   'output_wav':'models/'+name+'/y_test.wav',
                                            'pred_wav':'models/'+name+'/y_pred.wav', 
                                            'input_wav':'models/'+name+'/x_test.wav',
                                            'model_name':name,
                                            'show_plots':1,
                                            'path':'models/'+name
                                        })

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("in_file")
    parser.add_argument("name")
    parser.add_argument("--out_file", type=str, default="")
    parser.add_argument("--training_mode", type=int, default=0)
    parser.add_argument("--batch_size", type=int, default=4096)
    parser.add_argument("--max_epochs", type=int, default=3)
    parser.add_argument("--create_plots", type=int, default=1)
    parser.add_argument("--input_size", type=int, default=120)
    parser.add_argument("--split_data", type=int, default=6)
    args = parser.parse_args()
    main(args)
