/*
  ==============================================================================

    VCSwitch.cpp
    Created: 16 Mar 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VCSwitch.h"

//==============================================================================
VCSwitch::VCSwitch(double sampleRate) : ModuleComponent(sampleRate)
{
    numAutomations = 1;
    moduleType = VCSwitchType;

    ModuleControl control = {
        {0,0},
        "Control",
        true,
        true
    };
    CableConnection cable = {
        {0},
        "Cable",
        true,
        true,
        true
    };

    for (int i = 0; i < cableNames.size(); i++) {
        cables.push_back(cable);
        cables[i].name = cableNames[i];
    }
}

VCSwitch::~VCSwitch()
{
}

void VCSwitch::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(14.0f));
    auto area = getLocalBounds();
    g.drawText(ModuleStrings.at(moduleType), area.removeFromTop(20),
        juce::Justification::centred, true);   // draw some placeholder text
}

void VCSwitch::updateControls() {
    controlsStale = false;
}

void VCSwitch::resized()
{

}

void VCSwitch::reset(int voice) {
    for (int c = 0; c < cableNames.size(); c++) {
        cables[c].val[voice][0] = 0;
        cables[c].val[voice][1] = 0;
    }
}

void VCSwitch::run(int numVoices) {
    if (numVoices > NUM_VOICES) numVoices = NUM_VOICES;
    if (controlsStale) updateControls();

    for (int voice = 0; voice < numVoices; voice++) {
        for (int c = 0; c < 2; c++) {
            bool energized = cables[1].val[voice][c] >= 0;

            cables[0].val[voice][c] = energized ? (cables[4].val[voice][c]) : (cables[2].val[voice][c]);
            cables[3].val[voice][c] = energized ? 0 : (cables[2].val[voice][c]);
            cables[5].val[voice][c] = energized ? (cables[4].val[voice][c]) : 0;
        }
    }
    time += timeStep;
}

void VCSwitch::automate(int channel, double newValue) {
    return;
}

juce::String VCSwitch::getState() {
    return "N";
}

void VCSwitch::setState(juce::String state) {
    controlsStale = true;
}