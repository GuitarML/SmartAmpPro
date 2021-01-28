/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <fstream>

//==============================================================================
SmartAmpProAudioProcessor::SmartAmpProAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
    )

#endif
{
    setupDataDirectories();
    installPythonScripts();
    resetDirectory(userAppDataDirectory_tones);
    if (jsonFiles.size() > 0) {
        loadConfig(jsonFiles[current_model_index]);
    }
}

SmartAmpProAudioProcessor::~SmartAmpProAudioProcessor()
{
}

//==============================================================================
const String SmartAmpProAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SmartAmpProAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SmartAmpProAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SmartAmpProAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SmartAmpProAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SmartAmpProAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SmartAmpProAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SmartAmpProAudioProcessor::setCurrentProgram (int index)
{
}

const String SmartAmpProAudioProcessor::getProgramName (int index)
{
    return {};
}

void SmartAmpProAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SmartAmpProAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SmartAmpProAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SmartAmpProAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void SmartAmpProAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    // Setup Audio Data
    const int numSamples = buffer.getNumSamples();
    const int numInputChannels = getTotalNumInputChannels();

    // Amp =============================================================================
    if (amp_state == 1) {
        //    EQ (Presence, Bass, Mid, Treble)
        eq4band.process(buffer.getReadPointer(0), buffer.getWritePointer(0), midiMessages, numSamples, numInputChannels);

        buffer.applyGain(ampDrive);

		// Apply LSTM model
        if (model_loaded == 1) {
            LSTM.process(buffer.getReadPointer(0), buffer.getWritePointer(0), numSamples);
        }

        //    Master Volume 
        buffer.applyGain(ampMaster);

    }
    
    for (int ch = 1; ch < buffer.getNumChannels(); ++ch)
        buffer.copyFrom(ch, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool SmartAmpProAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SmartAmpProAudioProcessor::createEditor()
{
    return new SmartAmpProAudioProcessorEditor (*this);
}

//==============================================================================
void SmartAmpProAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SmartAmpProAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void SmartAmpProAudioProcessor::loadConfig(File configFile)
{
    this->suspendProcessing(true);
    model_loaded = 1;
    String path = configFile.getFullPathName();
    char_filename = path.toUTF8();
    loader.load_json(char_filename);
    LSTM.setParams(loader.hidden_size, loader.conv1d_kernel_size, loader.conv1d_1_kernel_size,
        loader.conv1d_num_channels, loader.conv1d_1_num_channels, loader.conv1d_bias_nc,
        loader.conv1d_1_bias_nc, loader.conv1d_kernel_nc, loader.conv1d_1_kernel_nc,
        loader.lstm_bias_nc, loader.lstm_kernel_nc,
        loader.dense_bias_nc, loader.dense_kernel_nc, loader.input_size_loader, loader.conv1d_stride_loader, loader.conv1d_1_stride_loader);

    this->suspendProcessing(false);
}

void SmartAmpProAudioProcessor::resetDirectory(const File& file)
{
    jsonFiles.clear();
    if (file.isDirectory())
    {
        juce::Array<juce::File> results;
        file.findChildFiles(results, juce::File::findFiles, false, "*.json");
        for (int i = results.size(); --i >= 0;)
            jsonFiles.push_back(File(results.getReference(i).getFullPathName()));
    }
}

void SmartAmpProAudioProcessor::addDirectory(const File& file)
{
    if (file.isDirectory())
    {
        juce::Array<juce::File> results;
        file.findChildFiles(results, juce::File::findFiles, false, "*.json");
        for (int i = results.size(); --i >= 0;)
            jsonFiles.push_back(File(results.getReference(i).getFullPathName()));
    }
}

void SmartAmpProAudioProcessor::setupDataDirectories()
{
    // User app data directory
    File userAppDataTempFile = userAppDataDirectory.getChildFile("tmp.pdl");

    File userAppDataTempFile_captures = userAppDataDirectory_captures.getChildFile("tmp.pdl");
    File userAppDataTempFile_install = userAppDataDirectory_install.getChildFile("tmp.pdl");
    File userAppDataTempFile_tones = userAppDataDirectory_tones.getChildFile("tmp.pdl");
    File userAppDataTempFile_training = userAppDataDirectory_training.getChildFile("tmp.pdl");

    // Create (and delete) temp file if necessary, so that user doesn't have
    // to manually create directories
    if (!userAppDataDirectory.exists()) {
        userAppDataTempFile.create();
    }
    if (userAppDataTempFile.existsAsFile()) {
        userAppDataTempFile.deleteFile();
    }

    if (!userAppDataDirectory_captures.exists()) {
        userAppDataTempFile_captures.create();
    }
    if (userAppDataTempFile_captures.existsAsFile()) {
        userAppDataTempFile_captures.deleteFile();
    }

    if (!userAppDataDirectory_install.exists()) {
        userAppDataTempFile_install.create();
    }
    if (userAppDataTempFile_install.existsAsFile()) {
        userAppDataTempFile_install.deleteFile();
    }

    if (!userAppDataDirectory_tones.exists()) {
        userAppDataTempFile_tones.create();
    }
    if (userAppDataTempFile_tones.existsAsFile()) {
        userAppDataTempFile_tones.deleteFile();
    }

    if (!userAppDataDirectory_training.exists()) {
        userAppDataTempFile_training.create();
    }
    if (userAppDataTempFile_training.existsAsFile()) {
        userAppDataTempFile_training.deleteFile();
    }

    // Add the tones directory and update tone list
    addDirectory(userAppDataDirectory_tones);
}

void SmartAmpProAudioProcessor::installPythonScripts()
//====================================================================
// Description: Checks that the python scripts and default tones
//  are installed to the SmartAmpPro directory, and if not, 
//  copy them from the binary data in the plugin to that directory.
//
//====================================================================
{
    // Python model training scripts
    File train_script = userAppDataDirectory_training.getFullPathName() + "/train.py";
    File plot_script = userAppDataDirectory_training.getFullPathName() + "/plot.py";

    // Default tones
    File ts9_tone = userAppDataDirectory_tones.getFullPathName() + "/CompressedOverdrive.json";
    File heavy_tone = userAppDataDirectory_tones.getFullPathName() + "/Heavy.json";
    File bjclean_tone = userAppDataDirectory_tones.getFullPathName() + "/TubeClean.json";
    File bjdirty_tone = userAppDataDirectory_tones.getFullPathName() + "/TubeDirty.json";
    File cbs_tone = userAppDataDirectory_tones.getFullPathName() + "/ClearBlueSky.json";

    // Python dependency installation scripts
    File install_req_windows = userAppDataDirectory_install.getFullPathName() + "/install_requirements.bat";
    File install_req_mac = userAppDataDirectory_install.getFullPathName() + "/install_requirements.sh";
    File req = userAppDataDirectory_install.getFullPathName() + "/requirements.txt";


    if (train_script.existsAsFile() == false) {
        std::string string_command = train_script.getFullPathName().toStdString();
        const char* char_train_script = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_train_script);
        myfile << BinaryData::train_py;

        myfile.close();
    }
    if (plot_script.existsAsFile() == false) {
        std::string string_command = plot_script.getFullPathName().toStdString();
        const char* char_plot_script = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_plot_script);
        myfile << BinaryData::plot_py;

        myfile.close();
    }
    if (ts9_tone.existsAsFile() == false) {
        std::string string_command = ts9_tone.getFullPathName().toStdString();
        const char* char_ts9_tone = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_ts9_tone);
        myfile << BinaryData::CompressedOverdrive_json;

        myfile.close();
    }
    if (heavy_tone.existsAsFile() == false) {
        std::string string_command = heavy_tone.getFullPathName().toStdString();
        const char* char_heavy_tone = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_heavy_tone);
        myfile << BinaryData::Heavy_json;

        myfile.close();
    }
    if (bjclean_tone.existsAsFile() == false) {
        std::string string_command = bjclean_tone.getFullPathName().toStdString();
        const char* char_bjclean = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_bjclean);
        myfile << BinaryData::TubeClean_json;

        myfile.close();
    }
    if (bjdirty_tone.existsAsFile() == false) {
        std::string string_command = bjdirty_tone.getFullPathName().toStdString();
        const char* char_bjdirty = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_bjdirty);
        myfile << BinaryData::TubeDirty_json;

        myfile.close();
    }
    if (cbs_tone.existsAsFile() == false) {
        std::string string_command = cbs_tone.getFullPathName().toStdString();
        const char* char_cbs = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_cbs);
        myfile << BinaryData::ClearBlueSky_json;

        myfile.close();
    }
    if (install_req_windows.existsAsFile() == false) {
        std::string string_command = install_req_windows.getFullPathName().toStdString();
        const char* char_windows = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_windows);
        myfile << BinaryData::install_requirements_bat;

        myfile.close();
    }
    if (install_req_mac.existsAsFile() == false) {
        std::string string_command = install_req_mac.getFullPathName().toStdString();
        const char* char_mac = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_mac);
        myfile << BinaryData::install_requirements_sh;

        myfile.close();
    }
    if (req.existsAsFile() == false) {
        std::string string_command = req.getFullPathName().toStdString();
        const char* char_req = &string_command[0];

        std::ofstream myfile;
        myfile.open(char_req);
        myfile << BinaryData::requirements_txt;

        myfile.close();
    }
}

float SmartAmpProAudioProcessor::convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max)
{
    float b = log(y_max / y_min) / (x_max - x_min);
    float a = y_max / exp(b * x_max);
    float converted_value = a * exp(b * in_value);
    return converted_value;
}

void SmartAmpProAudioProcessor::set_ampDrive(float db_ampDrive)
{
    ampDrive = decibelToLinear(db_ampDrive);
    ampGainKnobState = db_ampDrive;
}

void SmartAmpProAudioProcessor::set_ampMaster(float db_ampMaster)
{
    ampMasterKnobState = db_ampMaster;
    if (db_ampMaster == -36.0) {
        ampMaster = decibelToLinear(-100.0);
    } else {
        ampMaster = decibelToLinear(db_ampMaster);
    }
}

void SmartAmpProAudioProcessor::set_ampEQ(float bass_slider, float mid_slider, float treble_slider, float presence_slider)
{
    eq4band.setParameters(bass_slider, mid_slider, treble_slider, presence_slider);

    ampPresenceKnobState = presence_slider;
}

float SmartAmpProAudioProcessor::decibelToLinear(float dbValue)
{
    return powf(10.0, dbValue/20.0);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SmartAmpProAudioProcessor();
}
