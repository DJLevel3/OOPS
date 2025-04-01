/*
  ==============================================================================

    Envelope.cpp
    Created: 16 Mar 2025 11:47:29am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include "Envelope.h"

//==============================================================================
Envelope::Envelope(double sampleRate) : ModuleComponent(sampleRate)
{
    numAutomations = 5;
    moduleType = EnvelopeType;
    needsGate = true;
    for (int i = 0; i < 4; i++) {
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

    sliders[0]->onValueChange = [this] { double v = sliders[0]->getValue(); controls[4].val[0] = v; controls[4].val[1] = v; controlsStale = true; dawDirty.push_back(0); };
    sliders[1]->onValueChange = [this] { double v = sliders[1]->getValue(); controls[5].val[0] = v; controls[5].val[1] = v; controlsStale = true; dawDirty.push_back(1); };
    sliders[2]->onValueChange = [this] { double v = sliders[2]->getValue(); controls[6].val[0] = v; controls[6].val[1] = v; controlsStale = true; dawDirty.push_back(2); };
    sliders[3]->onValueChange = [this] { double v = sliders[3]->getValue(); controls[7].val[0] = v; controls[7].val[1] = v; controlsStale = true; dawDirty.push_back(3); };

    shapeButton.onClick = [this] { bool v = shapeButton.getToggleState(); controls[3].val[0] = v; controls[3].val[1] = v; shapeButton.setButtonText(v ? "Linear" : "Analog"); controlsStale = true; dawDirty.push_back(5); };

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
    sliders[2]->setValue(1, juce::sendNotificationSync);
    sliders[3]->setValue(0, juce::sendNotificationSync);

    reset();
}

Envelope::~Envelope()
{
    for (int i = 0; i < sliders.size(); i++) {
        delete sliders[i];
        delete sliderLabels[i];
    }
}

void Envelope::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));  // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText(ModuleStrings.at(moduleType), getLocalBounds().removeFromTop(20) ,
                juce::Justification::centred, true);
}

void Envelope::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    area.removeFromTop(20);
    auto bottom = area.removeFromBottom(25);
    shapeButton.setBounds(juce::Rectangle<int>(100, 20).withCentre(bottom.getCentre()));
    area.expand(-5, -5);
    int width = area.getWidth() / 2;
    bottom = area.removeFromBottom(area.getHeight()/2);
    bottom.removeFromTop(25);
    area.removeFromTop(25);
    sliders[0]->setBounds(area.removeFromLeft(width).expanded(-10, -2));
    sliders[1]->setBounds(area.expanded(-10, -2));
    sliders[2]->setBounds(bottom.removeFromLeft(width).expanded(-10, -2));
    sliders[3]->setBounds(bottom.expanded(-10, -2));
}

void Envelope::reset(int voice) {
    phase[voice][0] = 0;
    phase[voice][1] = 0;
    currentValue[voice][0] = 0;
    currentValue[voice][1] = 0;
    delayCounter[voice][0] = 0;
    delayCounter[voice][1] = 0;
    triggered[voice][0] = 0;
    triggered[voice][1] = 0;
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

void Envelope::automate(int channel, double newValue) {
    if (channel < sliders.size()) {
        sliders[channel]->setValue(newValue, juce::sendNotificationSync);
    }
    else if (channel == sliders.size()) shapeButton.setToggleState(newValue >= 1, juce::sendNotificationSync);
}

void Envelope::run(int numVoices) {
    if (numVoices > NUM_VOICES) numVoices = NUM_VOICES;
    if (controlsStale) updateControls();

    // Reset signal
    for (int voice = 0; voice < numVoices; voice++) {
        for (int c = 0; c < 2; c++) {
            if (cables[5].val[voice][c] >= 1) {
                phase[voice][c] = 0;
                currentValue[voice][c] = 0;
                delayCounter[voice][c] = 0;
                triggered[voice][c] = 0;
            }
        }


        // Trigger signal
        for (int c = 0; c < 2; c++) {
            if (cables[3].val[voice][c] > 2.0 / 3) {
                if (triggered[voice][c] == 0) {
                    delayCounter[voice][c] = 0;
                    phase[voice][c] = 1;
                }
                triggered[voice][c] = 1;
            }
            else if (cables[3].val[voice][c] < 1.0 / 3) {
                if (triggered[voice][c] == 1) {
                    phase[voice][c] = 0;
                }
                triggered[voice][c] = 0;
            }
        }

        // Calculate
        for (int c = 0; c < 2; c++) {
            double rate = clamp(rates[int(phase[voice][c])][c]);
            if (linear[c]) {
                switch (int(phase[voice][c])) {
                case 1: // delay
                    currentValue[voice][c] = clamp(currentValue[voice][c] - rate);
                    delayCounter[voice][c] += timeStep;
                    if (delayCounter[voice][c] >= delayTime[c]) phase[voice][c] += 1;
                    break;
                case 2: // attack
                    currentValue[voice][c] = clamp(currentValue[voice][c] + rate);
                    if (currentValue[voice][c] >= 1) phase[voice][c] += 1;
                    break;
                case 3: // decay
                    currentValue[voice][c] = clamp(currentValue[voice][c] - rate);
                    if (currentValue[voice][c] <= rates[4][c]) {
                        currentValue[voice][c] = rates[4][c];
                        phase[voice][c] += 1;
                    }
                    break;
                case 4: // sustain
                    break;
                default: // release
                    currentValue[voice][c] = clamp(currentValue[voice][c] - rate);
                }
            }
            else {
                rate = 1 - rate;
                switch (int(phase[voice][c])) {
                case 1: // delay
                    currentValue[voice][c] = clamp(((currentValue[voice][c] + 1) * rate) - 1);
                    delayCounter[voice][c] += timeStep;
                    if (delayCounter[voice][c] >= delayTime[c]) phase[voice][c] += 1;
                    break;
                case 2: // attack
                    currentValue[voice][c] = clamp(((currentValue[voice][c] - 2) * rate) + 2);
                    if (currentValue[voice][c] >= 1) phase[voice][c] += 1;
                    break;
                case 3: // decay
                    currentValue[voice][c] = clamp(((currentValue[voice][c] + 1) * rate) - 1, rates[4][c], 1.0);
                    if (currentValue[voice][c] <= rates[3][c]) {
                        currentValue[voice][c] = rates[3][c];
                        phase[voice][c] += 1;
                    }
                    break;
                case 4: // sustain
                    break;
                default: // release
                    currentValue[voice][c] = clamp(((currentValue[voice][c] + 1) * rate) - 1);
                }
            }
        }

        // Update outputs
        cables[4].val[voice][0] = currentValue[voice][0];
        cables[4].val[voice][1] = currentValue[voice][1];
        cables[0].val[voice][0] = cables[1].val[voice][0] * currentValue[voice][0];
        cables[0].val[voice][1] = cables[1].val[voice][1] * currentValue[voice][1];
    }
    time += timeStep;
}

juce::String Envelope::getState() {
    juce::String stateString = "";
    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
    }
    stateString.append((linear[0] ? "L" : "E"), 5);
    return stateString;
}

void Envelope::setState(juce::String state) {
    controlsStale = true;
    juce::StringArray array;
    array.addTokens(state, ":", "");
    if (array.size() < (int)sliders.size() * 2 + 1) {
        for (int slider = 0; slider < (int)sliders.size(); slider++) {
            sliders[slider]->setValue(0.0, juce::sendNotificationSync);
        }
        return;
    }

    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        sliders[slider]->setValue(array[slider * 2].getDoubleValue(), juce::sendNotificationSync);
    }
    linear[0] = state.getLastCharacter() == 'L';
    linear[1] = linear[0];
    shapeButton.setToggleState(linear[0], juce::NotificationType::sendNotificationSync);
}