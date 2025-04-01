/*
  ==============================================================================

    Master.cpp
    Created: 27 Mar 2025 10:29:09am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include "Master.h"

//==============================================================================
Master::Master(double sampleRate) : ModuleComponent(sampleRate)
{
    numAutomations = 3;
    moduleType = MasterType;
    isMaster = true;
    for (int i = 0; i < sliderNames.size(); i++) {
        sliders.push_back(new juce::Slider);
        sliderLabels.push_back(new juce::Label);
        addAndMakeVisible(sliders[i]);
        addAndMakeVisible(sliderLabels[i]);
        sliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        sliderLabels[i]->setText(sliderNames[i], juce::dontSendNotification);
        sliderLabels[i]->setJustificationType(juce::Justification::centredBottom);
        sliderLabels[i]->attachToComponent(sliders[i], false);
    }

    sliders[0]->setRange(-20, 0, 0.1);
    sliders[0]->setSliderStyle(juce::Slider::Rotary);
    sliders[0]->setTextValueSuffix(" dB");
    sliders[0]->onValueChange = [this] { double v = sliders[0]->getValue() / 10; controls[0].val[0] = std::pow(10.0, v); controls[0].val[1] = std::pow(10.0, v); controlsStale = true; dawDirty.push_back(0);  };

    sliders[1]->setRange(-5, 5, 1);
    sliders[1]->setSliderStyle(juce::Slider::Rotary);
    sliders[1]->setTextValueSuffix(" Oct");
    sliders[1]->onValueChange = [this] { double v = sliders[1]->getValue(); controls[1].val[0] = v; controls[1].val[1] = v; controlsStale = true; dawDirty.push_back(1); };

    sliders[2]->setRange(0, 1, 0.01);
    sliders[2]->setSliderStyle(juce::Slider::Rotary);
    sliders[2]->onValueChange = [this] { double v = sliders[2]->getValue(); controls[2].val[0] = v; controls[2].val[1] = v; controlsStale = true; dawDirty.push_back(2); };

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

    for (int i = 0; i < 3; i++) {
        controls.push_back(control);
        controls[i].name = controlNames[i];
    }
    for (int i = 0; i < 4; i++) {
        cables.push_back(cable);
        cables[i].name = cableNames[i];
    }
    cables[0].input = false;

    sliders[0]->setValue(-6, juce::NotificationType::sendNotificationSync);
    sliders[1]->setValue(0, juce::NotificationType::sendNotificationSync);
    sliders[2]->setValue(0, juce::NotificationType::sendNotificationSync);
}

Master::~Master()
{
    for (int i = 0; i < sliders.size(); i++) {
        delete sliderLabels[i];
        delete sliders[i];
    }
}

void Master::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("Master", getLocalBounds().removeFromTop(20),
                juce::Justification::centred, true);   // draw some placeholder text
}

void Master::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(20);
    int h = area.getHeight();
    h = h / (int)sliders.size();
    auto b = area;
    for (int i = 0; i < sliders.size(); i++) {
        b = area.removeFromTop(h).expanded(-5);
        b.removeFromTop(20);
        sliders[i]->setBounds(b);
    }
}

void Master::reset(int voice) {
    for (int c = 0; c < 2; c++) {
        cables[3].val[voice][c] = 0;
    }
}

void Master::updateControls() {
    controlsStale = false;
}

void Master::run(int numVoices) {
    if (controlsStale) updateControls();
    for (int voice = 0; voice < numVoices; voice++) {
        for (int c = 0; c < 2; c++) {
            double pitchVal = cables[2].val[voice][c];
            if (pitchVal != actualPitch[voice][c]) {
                targetPitch[voice][c] = pitchVal;
            }

            // TODO - portamento
            actualPitch[voice][c] = targetPitch[voice][c];

            cables[2].val[voice][c] = actualPitch[voice][c];
        }
    }
}

void Master::automate(int channel, double newValue) {
    if (channel < sliders.size()) {
        sliders[channel]->setValue(newValue, juce::sendNotificationSync);
    }
}

juce::String Master::getState() {
    juce::String stateString = "";
    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
    }
    stateString = stateString.dropLastCharacters(1);
    return stateString;
}

void Master::setState(juce::String state) {
    controlsStale = true;
    juce::StringArray array;
    array.addTokens(state, ":", "");
    if (array.size() < (int)controls.size() * 2) {
        for (int slider = 0; slider < (int)sliders.size(); slider++) {
            sliders[slider]->setValue(0.0, juce::sendNotificationSync);
        }
        return;
    }

    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        sliders[slider]->setValue(array[slider * 2].getDoubleValue(), juce::sendNotificationSync);
    }
}