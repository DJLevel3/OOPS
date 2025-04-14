/*
  ==============================================================================

    VoltageUtility.cpp
    Created: 16 Mar 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VoltageUtility.h"

//==============================================================================
VoltageUtility::VoltageUtility(double sampleRate) : ModuleComponent(sampleRate)
{
    numAutomations = 6;
    moduleType = VoltageUtilityType;
    for (int i = 0; i < sliderNames.size(); i++) {
        sliders.push_back(new juce::Slider);
        sliderLabels.push_back(new juce::Label);
        addAndMakeVisible(sliders[i]);
        addAndMakeVisible(sliderLabels[i]);
        sliders[i]->setRange(-1.0, 1.0, 0.01);
        sliders[i]->setSliderStyle(juce::Slider::Rotary);
        sliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        sliderLabels[i]->setText(sliderNames[i], juce::dontSendNotification);
        sliderLabels[i]->setJustificationType(juce::Justification::centredBottom);
    }

    sliders[0]->onValueChange = [this] { double v = sliders[0]->getValue(); controls[0].val[0] = v; controls[0].val[1] = v; controlsStale = true; dawDirty.push_back(0); };
    sliders[1]->onValueChange = [this] { double v = sliders[1]->getValue(); controls[1].val[0] = v; controls[1].val[1] = v; controlsStale = true; dawDirty.push_back(1); };
    sliders[2]->onValueChange = [this] { double v = sliders[2]->getValue(); controls[2].val[0] = v; controls[2].val[1] = v; controlsStale = true; dawDirty.push_back(2); };
    sliders[3]->onValueChange = [this] { double v = sliders[3]->getValue(); controls[3].val[0] = v; controls[3].val[1] = v; controlsStale = true; dawDirty.push_back(3); };
    sliders[4]->onValueChange = [this] { double v = sliders[4]->getValue(); controls[4].val[0] = v; controls[4].val[1] = v; controlsStale = true; dawDirty.push_back(4); };
    sliders[5]->onValueChange = [this] { double v = sliders[5]->getValue(); controls[5].val[0] = v; controls[5].val[1] = v; controlsStale = true; dawDirty.push_back(5); };

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


    for (int i = 0; i < controlNames.size(); i++) {
        controls.push_back(control);
        controls[i].name = controlNames[i];
    }
    for (int i = 0; i < cableNames.size(); i++) {
        cables.push_back(cable);
        cables[i].name = cableNames[i];
    }
    cables[0].input = false;

    for (int i = 0; i < sliders.size(); i++) {
        sliders[i]->setDoubleClickReturnValue(true, 0);
        sliders[i]->setValue(0, juce::sendNotificationSync);
    }
    dawDirty.clear();

    dawDirty.push_back(0);
    dawDirty.push_back(1);
    dawDirty.push_back(2);
    dawDirty.push_back(3);
    dawDirty.push_back(4);
    dawDirty.push_back(5);
}

VoltageUtility::~VoltageUtility()
{
    for (int i = (int)sliders.size() - 1; i >= 0; i--) {
        delete sliders[i];
        delete sliderLabels[i];
    }
}

void VoltageUtility::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(14.0f));
    auto area = getLocalBounds();
    g.drawText(ModuleStrings.at(moduleType), area.removeFromTop(20),
        juce::Justification::centred, true);   // draw some placeholder text

    area.expand(-5, -5);
    int h = area.getHeight() / 3;
    g.setColour(juce::Colours::grey);
    g.drawRect(area.removeFromTop(h), 1);   // draw an outline around the channel
    g.drawRect(area.removeFromTop(h), 1);   // draw an outline around the channel
    g.drawRect(area.removeFromTop(h), 1);   // draw an outline around the channel
    g.setColour(juce::Colours::white);
}

void VoltageUtility::updateControls() {
    for (int c = 0; c < 2; c++) {
        scale[c] = controls[0].val[c];
    }
    controlsStale = false;
}

void VoltageUtility::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(20);
    area.expand(-5, -5);
    auto left = area.removeFromLeft(area.getWidth() / 2);
    int h = area.getHeight() / 3;

    for (int i = 0; i < 6; i++) {
        sliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, left.getWidth() - 10, 20);
    }

    std::vector<juce::Rectangle<int>> sections;
    for (int i = 0; i < 3; i++) {
        sections.push_back(left.removeFromTop(h).expanded(0, -3));
        sections.push_back(area.removeFromTop(h).expanded(0, -3));
    }
    for (int slider = 0; slider < 6; slider++) {
        sliderLabels[slider]->setBounds(sections[slider].removeFromTop(20));
        sliders[slider]->setBounds(sections[slider]);
    }

}

void VoltageUtility::reset(int voice) {
}

void VoltageUtility::run(int numVoices) {
    if (numVoices > NUM_VOICES) numVoices = NUM_VOICES;
    if (controlsStale) updateControls();

    for (int voice = 0; voice < numVoices; voice++) {
        for (int cell = 0; cell < 3; cell++) {
            for (int channel = 0; channel < 2; channel++) {
                cables[cell * 2].val[voice][channel] = cables[cell * 2 + 1].val[voice][channel] * controls[cell * 2].val[channel] + controls[cell * 2 + 1].val[channel];
            }
        }
    }
    time += timeStep;
}

void VoltageUtility::automate(int channel, double newValue) {
    
    if (channel < sliders.size()) {
        int s = channel;
        double h = floor((sliders[s]->getMaximum() - sliders[s]->getMinimum()) * (newValue) / sliders[s]->getInterval()) * sliders[s]->getInterval();
        double v = (sliders[s]->getMinimum() + h);
        juce::MessageManager::callAsync([this, s, v]() {sliders[s]->setValue(v, juce::sendNotificationSync); });
        controlsStale = true;
    }
}

juce::String VoltageUtility::getState() {
    juce::String stateString = "";
    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
    }
    return stateString;
}

void VoltageUtility::setState(juce::String state) {
    controlsStale = true;
    juce::StringArray array;
    array.addTokens(state, ":", "");
    if (array.size() < (int)controls.size() * 2 + 1) {
        for (int slider = 0; slider < (int)sliders.size(); slider++) {
            sliders[slider]->setValue(0.0, juce::sendNotificationSync);
        }
        return;
    }

    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        sliders[slider]->setValue(array[slider * 2].getDoubleValue(), juce::sendNotificationSync);
    }
}