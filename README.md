# SmartAmpPro

Guitar plugin made with JUCE that uses neural network models to emulate real world hardware.
This plugin uses a stateless LSTM model to recreate the sound of real amps and pedals. You can record
samples and train models from the plugin. Tone models are saved in .json format. Model training
is accomplished using Tensorflow/Keras in a separate script (as of Version 1.0). The model training
is very fast compared to PedalNetRT or Automated-GuitarAmpModelling, but not as accurate.
For best sound, use with additional Reverb/IR.

Train models for SmartAmpPro is through [Colab](https://colab.research.google.com/notebooks/intro.ipynb#recent=true). 
Upload the "train_colab.ipynb" or "train_colab_mse.ipynb" script and .wav file(s) to Colab, then follow the instructions in the notes. 
No dependency installs necessary, only a Google account to train models in the cloud.

Hear the audio demo on [Youtube](https://youtu.be/1njl9wqUG0M)!

![app](https://github.com/GuitarML/SmartAmpPro/blob/main/resources/plugin.jpg)

## Installing the plugin

1. Download the appropriate plugin installer (Windows, Mac, Linux) from the [Releases](https://github.com/GuitarML/SmartAmpPro/releases) page.
2. Run the installer and follow the instructions. May need to reboot to allow your DAW to recognize the new plugin.


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


Note: Only use tone files trained from the SmartAmpPro Colab training script. The NeuralPi and SmartAmp/SmartPedal models are not compatible.


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



