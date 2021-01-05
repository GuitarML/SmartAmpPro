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

#include "AudioRecordingDemo.h"

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
    // Overdrive Pedal
    float convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max);

    // Amp
    void set_ampDrive(float db_ampCleanDrive);
    void set_ampMaster(float db_ampMaster);
    void set_ampEQ(float bass_slider, float mid_slider, float treble_slider, float presence_slider);

    float decibelToLinear(float dbValue);

    void addDirectory(const File& file);
    void resetDirectory(const File& file);
    std::vector<File> jsonFiles;
    File currentDirectory = File::getCurrentWorkingDirectory().getFullPathName();

    // Pedal/amp states
    int amp_state = 1; // 0 = off, 1 = on
    int custom_tone = 0; // 0 = custom tone loaded, 1 = default channel tone
    File loaded_tone;
    juce::String loaded_tone_name;
    const char* char_filename = "";
    int model_loaded = 0;

    // Amp knob states
    float ampPresenceKnobState = 0.0;
    float ampBassKnobState = 0.0;
    float ampMidKnobState = 0.0;
    float ampTrebleKnobState = 0.0;
    float ampGainKnobState = 10.0;
    float ampMasterKnobState = -12.0;

    ModelLoader loader;
    lstm LSTM;
    AudioRecordingDemo audio_recorder;
    int recording = 0;

private:
    Eq4Band eq4band; // Amp EQ


    // Amp
    float ampDrive = 1.0;
    float ampMaster = 1.0;

    var dummyVar;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmartAmpProAudioProcessor)
};
