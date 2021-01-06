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
    ImageButton ampOnButton;
    ImageButton ampLED;
    ComboBox modelSelect;
    
    // LookandFeels 
    myLookAndFeel ampSilverKnobLAF;

    Image background;
    int current_background = 1;

    TextButton recordButton;
    TextButton trainButton;
    Label timerLabel;
    Label helpLabel;
    TextButton loadButton;
    Label modelLabel;
    juce::String fname;
    virtual void buttonClicked(Button* button) override;
    void modelSelectChanged();
    void loadButtonClicked();
    void trainButtonClicked();
    virtual void sliderValueChanged(Slider* slider) override;
    void ampOnButtonClicked();
    void recordButtonClicked();

    // Recording Timer
    String record_file = "";
    void timerCallback() override;
    void timer_start();
    void timer_stop();
    int t = 190;
    String seconds = "10";
    String minutes = "";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmartAmpProAudioProcessorEditor)
};
