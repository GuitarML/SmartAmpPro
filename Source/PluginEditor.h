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
    Label versionLabel;

    ImageButton ampOnButton;
    ImageButton ampLED;
    ComboBox modelSelect;
    
    // LookandFeels 
    myLookAndFeel ampSilverKnobLAF;

    //Image background;
    int current_background = 1;
    Image background_smp_on = ImageCache::getFromMemory(BinaryData::smp_on_jpg, BinaryData::smp_on_jpgSize);
    Image background_smp_off = ImageCache::getFromMemory(BinaryData::smp_off_jpg, BinaryData::smp_off_jpgSize);
    Image background_orig_on = ImageCache::getFromMemory(BinaryData::original_on_jpg, BinaryData::original_on_jpgSize);
    Image background_orig_off = ImageCache::getFromMemory(BinaryData::original_off_jpg, BinaryData::original_off_jpgSize);

    File test_file;
    File model_folder;

    TextButton loadButton;
    juce::String fname;
    virtual void buttonClicked(Button* button) override;
    void modelSelectChanged();
    void loadButtonClicked();

    virtual void sliderValueChanged(Slider* slider) override;
    void ampOnButtonClicked();

    void ledButtonClicked();
    void setSkin();

    void resetImages();

public:
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> gainSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> bassSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> midSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> trebleSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> presenceSliderAttach;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> masterSliderAttach;
 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmartAmpProAudioProcessorEditor)
};
