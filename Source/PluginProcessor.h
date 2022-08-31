/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <nlohmann/json.hpp>
#include "NumCpp.hpp"
#include "lstm.h"
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Eq4Band.h"

#define GAIN_ID "gain"
#define GAIN_NAME "Gain"
#define BASS_ID "bass"
#define BASS_NAME "Bass"
#define MID_ID "mid"
#define MID_NAME "Mid"
#define TREBLE_ID "treble"
#define TREBLE_NAME "Treble"
#define PRESENCE_ID "presence"
#define PRESENCE_NAME "Presence"
#define MASTER_ID "master"
#define MASTER_NAME "Master"

//==============================================================================
/**
*/
class SmartAmpProAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SmartAmpProAudioProcessor();
    ~SmartAmpProAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadConfig(File configFile);
    void setupDataDirectories();
    void installTones();
    
    // Overdrive Pedal
    //float convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max);

    // Amp
    //void set_ampDrive(float db_ampCleanDrive);
    //void set_ampMaster(float db_ampMaster);
    void set_ampEQ(float bass_slider, float mid_slider, float treble_slider, float presence_slider);

    //float decibelToLinear(float dbValue);

    void addDirectory(const File& file);
    void resetDirectory(const File& file);
    std::vector<File> jsonFiles;
    File currentDirectory = File::getCurrentWorkingDirectory().getFullPathName();
    File userAppDataDirectory = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(JucePlugin_Manufacturer).getChildFile(JucePlugin_Name);
    File userAppDataDirectory_tones = userAppDataDirectory.getFullPathName() + "/tones";

    // Pedal/amp states
    int amp_state = 1; // 0 = off, 1 = on
    int custom_tone = 0; // 0 = custom tone loaded, 1 = default channel tone
    File loaded_tone;
    juce::String loaded_tone_name;
    const char* char_filename = "";
    int model_loaded = 0;
    int current_model_index = 0;

    File folder = File::getSpecialLocation(File::userDesktopDirectory);

    ModelLoader loader;
    lstm LSTM;
    //AudioRecordingDemo audio_recorder;
    //int recording = 0;
    int skin = 0;

    AudioProcessorValueTreeState treeState;

private:
    Eq4Band eq4band; // Amp EQ

    std::atomic<float>* bassParam = nullptr;
    std::atomic<float>* midParam = nullptr;
    std::atomic<float>* trebleParam = nullptr;
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* gainParam = nullptr;
    std::atomic<float>* presenceParam = nullptr;
    std::atomic<float>* masterParam = nullptr;

    var dummyVar;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmartAmpProAudioProcessor)
};
