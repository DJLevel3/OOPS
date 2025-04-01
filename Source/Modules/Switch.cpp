/*
  ==============================================================================

    Switch.cpp
    Created: 16 Mar 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Switch.h"

/*
//==============================================================================
Switch::Switch(double sampleRate) : ModuleComponent(sampleRate)
{
    moduleType = SwitchType;
    addAndMakeVisible(factor);
    addAndMakeVisible(factorText);
    factor.setRange(0.0, 1.0, 0.01);
    factor.setSliderStyle(juce::Slider::Rotary);
    factorText.attachToComponent(&factor, false);
    factorText.setText("Factor", juce::dontSendNotification);
    factorText.setJustificationType(juce::Justification::centredBottom);
    factor.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

    addAndMakeVisible(factorMod);
    addAndMakeVisible(factorModText);
    factorMod.setRange(-1.0, 1.0, 0.01);
    factorMod.setSliderStyle(juce::Slider::Rotary);
    factorModText.attachToComponent(&factorMod, false);
    factorModText.setText("CV Mod", juce::dontSendNotification);
    factorModText.setJustificationType(juce::Justification::centredBottom);
    factorMod.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);

    factor.onValueChange = [this] { double v = factor.getValue(); controls[0].val[0] = v; controls[0].val[1] = v; controlsStale = true; };
    factorMod.onValueChange = [this] { double v = factorMod.getValue(); controls[1].val[0] = v; controls[1].val[1] = v; };

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


    for (int i = 0; i < 2; i++) {
        controls.push_back(control);
        controls[i].name = controlNames[i];
    }
    for (int i = 0; i < 4; i++) {
        cables.push_back(cable);
        cables[i].name = cableNames[i];
    }
    cables[0].input = false;

    factor.setValue(0.0, juce::sendNotificationSync);
    factorMod.setValue(0.0, juce::sendNotificationSync);
}

Switch::~Switch()
{
}

void Switch::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(14.0f));
    g.drawText(ModuleStrings.at(moduleType), getLocalBounds().removeFromTop(20),
        juce::Justification::centred, true);   // draw some placeholder text
}

void Switch::updateControls() {
    for (int c = 0; c < 2; c++) {
        scale[c] = controls[0].val[c];
    }
    controlsStale = false;
}

void Switch::resized()
{
    juce::Rectangle<int> box = juce::Rectangle<int>(80, 240).withCentre(getLocalBounds().getCentre());
    factor.setBounds(box.removeFromTop(120));
    box.removeFromTop(40);
    factorMod.setBounds(box);
}

void Switch::reset(int voice) {
}

void Switch::run(int numVoices) {
    if (numVoices > NUM_VOICES) numVoices = NUM_VOICES;
    if (controlsStale) updateControls();

    for (int voice = 0; voice < numVoices; voice++) {
        for (int c = 0; c < 2; c++) {
            double m = scale[c] + controls[1].val[c] * cables[3].val[voice][c];
            cables[0].val[voice][c] = cables[1].val[voice][c] * (cables[2].val[voice][c] * clamp(m) + clamp(1 - m));;
        }
    }
    time += timeStep;
}

juce::String Switch::getState() {
    juce::String stateString = "";
    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
    }
    return stateString;
}

void Switch::setState(juce::String state) {
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
*/