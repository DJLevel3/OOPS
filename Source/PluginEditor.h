/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OOPSAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OOPSAudioProcessorEditor (OOPSAudioProcessor&);
    ~OOPSAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void updatePanel(bool initialize = false);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OOPSAudioProcessor& audioProcessor;
    juce::Slider voicesSlider;

    juce::Slider     moduleTypeSlider;
    juce::Slider     moduleSelectSlider;
    juce::Slider     cableSourceModuleSlider;
    juce::Slider     cableSourceCableSlider;
    juce::Slider     cableDestinationModuleSlider;
    juce::Slider     cableDestinationCableSlider;
    juce::Slider     cableSelectSlider;
    juce::Slider     automationSelectSlider;
    juce::Slider     automationTargetSlider;

    juce::Label      labelModuleTypeSlider;
    juce::Label      labelModuleSelectSlider;
    juce::Label      labelCableSourceModuleSlider;
    juce::Label      labelCableSourceCableSlider;
    juce::Label      labelCableDestinationModuleSlider;
    juce::Label      labelCableDestinationCableSlider;
    juce::Label      labelCableSelectSlider;
    juce::Label      labelAutomationSelectSlider;
    juce::Label      labelAutomationTargetSlider;

    juce::TextButton addModuleButton;
    //juce::TextButton moveModuleButton;
    juce::TextButton deleteModuleButton;
    juce::TextButton addCableButton;
    juce::TextButton deleteCableButton;
    juce::TextButton assignAutomationButton;
    juce::TextButton removeAutomationButton;

    int oldPOSize = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OOPSAudioProcessorEditor)
};
