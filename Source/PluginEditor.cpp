/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdio.h>
#include <fstream>
#include <iostream>


//==============================================================================
SmartAmpProAudioProcessorEditor::SmartAmpProAudioProcessorEditor (SmartAmpProAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to

    addAndMakeVisible(modelSelect);
    modelSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    int c = 1;
    for (const auto& jsonFile : processor.jsonFiles) {
        modelSelect.addItem(jsonFile.getFileNameWithoutExtension(), c);
        c += 1;
    }
    modelSelect.onChange = [this] {modelSelectChanged(); };
    modelSelect.setSelectedItemIndex(processor.current_model_index, juce::NotificationType::dontSendNotification);
    modelSelect.setScrollWheelEnabled(true);

    // Set Widget Graphics
    ampSilverKnobLAF.setLookAndFeel(ImageCache::getFromMemory(BinaryData::Vintage_Knob_png, BinaryData::Vintage_Knob_pngSize));
    statusKnob.setLookAndFeel(ImageCache::getFromMemory(BinaryData::status_knob_png, BinaryData::status_knob_pngSize));

    ampOnButton.setImages(true, true, true,
        ImageCache::getFromMemory(BinaryData::Power_switch_on_png, BinaryData::Power_switch_on_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::Power_switch_on_png, BinaryData::Power_switch_on_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    addAndMakeVisible(ampOnButton);
    ampOnButton.addListener(this);

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Import Tone");
    loadButton.setColour(juce::Label::textColourId, juce::Colours::black);
    loadButton.addListener(this);


    addAndMakeVisible(ampLED);
    ampLED.setImages(true, true, true,
        ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
        Image(), 1.0, Colours::transparentWhite,
        ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
        0.0);
    ampLED.addListener(this);

    presenceSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, PRESENCE_ID, ampPresenceKnob);
    addAndMakeVisible(ampPresenceKnob);
    ampPresenceKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampPresenceKnob.addListener(this);
    //ampPresenceKnob.setRange(-10.0, 10.0);
    ampPresenceKnob.setValue(processor.ampPresenceKnobState);
    ampPresenceKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampPresenceKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 75, 20);
    ampPresenceKnob.setNumDecimalPlacesToDisplay(1);
    ampPresenceKnob.setDoubleClickReturnValue(true, 0.0);

    bassSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, BASS_ID, ampBassKnob); 
    addAndMakeVisible(ampBassKnob);
    ampBassKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampBassKnob.addListener(this);
    ampBassKnob.setRange(-8.0, 8.0);
    ampBassKnob.setValue(processor.ampBassKnobState);
    ampBassKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampBassKnob.setNumDecimalPlacesToDisplay(1);
    ampBassKnob.setDoubleClickReturnValue(true, 0.0);

    midSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MID_ID, ampMidKnob);   
    addAndMakeVisible(ampMidKnob);
    ampMidKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampMidKnob.addListener(this);
    ampMidKnob.setRange(-8.0, 8.0);
    ampMidKnob.setValue(processor.ampMidKnobState);
    ampMidKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampMidKnob.setNumDecimalPlacesToDisplay(1);
    ampMidKnob.setDoubleClickReturnValue(true, 0.0);

    trebleSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, TREBLE_ID, ampTrebleKnob);
    addAndMakeVisible(ampTrebleKnob);
    ampTrebleKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampTrebleKnob.addListener(this);
    ampTrebleKnob.setRange(-8.0, 8.0);
    ampTrebleKnob.setValue(processor.ampTrebleKnobState);
    ampTrebleKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampTrebleKnob.setNumDecimalPlacesToDisplay(1);
    ampTrebleKnob.setDoubleClickReturnValue(true, 0.0);

    gainSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, GAIN_ID, ampGainKnob);
    addAndMakeVisible(ampGainKnob);
    ampGainKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampGainKnob.addListener(this);
    ampGainKnob.setRange(-12.0, 12.0);
    ampGainKnob.setValue(processor.ampGainKnobState);
    ampGainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampGainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    ampGainKnob.setNumDecimalPlacesToDisplay(1);
    ampGainKnob.setDoubleClickReturnValue(true, 0.0);

    masterSliderAttach = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.treeState, MASTER_ID, ampMasterKnob);
    addAndMakeVisible(ampMasterKnob);
    ampMasterKnob.setLookAndFeel(&ampSilverKnobLAF);
    ampMasterKnob.addListener(this);
    ampMasterKnob.setRange(-36.0, 12.0);
    ampMasterKnob.setValue(processor.ampMasterKnobState);
    ampMasterKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    ampMasterKnob.setNumDecimalPlacesToDisplay(1);
    ampMasterKnob.setDoubleClickReturnValue(true, -12.0);

    addAndMakeVisible(versionLabel);
    versionLabel.setText("v1.0", juce::NotificationType::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::left);
    versionLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    auto font = versionLabel.getFont();
    float height = font.getHeight();
    font.setHeight(height); // 0.75
    versionLabel.setFont(font);

    // Size of plugin GUI
    setSize(694, 376);

    // Set current plugin skin
    setSkin();
    resetImages();

}

SmartAmpProAudioProcessorEditor::~SmartAmpProAudioProcessorEditor()
{
    ampBassKnob.setLookAndFeel(nullptr);
    ampMidKnob.setLookAndFeel(nullptr);
    ampTrebleKnob.setLookAndFeel(nullptr);
    ampGainKnob.setLookAndFeel(nullptr);
    ampPresenceKnob.setLookAndFeel(nullptr);
    ampMasterKnob.setLookAndFeel(nullptr);
}

//==============================================================================
void SmartAmpProAudioProcessorEditor::paint (Graphics& g)
{
    Image background_smp_on = ImageCache::getFromMemory(BinaryData::smp_on_jpg, BinaryData::smp_on_jpgSize);
    Image background_smp_off = ImageCache::getFromMemory(BinaryData::smp_off_jpg, BinaryData::smp_off_jpgSize);
    Image background_orig_on = ImageCache::getFromMemory(BinaryData::original_on_jpg, BinaryData::original_on_jpgSize);
    Image background_orig_off = ImageCache::getFromMemory(BinaryData::original_off_jpg, BinaryData::original_off_jpgSize);
    if (processor.skin == 0) {
        if (current_background == 1 && processor.amp_state == 1) {
            // Redraw only the clipped part of the background image
            juce::Rectangle<int> ClipRect = g.getClipBounds();
            g.drawImage(background_smp_on, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
        }
        else {
            // Redraw only the clipped part of the background image
            juce::Rectangle<int> ClipRect = g.getClipBounds();
            g.drawImage(background_smp_off, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
        }
    } else if (processor.skin == 1) {
        if (current_background == 1 && processor.amp_state == 1) {
            // Redraw only the clipped part of the background image
            juce::Rectangle<int> ClipRect = g.getClipBounds();
            g.drawImage(background_orig_on, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
        }
        else {
            // Redraw only the clipped part of the background image
            juce::Rectangle<int> ClipRect = g.getClipBounds();
            g.drawImage(background_orig_off, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
        }
    }

}

void SmartAmpProAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    modelSelect.setBounds(15, 10, 210, 25);

    loadButton.setBounds(15, 42, 300, 25);


    // Amp Widgets
    ampPresenceKnob.setBounds(445, 242, 55, 75);
    ampBassKnob.setBounds(203, 242, 55, 75);
    ampMidKnob.setBounds(283, 242, 55, 75);
    ampTrebleKnob.setBounds(368, 242, 55, 75);
    ampGainKnob.setBounds(100, 225, 75, 95);
    ampMasterKnob.setBounds(542, 225, 75, 95);

    ampOnButton.setBounds(54, 259, 15, 25);
    ampLED.setBounds(636, 240, 15, 15);
    versionLabel.setBounds(675, 362, 60, 10);
}

void SmartAmpProAudioProcessorEditor::modelSelectChanged()
{
    const int selectedFileIndex = modelSelect.getSelectedItemIndex();
    if (selectedFileIndex >= 0 && selectedFileIndex < processor.jsonFiles.size()) {
        processor.loadConfig(processor.jsonFiles[selectedFileIndex]);
        processor.current_model_index = modelSelect.getSelectedItemIndex();
    }
}

void SmartAmpProAudioProcessorEditor::loadButtonClicked()
{
    myChooser = std::make_unique<FileChooser> ("Select a folder to load models from",
                                               processor.folder,
                                               "*.json");
 
    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
 
    myChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)                
    {
        if (!chooser.getResult().exists()) {
                return;
        }
        
        Array<File> files = chooser.getResults();
        for (auto file : files) {
            File fullpath = processor.userAppDataDirectory_tones.getFullPathName() + "/" + file.getFileName();
            bool b = fullpath.existsAsFile();
            if (fullpath.existsAsFile() == false) {

                processor.loadConfig(file);
                fname = file.getFileName();
                processor.loaded_tone = file;
                processor.loaded_tone_name = fname;
                processor.custom_tone = 1;

                // Copy selected file to model directory and load into dropdown menu
                bool a = file.copyFileTo(fullpath);
                if (a == true) {
                    modelSelect.addItem(file.getFileNameWithoutExtension(), processor.jsonFiles.size() + 1);
                    modelSelect.setSelectedItemIndex(processor.jsonFiles.size(), juce::NotificationType::dontSendNotification);
                    processor.jsonFiles.push_back(file);

            processor.folder = file.getParentDirectory();
        } 
        
    }
}



void SmartAmpProAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &ampOnButton) {
        ampOnButtonClicked();
        helpLabel.setText("", juce::NotificationType::dontSendNotification);
    } else if (button == &loadButton) {
        loadButtonClicked();
    } else if (button == &recordButton) {
        recordButtonClicked();
    } else if (button == &trainButton) {
        trainButtonClicked();
    } else if (button == &exportButton) {
        exportButtonClicked();
    } else if (button == &ampLED) {
        ledButtonClicked();
    }
}

void SmartAmpProAudioProcessorEditor::setSkin() {
    if (processor.skin == 1) {
        ampSilverKnobLAF.setLookAndFeel(ImageCache::getFromMemory(BinaryData::knob_silver_png, BinaryData::knob_silver_pngSize));
        // Amp Widgets
        ampPresenceKnob.setBounds(461, 264, 65, 85);
        ampBassKnob.setBounds(179, 264, 65, 85);
        ampMidKnob.setBounds(276, 264, 65, 85);
        ampTrebleKnob.setBounds(368, 264, 65, 85);
        ampGainKnob.setBounds(86, 264, 65, 85);
        ampMasterKnob.setBounds(567, 264, 65, 85);

        ampOnButton.setBounds(25, 277, 15, 25);
        ampLED.setBounds(653, 87, 15, 15);
        resetImages();
        repaint();
    }
    else if (processor.skin == 0) {
        ampSilverKnobLAF.setLookAndFeel(ImageCache::getFromMemory(BinaryData::Vintage_Knob_png, BinaryData::Vintage_Knob_pngSize));
        // Amp Widgets
        ampPresenceKnob.setBounds(445, 242, 55, 75);
        ampBassKnob.setBounds(203, 242, 55, 75);
        ampMidKnob.setBounds(283, 242, 55, 75);
        ampTrebleKnob.setBounds(368, 242, 55, 75);
        ampGainKnob.setBounds(100, 225, 75, 95);
        ampMasterKnob.setBounds(542, 225, 75, 95);

        ampOnButton.setBounds(54, 259, 15, 25);
        ampLED.setBounds(636, 240, 15, 15);
        resetImages();
        repaint();
    }
}

void SmartAmpProAudioProcessorEditor::ledButtonClicked() {
    if (processor.skin == 0) {
        processor.skin = 1;
        setSkin();
    } else if (processor.skin == 1) {
        processor.skin = 0;
        setSkin();
    }
}

void SmartAmpProAudioProcessorEditor::ampOnButtonClicked() {
    if (processor.amp_state == 0) {
        processor.amp_state = 1;
        // Reset the directory in case user is manually adding or removing models from the SmartAmpPro directory
        processor.resetDirectory(processor.userAppDataDirectory_tones);
    }
    else {
        processor.amp_state = 0;
    }
    resetImages();
    repaint();
}


void SmartAmpProAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    // Amp
    if (slider == &ampGainKnob)
        processor.set_ampDrive(slider->getValue());
    else if (slider == &ampMasterKnob)
        processor.set_ampMaster(slider->getValue());
    else if (slider == &ampBassKnob || slider == &ampMidKnob || slider == &ampTrebleKnob) {
        processor.set_ampEQ(ampBassKnob.getValue(), ampMidKnob.getValue(), ampTrebleKnob.getValue(), ampPresenceKnob.getValue());
        // Set knob states for saving positions when closing/reopening GUI
        processor.ampBassKnobState = ampBassKnob.getValue();
        processor.ampMidKnobState = ampMidKnob.getValue();
        processor.ampTrebleKnobState = ampTrebleKnob.getValue();
    }
    else if (slider == &ampPresenceKnob) {
            processor.set_ampEQ(ampBassKnob.getValue(), ampMidKnob.getValue(), ampTrebleKnob.getValue(), ampPresenceKnob.getValue());
    }
}


void SmartAmpProAudioProcessorEditor::resetImages()
{
    // Set On/Off amp graphic
    if (processor.skin == 0) {
        if (processor.amp_state == 0) {
            ampOnButton.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::Power_switch_off_png, BinaryData::Power_switch_off_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::Power_switch_off_png, BinaryData::Power_switch_off_pngSize), 1.0, Colours::transparentWhite,
                0.0);
            ampLED.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::led_blue_off_png, BinaryData::led_blue_off_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::led_blue_off_png, BinaryData::led_blue_off_pngSize), 1.0, Colours::transparentWhite,
                0.0);
        }
        else {
            ampOnButton.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::Power_switch_on_png, BinaryData::Power_switch_on_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::Power_switch_on_png, BinaryData::Power_switch_on_pngSize), 1.0, Colours::transparentWhite,
                0.0);
            ampLED.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
                0.0);
        }
    }
    else if (processor.skin == 1) {
        if (processor.amp_state == 0) {
            ampOnButton.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::power_switch_up_png, BinaryData::power_switch_up_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::power_switch_down_png, BinaryData::power_switch_down_pngSize), 1.0, Colours::transparentWhite,
                0.0);
            ampLED.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::led_blue_off_png, BinaryData::led_blue_off_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::led_blue_off_png, BinaryData::led_blue_off_pngSize), 1.0, Colours::transparentWhite,
                0.0);
        }
        else {
            ampOnButton.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::power_switch_down_png, BinaryData::Power_switch_on_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::power_switch_down_png, BinaryData::power_switch_down_pngSize), 1.0, Colours::transparentWhite,
                0.0);
            ampLED.setImages(true, true, true,
                ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
                Image(), 1.0, Colours::transparentWhite,
                ImageCache::getFromMemory(BinaryData::led_blue_on_png, BinaryData::led_blue_on_pngSize), 1.0, Colours::transparentWhite,
                0.0);
        }
    }
}