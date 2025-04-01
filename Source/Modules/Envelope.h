/*
  ==============================================================================

    Envelope.h
    Created: 16 Mar 2025 11:47:29am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModuleComponent.h"

//==============================================================================
/*
*/
class Envelope  : public ModuleComponent
{
public:
    Envelope(double sampleRate);
    ~Envelope() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void updateControls() override;
    void run(int numVoices) override;
    void automate(int channel, double newValue);

    juce::String getState();
    void setState(juce::String state);

    void reset() override {
        for (int voice = 0; voice < NUM_VOICES; voice++) reset(voice);
        time = 0;
    }
    void reset(int voice) override;
protected:
    double triggered[NUM_VOICES][2] = { 0 };
    double phase[NUM_VOICES][2] = { 0 };
    double currentValue[NUM_VOICES][2] = { 0 };
    double rates[5][2] = { 0 };
    double delayTime[2] = { 0 };
    double delayCounter[NUM_VOICES][2] = { 0 };
    bool linear[2];
    bool controlsStale = true;
private:
    std::string controlNames[8] = { "", "", "", "Shape", "Attack", "Decay", "Sustain", "Release" };
    std::vector<std::string> cableNames = { "Output", "Input", "", "Trigger", "Envelope", "Reset" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::string sliderNames[4] = { "A", "D", "S", "R" };
    juce::TextButton shapeButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Envelope)
};
