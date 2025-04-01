/*
  ==============================================================================

    Oscillator.cpp
    Created: 16 Mar 2025 11:47:04am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Oscillator.h"

//==============================================================================
Oscillator::Oscillator(double sampleRate) : ModuleComponent(sampleRate)
{
    numAutomations = 7;
    moduleType = OscillatorType;
    needsPitch = true;
    needsReset = true;

    for (int i = 0; i < 6; i++) {
        sliders.push_back(new juce::Slider);
        sliderLabels.push_back(new juce::Label);
        addAndMakeVisible(sliders[i]);
        addAndMakeVisible(sliderLabels[i]);
        sliderLabels[i]->setText(sliderNames[i], juce::dontSendNotification);
        sliderLabels[i]->setJustificationType(juce::Justification::centredBottom);
    }

    addAndMakeVisible(stereoButton);
    stereoButton.setButtonText("Stereo");
    stereoButton.setClickingTogglesState(true);

    sliders[0]->setRange(-24, 24, 1);
    sliders[0]->setSliderStyle(juce::Slider::Rotary);
    sliders[1]->setRange(-100, 100, 1);
    sliders[1]->setSliderStyle(juce::Slider::Rotary);
    sliders[2]->setRange(-1, 1, 0.01);
    sliders[2]->setSliderStyle(juce::Slider::Rotary);
    sliders[3]->setRange(-2, 2, 0.01);
    sliders[3]->setSliderStyle(juce::Slider::Rotary);
    sliders[4]->setRange(0, 3, 1);
    sliders[4]->setSliderStyle(juce::Slider::Rotary);
    sliders[5]->setRange(-1, 1, 0.01);
    sliders[5]->setSliderStyle(juce::Slider::Rotary);

    sliders[0]->onValueChange = [this] { double v = sliders[0]->getValue(); controls[0].val[0] = v; controls[0].val[1] =  v; controlsStale = true; dawDirty.push_back(0); };
    sliders[1]->onValueChange = [this] { double v = sliders[1]->getValue(); controls[1].val[0] = v; controls[1].val[1] = -v; controlsStale = true; dawDirty.push_back(1); };
    sliders[2]->onValueChange = [this] { double v = sliders[2]->getValue(); controls[2].val[0] = v; controls[2].val[1] =  0; controlsStale = true; dawDirty.push_back(2); };
    sliders[3]->onValueChange = [this] { double v = sliders[3]->getValue(); controls[5].val[0] = v; controls[5].val[1] =  v; controlsStale = true; dawDirty.push_back(3); };
    sliders[4]->onValueChange = [this] { double v = sliders[4]->getValue(); controls[3].val[0] = v; controls[3].val[1] =  v; controlsStale = true; dawDirty.push_back(4); };
    sliders[5]->onValueChange = [this] { double v = sliders[5]->getValue(); controls[4].val[0] = v; controls[4].val[1] =  v; controlsStale = true; dawDirty.push_back(5); };

    stereoButton.onClick = [this] {
        bool v = stereoButton.getToggleState();
        mono = v;
        stereoButton.setButtonText(v ? "Mono" : "Stereo");
        controlsStale = true;
        dawDirty.push_back(6);
        };

    ModuleControl control = {
        {0,0},
        "Control",
        true,
        true
    };
    for (int i = 0; i < 6; i++) {
        controls.push_back(control);
        controls[i].name = controlNames[i];
    }

    CableConnection cable = {
        {0},
        "Cable",
        true,
        true,
        true
    };
    for (int i = 0; i < 8; i++) {
        cables.push_back(cable);
        cables[i].name = cableNames[i];
    }
    cables[0].input = false;

    sliders[0]->setValue(0, juce::sendNotificationSync);
    sliders[1]->setValue(0, juce::sendNotificationSync);
    sliders[2]->setValue(0, juce::sendNotificationSync);
    sliders[3]->setValue(0, juce::sendNotificationSync);
    sliders[4]->setValue(0, juce::sendNotificationSync);
    sliders[5]->setValue(0, juce::sendNotificationSync);

    reset();
}

Oscillator::~Oscillator()
{
    for (int i = 0; i < sliders.size(); i++) {
        delete sliders[i];
        delete sliderLabels[i];
    }
}

void Oscillator::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText (ModuleStrings.at(moduleType), getLocalBounds().removeFromTop(20),
                juce::Justification::centred, true);   // draw some placeholder text
}

void Oscillator::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    area.removeFromTop(20);
    auto bottom = area.removeFromBottom(25);
    stereoButton.setBounds(juce::Rectangle<int>(100, 20).withCentre(bottom.getCentre()));

    area.expand(-5, -5);
    auto left = area.removeFromLeft(area.getWidth() / 2);


    for (int i = 0; i < 6; i++) {
        sliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, left.getWidth() - 10, 20);
    }

    auto top = left.removeFromTop(left.getHeight() / 3).expanded(0, -3);
    sliderLabels[0]->setBounds(top.removeFromTop(20));
    sliders[0]->setBounds(top);
    top = area.removeFromTop(area.getHeight() / 3).expanded(0, -3);
    sliderLabels[1]->setBounds(top.removeFromTop(20));
    sliders[1]->setBounds(top);

    top = left.removeFromTop(left.getHeight() / 2).expanded(0, -3);
    sliderLabels[2]->setBounds(top.removeFromTop(20));
    sliders[2]->setBounds(top);
    top = area.removeFromTop(area.getHeight() / 2).expanded(0, -3);
    sliderLabels[3]->setBounds(top.removeFromTop(20));
    sliders[3]->setBounds(top);

    left = left.expanded(0, -3);
    sliderLabels[4]->setBounds(left.removeFromTop(20));
    sliders[4]->setBounds(left);

    area = area.expanded(0, -3);
    sliderLabels[5]->setBounds(area.removeFromTop(20));
    sliders[5]->setBounds(area);

}

void Oscillator::reset(int voice) {
    phase[voice][0] = controls[2].val[0] * TAU;
    phase[voice][1] = controls[2].val[1] * TAU;
    controlsStale = true;
}

void Oscillator::updateControls() {
    basePitch[0] = 261.63 * std::pow(2, controls[0].val[0] / 12) * std::pow(2, controls[1].val[0] / 1200);
    basePitch[1] = 261.63 * std::pow(2, controls[0].val[1] / 12) * std::pow(2, controls[1].val[1] / 1200);
    controlsStale = false;
}

void Oscillator::run(int numVoices) {
    if (numVoices > NUM_VOICES) numVoices = NUM_VOICES;
    if (controlsStale) updateControls();

    for (int voice = 0; voice < numVoices; voice++) {
        for (int c = 0; c < 2; c++){
            if (mono && c > 0) {
                cables[0].val[voice][c] = cables[0].val[voice][0];
            }
            else {
                // Update internal state values
                frequency[voice][c] = basePitch[c] * std::pow(2, cables[2].val[voice][c]) * (cables[7].val[voice][c] * controls[5].val[c] + 1) * timeStep * TAU;
                phase[voice][c] = std::fmod(phase[voice][c] + frequency[voice][c], TAU);

                int w = (int)controls[3].val[c];
                double x;
                switch (w) {
                case 1:
                    x = (phase[voice][c] - PI) / PI;
                    break;
                case 2:
                    x = (phase[voice][c] > (PI * (1 + controls[4].val[c]))) ? (-1 - controls[4].val[c]) : (1 - controls[4].val[c]);
                    break;
                case 3:
                    x = ((phase[voice][c] > PI) ? -1 : 1) * ((std::fmod(2 * phase[voice][c], TAU) - PI) / PI);
                    break;
                default:
                    x = std::sin(phase[voice][c]);
                }

                // Update outputs
                cables[0].val[voice][c] = x;
            }
        }
    }
    time += timeStep;
}

void Oscillator::automate(int channel, double newValue) {
    if (channel < sliders.size()) {
        sliders[channel]->setValue(newValue, juce::sendNotificationSync);
    }
    else if (channel == sliders.size()) stereoButton.setToggleState(newValue >= 1, juce::sendNotificationSync);
}

juce::String Oscillator::getState() {
    juce::String stateString = "";
    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
    }
    stateString.append(mono ? "M" : "S", 1);
    return stateString;
}
void Oscillator::setState(juce::String state) {
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
    mono = state.getLastCharacter() == 'M';
    stereoButton.setToggleState(mono, juce::sendNotificationSync);
}