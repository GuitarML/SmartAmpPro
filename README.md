# SmartAmpPro

Guitar plugin made with JUCE that uses neural network models to emulate real world hardware.
This plugin uses a LSTM model to recreate the sound of real amps and pedals. You can record
samples and train models from the plugin. Tone models are saved in .json format. Model training
is accomplished using Tensorflow/Keras.

Developer Note: After performing listening tests between the SmartAmp(WaveNet model), and
SmartAmpPro(LSTM model) it was determined that the WaveNet produces a much more natural
sound, even though both models have a low error-to-signal ratio. Since the main goal of
using A.I. is to produce the highest quality sound, any future development will continue
using WaveNet, or a different A.I. model than the one used here ([GuitarLSTM](https://github.com/GuitarML/GuitarLSTM)). 


## Installing the plugin

1. Download plugin (Windows 10 only) [here]
2. Copy to your DAW's VST directory

## Using the plugin

On Windows 10, the SmartAmpPro creates a "C:\Users\<user>\AppData\Roaming\GuitarML\SmartAmpPro" directory. 
All .json models, associated training files, and .wav samples created from the plugin will be saved here. 
When using the "Add Tone" button, the chosen .json file will be copied here. There is no automatic data cleanup
of this directory. If you modify the "train.py" file and want to revert to the original, remove the the .py file and 
it will be re-installed the next time you open the plugin.


### Capturing .wav samples for model training:

1. Click the "Start Capture" button to and enter a model name to save. 
2. After saving, the plugin wil begin a 10 second coundown followed by a 3 minute recording session.
3. The plugin will automatically record from Channels 1 and 2, with the input expected on channel 1, and the target
   expected on channel 2. This assumes the user has an Audio Interface connected with at least two channels.
   (all testing was done on a Focusrite Scarlett 2i2)
4. Once the three minutes are done, recording will automatically end.
5. This saves the two channels to a single stereo .wav file, with the input on the left (channel 1), and target on the right (channel 2)


### Creating .json models from recorded .wav samples.

In order to use the "Train Model" button, the following python libraries must be installed on your system (not a virtual environment)
-tensorflow (2.4 used in testing with python3.8) 
-matplotlib
-scipy
Note: Earlier versions of python may not work with latest tensorflow

To test these are installed correctly for SmartAmpPro, open a command prompt, run python and try to import tensorflow, matplotlib, and scipy.
The plugin simply makes a system call to python and runs the training script, so these need to be a system-wide installation on your computer.

1. Click the "Train Model" button and select the .wav file to train from.
    (These are in the "C:\Users\<user>\AppData\Roaming\GuitarML\SmartAmpPro" directory from the above step)
2. If you have two separate .wav files (float32, mono, no extra data) then you can select both from the
   dialog box. Ensure that the input comes first alphabetically. For example:
	"Input.wav", "Output.wav" works, but  "zInput.wav" and "Output.wav" would use the wrong file as input vs. output.
   Ensure that no model exists in the "models" directory with the same name as the target .wav file. The target .wav file
   name (or the stereo .wav filename from the above step) is used for the output .json model name.
3. A terminal window should pop up and execute the training script. If it appears and quickly disappears, check that your
model name doesn't already exist in the SmartAmpPro/models directory, or check that the python libraries were installed correctly.
To further troubleshoot, navigate to the SmartAmpPro directory, open a cmd prompt and manually run "python train.py <input.wav(s)> <name>"
4. After training has completed (should take 5 minutes or less), you will now have a newly created model to select in the plugin drop down box.

Note: You can modify the train.py script to test different parameters, but it may produce undesired results in the plugin.


## Build Instructions

1. Clone or download this repository.
2. Download and install [JUCE](https://juce.com/) This project uses the "Projucer" application from the JUCE website. 
3. Download and extract [NumCpp](https://github.com/dpilger26/NumCpp) This is a c++ implementation of the Python Numpy library.
4. Download and extract [Boost](https://www.boost.org/)  (NumCpp requires Boost)
5. Download and extract: [json] (https://github.com/nlohmann/json)
6. Open the SmartAmpPro.jucer file and in the appropriate Exporter Header Search Path field, enter the appropriate include paths.
   For example:

```
  	<full-path-to>/json-develop/include
	<full-path-to>/NumCpp-master/include
	<full-path-to>/boost_1_75_0/boost_1_75_0
```

Dev Note: The above dependencies were chosen to facilitate rapid prototyping. It is possible to accomplish the same
	thing using only Juce and the standard c++ library.

Note: Make sure to build in Release mode unless actually debugging. Debug mode will not keep up with real time playing.
