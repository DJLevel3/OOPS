/*
  ==============================================================================

    Envelope.cpp
    Created: 16 Mar 2025 11:47:29am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Envelope.h"

//==============================================================================
Envelope::Envelope(double sampleRate) : ModuleComponent(sampleRate)
{
    for (int i = 0; i < 5; i++) {
        sliders.push_back(new juce::Slider);
        sliderLabels.push_back(new juce::Label);
        addAndMakeVisible(sliders[i]);
        addAndMakeVisible(sliderLabels[i]);
        sliders[i]->setRange(0.0, 1.0, 0.01);
        sliders[i]->setSliderStyle(juce::Slider::LinearBarVertical);
        sliderLabels[i]->attachToComponent(sliders[i], false);
        sliderLabels[i]->setText(sliderNames[i], juce::dontSendNotification);
        sliderLabels[i]->setJustificationType(juce::Justification::centredBottom);
    }

    addAndMakeVisible(shapeButton);
    shapeButton.setButtonText("Analog");
    shapeButton.setClickingTogglesState(true);

    sliders[0]->onValueChange = [this] { double v = sliders[0]->getValue(); controls[2].val[0] = v; controls[2].val[1] = v; controlsStale = true; };
    sliders[1]->onValueChange = [this] { double v = sliders[1]->getValue(); controls[4].val[0] = v; controls[4].val[1] = v; controlsStale = true; };
    sliders[2]->onValueChange = [this] { double v = sliders[2]->getValue(); controls[5].val[0] = v; controls[5].val[1] = v; controlsStale = true; };
    sliders[3]->onValueChange = [this] { double v = sliders[3]->getValue(); controls[6].val[0] = v; controls[6].val[1] = v; controlsStale = true; };
    sliders[4]->onValueChange = [this] { double v = sliders[4]->getValue(); controls[7].val[0] = v; controls[7].val[1] = v; controlsStale = true; };

    shapeButton.onClick = [this] { bool v = shapeButton.getToggleState(); controls[3].val[0] = v; controls[3].val[1] = v; shapeButton.setButtonText(v ? "Linear" : "Analog"); controlsStale = true; };

    ModuleControl control = {
        {0,0},
        "Control",
        true,
        true
    };
    CableConnection cable = {
        {0,0},
        "Cable",
        true,
        true,
        true
    };

    for (int i = 0; i < 8; i++) {
        controls.push_back(control);
        controls[i].name = controlNames[i];
    }
    for (int i = 0; i < 6; i++) {
        cables.push_back(cable);
        cables[i].name = cableNames[i];
    }
    cables[0].input = false;
    cables[4].input = false;

    sliders[0]->setValue(0, juce::sendNotificationSync);
    sliders[1]->setValue(0, juce::sendNotificationSync);
    sliders[2]->setValue(0, juce::sendNotificationSync);
    sliders[3]->setValue(1, juce::sendNotificationSync);
    sliders[4]->setValue(0, juce::sendNotificationSync);

    reset();
}

Envelope::~Envelope()
{
    for (int i = 0; i < 5; i++) {
        delete sliders[i];
        delete sliderLabels[i];
    }
}

void Envelope::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText("Envelope", getLocalBounds().removeFromTop(20) ,
                juce::Justification::centred, true);   // draw some placeholder text
}

void Envelope::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    area.removeFromTop(20);
    auto bottom = area.removeFromBottom(25);
    shapeButton.setBounds(juce::Rectangle<int>(100, 20).withCentre(bottom.getCentre()));
    area.expand(-5, -5);
    area.removeFromTop(20);
    int width = area.getWidth() / 5;
    bottom = area.removeFromLeft(width);
    sliders[0]->setBounds(bottom.expanded(-2, -2));
    bottom = area.removeFromLeft(width);
    sliders[1]->setBounds(bottom.expanded(-2, -2));
    bottom = area.removeFromRight(width);
    sliders[4]->setBounds(bottom.expanded(-2, -2));
    bottom = area.removeFromRight(width);
    sliders[3]->setBounds(bottom.expanded(-2, -2));
    sliders[2]->setBounds(area.expanded(-2, -2));
}

void Envelope::reset() {
    phase[0] = 0;
    phase[1] = 0;
    currentValue[0] = 0;
    currentValue[1] = 0;
    delayCounter[0] = 0;
    delayCounter[1] = 0;
    triggered[0] = 0;
    triggered[1] = 0;
    time = 0;
    controlsStale = true;
}

void Envelope::updateControls() {
    for (int c = 0; c < 2; c++) {
        linear[c] = controls[3].val[c] >= 1;

        rates[0][c] = (1 / clamp(controls[7].val[c], 0.0001, 10000)) * timeStep;
        rates[1][c] = (1 / clamp(controls[7].val[c], 0.0001, 10000)) * timeStep;
        rates[2][c] = (1 / clamp(controls[4].val[c], 0.0001, 10000)) * timeStep;
        rates[3][c] = (1 / clamp(controls[5].val[c], 0.0001, 10000)) * timeStep;
        rates[4][c] = controls[6].val[c];

        delayTime[c] = controls[2].val[c];
    }
    controlsStale = false;
}

void Envelope::run() {
    // Reset signal
    for (int c = 0; c < 2; c++) {
        if (cables[5].val[c] >= 1) {
            phase[c] = 0;
            currentValue[c] = 0;
            delayCounter[c] = 0;
            triggered[c] = 0;
        }
    }

    if (controlsStale) {
        updateControls();
    }

    // Trigger signal
    for (int c = 0; c < 2; c++) {
        if (cables[3].val[c] > 2.0 / 3) {
            if (triggered[c] == 0) {
                delayCounter[c] = 0;
                phase[c] = 1;
            }
            triggered[c] = 1;
        }
        else if (cables[3].val[c] < 1.0 / 3) {
            if (triggered[c] == 1) {
                phase[c] = 0;
            }
            triggered[c] = 0;
        }
    }

    // Calculate
    for (int c = 0; c < 2; c++) {
        double rate = clamp(rates[int(phase[c])][c]);
        if (linear[c]) {
            switch (int(phase[c])) {
            case 1: // delay
                currentValue[c] = clamp(currentValue[c] - rate);
                delayCounter[c] += timeStep;
                if (delayCounter[c] >= delayTime[c]) phase[c] += 1;
                break;
            case 2: // attack
                currentValue[c] = clamp(currentValue[c] + rate);
                if (currentValue[c] >= 1) phase[c] += 1;
                break;
            case 3: // decay
                currentValue[c] = clamp(currentValue[c] - rate);
                if (currentValue[c] <= rates[4][c]) {
                    currentValue[c] = rates[4][c];
                    phase[c] += 1;
                }
                break;
            case 4: // sustain
                break;
            default: // release
                currentValue[c] = clamp(currentValue[c] - rate);
            }
        }
        else {
            rate = 1 - rate;
            switch (int(phase[c])) {
            case 1: // delay
                currentValue[c] = clamp(((currentValue[c] + 1) * rate) - 1);
                delayCounter[c] += timeStep;
                if (delayCounter[c] >= delayTime[c]) phase[c] += 1;
                break;
            case 2: // attack
                currentValue[c] = clamp(((currentValue[c] - 2) * rate) + 2);
                if (currentValue[c] >= 1) phase[c] += 1;
                break;
            case 3: // decay
                currentValue[c] = clamp(((currentValue[c] + 1) * rate) - 1, rates[4][c], 1.0);
                if (currentValue[c] <= rates[3][c]) {
                    currentValue[c] = rates[3][c];
                    phase[c] += 1;
                }
                break;
            case 4: // sustain
                break;
            default: // release
                currentValue[c] = clamp(((currentValue[c] + 1) * rate) - 1);
            }
        }
    }

    // Update outputs
    cables[4].val[0] = currentValue[0];
    cables[4].val[1] = currentValue[1];
    cables[0].val[0] = cables[1].val[0] * currentValue[0];
    cables[0].val[1] = cables[1].val[1] * currentValue[1];
    time += timeStep;
}