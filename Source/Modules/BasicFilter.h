/*
  ==============================================================================

    BasicFilter.h
    Created: 23 Apr 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModuleComponent.h"

//==============================================================================
/*
*/
class BasicFilter : public ModuleComponent
{
public:
    BasicFilter(double sampleRate);
    ~BasicFilter() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void updateControls() override;
    void run(int numVoices) override;
    void automate(int channel, double newValue);
    std::string getCableName(int cableNumber) override {
        if (cableNumber >= cableNames.size()) return "";
        return cableNames[cableNumber];
    }

    juce::String getState();
    void setState(juce::String state);
    void reset() override {
        for (int voice = 0; voice < NUM_VOICES; voice++) reset(voice);
        time = 0;
    }
    void reset(int voice) override;
protected:
    bool highPass = false;
    bool fourPole = false;
    double buf0[NUM_VOICES][2] = { 0 };
    double buf1[NUM_VOICES][2] = { 0 };
    double buf2[NUM_VOICES][2] = { 0 };
    double buf3[NUM_VOICES][2] = { 0 };
    bool controlsStale = true;
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::string sliderNames[4] = { "Cutoff", "Resonance", "CMod", "RMod" };
    juce::TextButton lowHighButton;
    juce::TextButton fourPoleButton;
private:
    std::vector<std::string> controlNames = { "Cutoff", "Resonance", "CMod", "RMod"};
    std::vector<std::string> cableNames = { "Output", "Input", "Cutoff", "Resonance" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicFilter)
};
