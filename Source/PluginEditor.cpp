/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OOPSAudioProcessorEditor::OOPSAudioProcessorEditor (OOPSAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(false, false);
    setSize (1280, 800);

    
    addAndMakeVisible(voicesSlider);
    voicesSlider.setRange(1, NUM_VOICES, 1);
    voicesSlider.setSliderStyle(juce::Slider::Rotary);
    voicesSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    voicesSlider.onValueChange = [this] {
        audioProcessor.voiceLimit = (int)voicesSlider.getValue();
        };
    voicesSlider.setValue(audioProcessor.voiceLimit, juce::NotificationType::dontSendNotification);
}

OOPSAudioProcessorEditor::~OOPSAudioProcessorEditor()
{
}

//==============================================================================
void OOPSAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (audioProcessor.processingOrder.size() != oldPOSize) resized();
    oldPOSize = (int)audioProcessor.processingOrder.size();
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void OOPSAudioProcessorEditor::resized()
{
    setSize(1280, 800);
    std::vector<juce::Rectangle<int>> moduleSlots;
    juce::Rectangle<int> panel = getLocalBounds();
    juce::Rectangle<int> area = panel.removeFromRight(1080);
    juce::Rectangle<int> area2 = area.removeFromBottom(400);
    voicesSlider.setBounds(panel.removeFromBottom(100).expanded(-5,-5));

    for (int i = 0; i < 8; i++) {
        moduleSlots.push_back(area.removeFromLeft(135));
    }

    for (int i = 0; i < 8; i++) {
        moduleSlots.push_back(area2.removeFromLeft(135));
    }

    for (int i = 0; i < std::min(audioProcessor.processingOrder.size(), moduleSlots.size()); i++) {
        addAndMakeVisible(audioProcessor.processingOrder[i]);
        audioProcessor.processingOrder[i]->setBounds(moduleSlots[i]);
    }
}
