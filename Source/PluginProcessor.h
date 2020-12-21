/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

// Include the following:
//      C:\Users\KBloemer\Desktop\Archive\AudioFile-master\AudioFile-master
//      C:\Users\KBloemer\Desktop\Archive\json-develop\json-develop\include
//      C:\Users\KBloemer\Desktop\Archive\FunctionalPlus-master\include
//      C:\Users\KBloemer\Desktop\Archive\eigen-3.3.7\eigen-3.3.7
//      C:\Users\KBloemer\Desktop\Archive\frugally-deep-master\include
#include <nlohmann/json.hpp>
#include "NumCpp.hpp"
#include "lstm.h"
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Eq4Band.h"

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

    void loadConfigAmp();
    void loadConfig(File configFile);

    // Overdrive Pedal
    float convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max);

    // Amp
    void set_ampDrive(float db_ampCleanDrive);
    void set_ampMaster(float db_ampMaster);
    void set_ampEQ(float bass_slider, float mid_slider, float treble_slider, float presence_slider);

    float decibelToLinear(float dbValue);

    // Pedal/amp states
    int amp_state = 1; // 0 = off, 1 = on
    int custom_tone = 0; // 0 = custom tone loaded, 1 = default channel tone
    File loaded_tone;
    juce::String loaded_tone_name;

    // Amp knob states
    float ampPresenceKnobState = 0.0;
    float ampBassKnobState = 0.0;
    float ampMidKnobState = 0.0;
    float ampTrebleKnobState = 0.0;
    float ampGainKnobState = 10.0;
    float ampMasterKnobState = -12.0;

    int input_size = 120; //TODO read from file
    //const fdeep::model model = fdeep::load_model("C:/Users/rache/Desktop/func_ts9_in100.json");

    ModelLoader loader;
    lstm LSTM;

    nc::NdArray<float> input = nc::random::rand<float>(nc::Shape(input_size, 1));
    
    std::vector<float> new_buffer;

    std::vector<float> old_buffer;
    std::vector<std::vector<float>> data;

    std::vector<std::vector<float>> set_data(const float **inputData, int numSamples, int input_size);
    void check_buffer(int numSamples, int input_size);

private:
    Eq4Band eq4band; // Amp EQ


    // Amp
    float ampDrive = 1.0;
    float ampMaster = 1.0;

    var dummyVar;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmartAmpProAudioProcessor)
};
