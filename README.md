# SmartAmpPro

Guitar plugin made with JUCE that uses neural network models to emulate real world hardware.
This plugin uses a LSTM model to recreate the sound of real amps and pedals. You can record
samples and train models from the plugin. Tone models are saved in .json format. Model training
is accomplished using Tensorflow/Keras in a separate script (as of Version 1.0). The main improvement from the original SmartAmp is that
training takes less than five minutes on CPU (vs. 8 hours on GPU) for comparable sound quality.
Training has also been integrated into the plugin. For best sound, use with additional Reverb/IR.

Train models for SmartAmpPro is through [Colab](https://colab.research.google.com/notebooks/intro.ipynb#recent=true). 
Upload the "train_colab.ipynb" or "train_colab_mse.ipynb" script and .wav file(s) to Colab, then follow the instructions in the notes. 
No installs necessary, only a Google account to train models in the cloud.

Hear the audio demo on [Youtube](https://youtu.be/1njl9wqUG0M)!

![app](https://github.com/GuitarML/SmartAmpPro/blob/main/resources/plugin.jpg)

## Installing the plugin

1. Download pre-release plugin (Windows 10, Mac, Ubuntu Linux) [here](https://github.com/GuitarML/SmartAmpPro/releases/tag/v0.1-alpha)
2. Copy to your DAW's VST directory
3. For additional tones created by the community, download the [TonePack](https://github.com/GuitarML/SmartAmpPro/releases/tag/v1.0)


userApplicationDataDirectoy Locations (default location for models):
```
Windows 10: "C:/Users/<username>/AppData/Roaming/GuitarML/SmartAmpPro"
OSX (default): /Users/<username>/Library/GuitarML/SmartAmpPro
OSX (for Garageband, substitute appropriate version): 
    /Users/<username>/Library/Containers/com.apple.garageband10/Data/Library/GuitarML/SmartAmpPro
Linux: /home/<username>/.config/GuitarML/SmartAmpPro
```
```
#The following directories are created in the SmartAmpPro folder:
/tones			# Where all .json tone files are saved
```


Note: The SmartAmp/PedalNetRT and NeuralPi/Automated-GuitarAmpModelling .json files are not compatible with this plugin because it is a
different machine learning model. Only use tone files trained from the SmartAmpPro Colab training script.


## Build Instructions

### Build with Cmake

```bash
# Clone the repository
$ git clone https://github.com/GuitarML/SmartAmpPro.git
$ cd SmartAmpPro

# initialize and set up submodules
$ git submodule update --init --recursive

# build with CMake
$ cmake -Bbuild
$ cmake --build build --config Release
```
The binaries will be located in `SmartAmpPro/build/SmartAmpPro_artefacts/`



