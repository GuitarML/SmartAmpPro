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

//==============================================================================
/**
*/
class SmartAmpProAudioProcessorEditor  : public AudioProcessorEditor,
                                       private Button::Listener,
                                       private Slider::Listener
                                
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

    
    // LookandFeels 
    myLookAndFeel ampSilverKnobLAF;

    Image background;
    int current_background = 1;

    TextButton recordButton;
    TextButton loadButton;
    Label modelLabel;
    juce::String fname;
    virtual void buttonClicked(Button* button) override;
    void loadButtonClicked();
    virtual void sliderValueChanged(Slider* slider) override;
    void ampOnButtonClicked();
    void recordButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmartAmpProAudioProcessorEditor)
};
