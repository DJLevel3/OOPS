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
    setSize (1080, 960);
    addAndMakeVisible(audioProcessor.processingOrder[0]);
    addAndMakeVisible(audioProcessor.processingOrder[1]);
    addAndMakeVisible(audioProcessor.processingOrder[2]);
    addAndMakeVisible(audioProcessor.processingOrder[3]);
}

OOPSAudioProcessorEditor::~OOPSAudioProcessorEditor()
{
}

//==============================================================================
void OOPSAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void OOPSAudioProcessorEditor::resized()
{
    setSize(1280, 800);
    std::vector<juce::Rectangle<int>> moduleSlots;
    juce::Rectangle<int> bounds = getLocalBounds();
    juce::Rectangle<int> area = getLocalBounds().removeFromRight(1080);
    juce::Rectangle<int> area2 = area.removeFromBottom(400);

    for (int i = 0; i < 8; i++) {
        moduleSlots.push_back(area.removeFromLeft(135));
    }

    for (int i = 0; i < 8; i++) {
        moduleSlots.push_back(area2.removeFromLeft(135));
    }

    for (int i = 0; i < std::min(audioProcessor.processingOrder.size(), moduleSlots.size()); i++) {
        audioProcessor.processingOrder[i]->setBounds(moduleSlots[i]);
    }
}
