/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "myLookAndFeel.h"
#include <stdlib.h>

//==============================================================================
/**
*/
class SmartAmpProAudioProcessorEditor  : public AudioProcessorEditor,
                                       private Button::Listener,
                                       private Slider::Listener,
                                       private Timer
                                
{
public:
    SmartAmpProAudioProcessorEditor (SmartAmpProAudioProcessor&);
    ~SmartAmpProAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SmartAmpProAudioProcessor& processor;

    // Amp Widgets
    Slider ampPresenceKnob;
    Slider ampBassKnob;
    Slider ampMidKnob;
    Slider ampTrebleKnob;
    Slider ampGainKnob;
    Slider ampMasterKnob;
    Slider progressCircle;
    ImageButton ampOnButton;
    ImageButton ampLED;
    ComboBox modelSelect;
    
    // LookandFeels 
    myLookAndFeel ampSilverKnobLAF;
    myLookAndFeel statusKnob;

    //Image background;
    int current_background = 1;
    Image background_smp_on = ImageCache::getFromMemory(BinaryData::smp_on_jpg, BinaryData::smp_on_jpgSize);
    Image background_smp_off = ImageCache::getFromMemory(BinaryData::smp_off_jpg, BinaryData::smp_off_jpgSize);
    Image background_orig_on = ImageCache::getFromMemory(BinaryData::original_on_jpg, BinaryData::original_on_jpgSize);
    Image background_orig_off = ImageCache::getFromMemory(BinaryData::original_off_jpg, BinaryData::original_off_jpgSize);

    int training = 0;
    File test_file;
    File model_folder;

    TextButton recordButton;
    TextButton trainButton;
    TextButton exportButton;
    Label timerLabel;
    Label helpLabel;
    TextButton loadButton;
    juce::String fname;
    virtual void buttonClicked(Button* button) override;
    void modelSelectChanged();
    void loadButtonClicked();
    void trainButtonClicked();
    void exportButtonClicked();
    virtual void sliderValueChanged(Slider* slider) override;
    void ampOnButtonClicked();
    void recordButtonClicked();
    void ledButtonClicked();
    void setSkin();

    void setTrainingStatus(int status);

    // Recording Timer
    String record_file = "";
    void timerCallback() override;
    void timer_start();
    void timer_stop();
    int t = 190;
    String seconds = "10";
    String minutes = "";
    float progressValue = 0.0;

    void resetImages();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmartAmpProAudioProcessorEditor)
};
