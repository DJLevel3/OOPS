/*
  ==============================================================================

    HarmonicOscillator.cpp
    Created: 16 Mar 2025 11:47:04am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HarmonicOscillator.h"

//==============================================================================
HarmonicOscillator::HarmonicOscillator(double sampleRate) : ModuleComponent(sampleRate)
{
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

    sliders[0]->setRange(1, 16, 1);
    sliders[0]->setSliderStyle(juce::Slider::Rotary);
    sliders[1]->setRange(1, 16, 1);
    sliders[1]->setSliderStyle(juce::Slider::Rotary);
    sliders[2]->setRange(-1, 1, 0.01);
    sliders[2]->setSliderStyle(juce::Slider::Rotary);
    sliders[3]->setRange(-2, 2, 0.01);
    sliders[3]->setSliderStyle(juce::Slider::Rotary);
    sliders[4]->setRange(0, 3, 1);
    sliders[4]->setSliderStyle(juce::Slider::Rotary);
    sliders[5]->setRange(-1, 1, 0.01);
    sliders[5]->setSliderStyle(juce::Slider::Rotary);

    sliders[0]->onValueChange = [this] { double v = sliders[0]->getValue(); controls[0].val[0] = v; controls[0].val[1] = v; controlsStale = true; };
    sliders[1]->onValueChange = [this] { double v = sliders[1]->getValue(); controls[1].val[0] = v; controls[1].val[1] = v; controlsStale = true; };
    sliders[2]->onValueChange = [this] { double v = sliders[2]->getValue(); controls[2].val[0] = v; controls[2].val[1] = 0; controlsStale = true; };
    sliders[3]->onValueChange = [this] { double v = sliders[3]->getValue(); controls[5].val[0] = v; controls[5].val[1] = v; controlsStale = true; };
    sliders[4]->onValueChange = [this] { double v = sliders[4]->getValue(); controls[3].val[0] = v; controls[3].val[1] = v; controlsStale = true; };
    sliders[5]->onValueChange = [this] { double v = sliders[5]->getValue(); controls[4].val[0] = v; controls[4].val[1] = v; controlsStale = true; };

    stereoButton.onClick = [this] {
        bool v = stereoButton.getToggleState();
        mono = v;
        stereoButton.setButtonText(v ? "Mono" : "Stereo");
        controlsStale = true;
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
    controls[0].val[0] = 1;
    controls[0].val[1] = 1;
    controls[1].val[0] = 1;
    controls[1].val[1] = 1;

    CableConnection cable = {
        {0},
        "Cable           ",
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

HarmonicOscillator::~HarmonicOscillator()
{
    for (int i = 0; i < 6; i++) {
        delete sliders[i];
        delete sliderLabels[i];
    }
    return;
}

void HarmonicOscillator::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(14.0f));
    g.drawText("Harmonic Oscillator", getLocalBounds().removeFromTop(20),
        juce::Justification::centred, true);   // draw some placeholder text
}

void HarmonicOscillator::resized()
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

void HarmonicOscillator::reset(int voice) {
    phase[voice][0] = controls[2].val[0] * TAU;
    phase[voice][1] = controls[2].val[1] * TAU;
    controlsStale = true;
}

void HarmonicOscillator::updateControls() {
    basePitch[0] = 261.63 * controls[0].val[0] / controls[1].val[0];
    basePitch[1] = 261.63 * controls[0].val[1] / controls[1].val[1];
    controlsStale = false;
}

void HarmonicOscillator::run() {
    if (controlsStale) updateControls();

    for (int voice = 0; voice < NUM_VOICES; voice++) {
        for (int c = 0; c < 2; c++) {
            // Update internal state values
            frequency[voice][c] = basePitch[c] * std::pow(2, cables[2].val[voice][c]) * (cables[7].val[voice][c] * controls[5].val[c] + 1) * timeStep * TAU;
            phase[voice][c] = std::fmod(phase[voice][c] + frequency[voice][c], TAU);

            // Calculate
            double x;

            int w = controls[3].val[c];
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
    time += timeStep;
}
