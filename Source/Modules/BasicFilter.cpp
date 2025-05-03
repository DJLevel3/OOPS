/*
  ==============================================================================

    BasicFilter.cpp
    Created: 16 Mar 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BasicFilter.h"

//==============================================================================
BasicFilter::BasicFilter(double sampleRate) : ModuleComponent(sampleRate)
{
    numAutomations = 4;
    moduleType = BasicFilterType;

    for (int i = 0; i < 4; i++) {
        sliders.push_back(new juce::Slider);
        sliderLabels.push_back(new juce::Label);
        addAndMakeVisible(sliders[i]);
        addAndMakeVisible(sliderLabels[i]);
        sliders[i]->setRange(0.0, 1.0, 0.0001);
        sliders[i]->setValue(0);
        sliders[i]->setSliderStyle(juce::Slider::LinearBarVertical);
        sliders[i]->setDoubleClickReturnValue(true, 0);
        sliderLabels[i]->attachToComponent(sliders[i], false);
        sliderLabels[i]->setText(sliderNames[i], juce::dontSendNotification);
        sliderLabels[i]->setJustificationType(juce::Justification::centredBottom);
    }

    addAndMakeVisible(lowHighButton);
    lowHighButton.setButtonText("Low Pass");
    lowHighButton.setClickingTogglesState(true);

    addAndMakeVisible(fourPoleButton);
    fourPoleButton.setButtonText("2-Pole");
    fourPoleButton.setClickingTogglesState(true);

    sliders[0]->onValueChange = [this] { double v = sliders[0]->getValue(); controls[0].val[0] = v; controls[0].val[1] = v; controlsStale = true; dawDirty.push_back(0); };
    sliders[1]->onValueChange = [this] { double v = sliders[1]->getValue(); controls[1].val[0] = v; controls[1].val[1] = v; controlsStale = true; dawDirty.push_back(1); };
    sliders[2]->onValueChange = [this] { double v = sliders[2]->getValue(); controls[2].val[0] = v; controls[2].val[1] = v; controlsStale = true; dawDirty.push_back(2); };
    sliders[3]->onValueChange = [this] { double v = sliders[3]->getValue(); controls[3].val[0] = v; controls[3].val[1] = v; controlsStale = true; dawDirty.push_back(3); };

    lowHighButton.onClick = [this] {
        highPass = lowHighButton.getToggleState();
        lowHighButton.setButtonText(highPass ? "High pass" : "Low Pass");
        controlsStale = true;
        dawDirty.push_back(4);
        for (int i = 0; i < NUM_VOICES; i++) {
            buf0[i][0] = 0;
            buf1[i][1] = 0;
        }
    };

    fourPoleButton.onClick = [this] {fourPole = fourPoleButton.getToggleState(); controlsStale = true; dawDirty.push_back(5); fourPoleButton.setButtonText(fourPole ? "4-Pole" : "2-Pole"); };

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

    dawDirty.clear();

    dawDirty.push_back(0);
}

BasicFilter::~BasicFilter()
{
    for (int i = 0; i < sliders.size(); i++) {
        delete sliders[i];
        delete sliderLabels[i];
    }
}

void BasicFilter::paint(juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(14.0f));
    g.drawText(ModuleStrings.at(moduleType), getLocalBounds().removeFromTop(20),
        juce::Justification::centred, true);   // draw some placeholder text
}

void BasicFilter::updateControls() {
    dawDirty.clear();
    dawDirty.push_back(0);
    controlsStale = false;
}

void BasicFilter::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    area.removeFromTop(20);
    auto bottom = area.removeFromBottom(25);
    lowHighButton.setBounds(juce::Rectangle<int>(50, 20).withCentre(bottom.removeFromLeft(bottom.getWidth()/2).getCentre()));
    fourPoleButton.setBounds(juce::Rectangle<int>(50, 20).withCentre(bottom.getCentre()));

    area.expand(-5, -5);
    auto left = area.removeFromLeft(area.getWidth() / 2);


    for (int i = 0; i < sliders.size(); i++) {
        sliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, left.getWidth() - 10, 20);
    }

    auto top = left.removeFromTop(left.getHeight() * 2 / 3).expanded(0, -3);
    sliderLabels[0]->setBounds(top.removeFromTop(20));
    sliders[0]->setBounds(top);
    top = area.removeFromTop(area.getHeight() * 2 / 3).expanded(0, -3);
    sliderLabels[1]->setBounds(top.removeFromTop(20));
    sliders[1]->setBounds(top);

    top = left.removeFromTop(left.getHeight()).expanded(0, -3);
    sliderLabels[2]->setBounds(top.removeFromTop(20));
    sliders[2]->setBounds(top);
    top = area.removeFromTop(area.getHeight()).expanded(0, -3);
    sliderLabels[3]->setBounds(top.removeFromTop(20));
    sliders[3]->setBounds(top);
}

void BasicFilter::reset(int voice) {
    buf0[voice][0] = 0;
    buf0[voice][1] = 0;
    buf1[voice][0] = 0;
    buf1[voice][1] = 0;
    buf2[voice][0] = 0;
    buf2[voice][1] = 0;
    buf3[voice][0] = 0;
    buf3[voice][1] = 0;
}

void BasicFilter::run(int numVoices) {
    if (numVoices > NUM_VOICES) numVoices = NUM_VOICES;
    if (controlsStale) updateControls();


    for (int v = 0; v < numVoices; v++) {
        for (int c = 0; c < 2; c++) {
            double cut = sqrt(juce::jmax(juce::jmin(((controls[0].val[c] * controls[0].val[c]) + controls[2].val[c] * cables[2].val[v][c]) * 48000 / sampleRate, 0.9999), 0.));
            double res = controls[1].val[c] + controls[3].val[c] * cables[3].val[v][c];

            // filter that mofo
            buf0[v][c] += cut * (cables[1].val[v][c] - buf0[v][c] + juce::jmax(juce::jmin(((buf0[v][c] - (buf1[v][c])) * (res + res / (1 - cut))), 2.), -2.));
            buf1[v][c] += cut * (buf0[v][c] - buf1[v][c]);
            buf2[v][c] += cut * (buf1[v][c] - buf2[v][c]);
            buf3[v][c] += cut * (buf2[v][c] - buf3[v][c]);

            // clip to make sure things don't get out of hand
            /*
            buf0[v][c] = juce::jmax(juce::jmin(buf0[v][c], 1.), -1.);
            buf1[v][c] = juce::jmax(juce::jmin(buf1[v][c], 1.), -1.);
            buf2[v][c] = juce::jmax(juce::jmin(buf2[v][c], 1.), -1.);
            buf3[v][c] = juce::jmax(juce::jmin(buf3[v][c], 1.), -1.);
            */
            if (!highPass) {
                cables[0].val[v][c] = juce::jmax(juce::jmin(fourPole ? buf3[v][c] : buf1[v][c], 5.), -5.);
            }
            else {
                cables[0].val[v][c] = juce::jmax(juce::jmin(cables[1].val[v][c] - (fourPole ? buf3[v][c] : buf1[v][c]), 5.), -5.);
            }
        }
    }

    time += timeStep;
}

void BasicFilter::automate(int channel, double newValue) {
    if (channel < 0 || channel >= 4) {
        return;
    }
    int s = channel;
    double h = floor((sliders[s]->getMaximum() - sliders[s]->getMinimum()) * (newValue) / sliders[s]->getInterval()) * sliders[s]->getInterval();
    double v = (sliders[s]->getMinimum() + h);
    controls[s].val[0] = v;
    controls[s].val[1] = v;
    juce::MessageManager::callAsync([this, s, v]() {sliders[s]->setValue(v, juce::sendNotificationSync); });
    controlsStale = true;
    dawDirty.clear();
    dawDirty.push_back(0);
}

juce::String BasicFilter::getState() {
    juce::String stateString = "";
    for (int slider = 0; slider < (int)sliders.size(); slider++) {
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
        stateString.append(juce::String(sliders[slider]->getValue(), 0, false), 10);
        stateString.append(":", 1);
    }
    stateString.append((highPass ? (fourPole ? "A" : "B") : (fourPole ? "C" : "D")), 5);
    return stateString;
}

void BasicFilter::setState(juce::String state) {
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
    switch (state.getLastCharacter()) {
    case 'A':
        highPass = true;
        fourPole = true;
        break;
    case 'B':
        highPass = true;
        fourPole = false;
        break;
    case 'C':
        highPass = false;
        fourPole = true;
        break;
    case 'D':
        highPass = false;
        fourPole = false;
        break;
    }
    lowHighButton.setToggleState(highPass, juce::NotificationType::sendNotificationSync);
    fourPoleButton.setToggleState(fourPole, juce::NotificationType::sendNotificationSync);
}