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
    setSize (1080, 800);
    addAndMakeVisible(audioProcessor.osc);
    addAndMakeVisible(audioProcessor.fmOsc);
    addAndMakeVisible(audioProcessor.env);
}

OOPSAudioProcessorEditor::~OOPSAudioProcessorEditor()
{
}

//==============================================================================
void OOPSAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    auto area = getLocalBounds();
    auto area2 = area.removeFromBottom(400);

    audioProcessor.fmOsc.setBounds(area.removeFromLeft(180));
    audioProcessor.osc.setBounds(area.removeFromLeft(180));
    audioProcessor.env.setBounds(area.removeFromLeft(180));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", area, juce::Justification::centred, 1);
}

void OOPSAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
