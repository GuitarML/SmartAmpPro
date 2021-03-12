# SmartAmpPro

Guitar plugin made with JUCE that uses neural network models to emulate real world hardware.
This plugin uses a LSTM model to recreate the sound of real amps and pedals. You can record
samples and train models from the plugin. Tone models are saved in .json format. Model training
is accomplished using Tensorflow/Keras. The main improvement from the original SmartAmp is that
training takes less than five minutes on CPU (vs. 8 hours on GPU) for comparable sound quality.
Training has also been integrated into the plugin. For best sound, use with additional Reverb/IR.

An alternate way to train models for SmartAmpPro is through [Colab](https://colab.research.google.com/notebooks/intro.ipynb#recent=true). 
Upload the "train_colab.ipynb" or "train_colab_mse.ipynb" script and .wav file(s) to Colab, then follow the instructions in the notes. 
No installs necessary, only a Google account to train models in the cloud.

Hear the audio demo on [Youtube](https://youtu.be/1njl9wqUG0M)!

![app](https://github.com/GuitarML/SmartAmpPro/blob/main/resources/plugin.jpg)

## Installing the plugin

1. Download pre-release plugin (Windows 10, Mac, Ubuntu Linux) [here](https://github.com/GuitarML/SmartAmpPro/releases/tag/v0.1-alpha)
2. Copy to your DAW's VST directory

## Installing python dependencies

The python dependencies for training tones are listed in the ```requirements.txt``` file. It's 
recommended to use the "pip" package manager to install these dependencies. Pip is included in 
Python3.4 and up. Python3.6 and Python3.8 were used during development and either version is
recommended for SmartAmpPro.  Tensorflow2.4 does not support Python3.9 as of 1/22/20.

```
#In the command terminal:

pip install -r requirements.txt

# OR run the included "install_requirements" script (.bat for Windows, .sh for Mac/Linux)
```

Note: You can still use the plugin without installing the python dependencies, but the 
      "Train Tone" button will not work.

## Using the plugin

All .json models, python training files, and .wav samples created from the plugin will be saved to the
"userApplicationDataDirectory" defined by Juce for the OS. There is no automatic data cleanup of this 
directory. If you modify the "train.py" file and want to revert to the original, remove the the .py file 
and it will be re-installed the next time you open the plugin.

userApplicationDataDirectoy Locations (default location for models, python scripts, and .wav samples for training):
```
Windows 10: "C:/Users/<username>/AppData/Roaming/GuitarML/SmartAmpPro"
OSX (default): /Users/<username>/Library/GuitarML/SmartAmpPro
OSX (for Garageband, substitute appropriate version): 
    /Users/<username>/Library/Containers/com.apple.garageband10/Data/Library/GuitarML/SmartAmpPro
Linux: /home/<username>/.config/GuitarML/SmartAmpPro
```
```
#The following directories are created in the SmartAmpPro folder:
/captures		# Where all recorded .wav files are saved
/install		# Where Python dependency installation scripts are stored
/models			# The model output of the training scripts (Keras .h5 model, generated .wav samples, generated plots)
/tones			# Where all .json tone files are saved
/training		# Where python training scripts are stored
```

### "Import Tone" and "Export Tone" buttons
Export or import tone files (with .json extension) to and from the plugin. Click the "Add Tone" button to open up 
a file select dialog box. Select your .json tone file (or multiple files) to import them into the plugin. 
This simply copies the tone file to your SmartAmpPro directory defined above. The tone is now an option
in the drop down box in the plugin. The "Export Tone" button copies the tone files from the SmartAmpPro
directory to the chosen location.

Note: The original SmartAmp/PedalNetRT .json files are not compatible with this plugin because it is a
different machine learning model. Only use tone files trained from SmartAmpPro.

### "Start Capture" button:

1. Click the "Start Capture" button and enter a model name to save. Ensure this name contains no spaces. 
2. After saving, the plugin will begin a 10 second coundown followed by a 3 minute recording session.
3. The plugin will automatically record from Channels 1 and 2, with the input expected on channel 1, and the target
   expected on channel 2. This assumes the user has an Audio Interface connected with at least two channels.
   (all testing was done on a Focusrite Scarlett 2i2)
4. Once the three minutes are over, recording will automatically stop and the .wav file will be saved.
   Note: This saves the two channels to a single stereo .wav file, with the input on the left (channel 1), and target on the right (channel 2)
   Note: The playing prompts such as "Play loud!" that appear while training are suggestions, and don't need to be
          followed exactly to successfully train a model.

### "Train Tone" button:

The Python dependencies from the ```requirements.txt``` file must be installed on your system (not a virtual env)
for the tone training to work.

1. Click the "Train Model" button and select the .wav file to train from.
    (These are in the "userApplicationDataDirectoy" mentioned above)
2. If you have two separate .wav files (float32, mono, no extra data) then you can select both from the
   dialog box. Ensure that the input comes first alphabetically. For example:
	"Input.wav", "Output.wav" works, but  "zInput.wav" and "Output.wav" would use the wrong file as input vs. output.
3. On Windows, a terminal window should pop up and execute the training script. Mac suppresses the terminal and runs in the background.
   The percent complete status should update in the plugin. If it remains at 0, or seems to get stuck, stop the training by clicking
   the button. 
   Note: To troubleshoot, navigate to the SmartAmpPro directory, open a cmd prompt and manually run train.py with the appropriate command.
   ```
   # For single stereo wav file recorded from SmartAmpPro:
   "python train.py input.wav model_name"
   
      # For two mono wav files:
   "python train.py input.wav model_name --out_file=output.wav"
   ```
4. Analysis plots are saved to the SmartAmpPro/models/<model_name> directory, along with sample .wav files and the Keras .h5 model file.
5. After training has completed (should take 5 minutes or less), you will now have a newly created model to select in the plugin drop down box.
	
Note: Currently clicking "Stop Training" stops the internal plugin processes for training, but the separate training application will keep running until complete.

Note: The guitar effect will keep running during training, but due to the extra CPU usage it may be better to switch the effect off until training is finished. 

Note: You can modify the train.py script to test different parameters, but it may produce undesired results in the plugin. Recommended to only
      modify the number of epochs, learning rate, or the number of hidden units of the LSTM layer. 

See the [GuitarLSTM](https://github.com/GuitarML/GuitarLSTM) repo for more information on how training works, and tips for creating sample recordings.

## Build Instructions

1. Clone or download this repository.
2. Download and install [JUCE](https://juce.com/) This project uses the "Projucer" application from the JUCE website. 
3. Download and extract [NumCpp](https://github.com/dpilger26/NumCpp) This is a c++ implementation of the Python Numpy library.
4. Download and extract [Boost](https://www.boost.org/)  (NumCpp requires Boost)
5. Download and extract: [json](https://github.com/nlohmann/json) Json for c++.
6. Open the SmartAmpPro.jucer file and in the appropriate Exporter Header Search Path field, enter the appropriate include paths.
   For example:

```
  	<full-path-to>/json-develop/include
	<full-path-to>/NumCpp-master/include
	<full-path-to>/boost_1_75_0/boost_1_75_0
```
7. Build SmartAmpPro from the Juce Projucer application. 

Dev Note: The above dependencies were chosen to facilitate rapid prototyping. It is possible to accomplish the same
	thing using only Juce and the standard c++ library.

Note: Make sure to build in Release mode unless actually debugging. Debug mode will not keep up with real time playing.


## Future Work

This plugin is designed to showcase the speed at which the guitar models can be trained.  Using models with larger
parameters can improve accuracy at the cost of longer training time. The c++ inference can also be rewritten to be 
more efficient and handle larger models at real time speeds. Future work will focus on improving the c++ inference 
code to handle larger models, and optimizing the LSTM model to handle more complex signals (high gain/distortion).

The current model training takes a "snapshot" of a particular amp/pedal/rig. It is possible to use multiple recordings
of different knob settings and train a single model that can interpolate between the settings. For example, a model
with an adjustable gain parameter, to more accurately simulate the behavior of the system. This will also be a focus
of future work.

Other possibilities: <br>
    - Online model sharing and integration with plugin <br>
    - Hardware integration as an actual guitar pedal (RaspberryPi, Arduino, Beaglebone)<br>
