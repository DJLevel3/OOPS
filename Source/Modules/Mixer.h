/*
  ==============================================================================

    Mixer.h
    Created: 16 Mar 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModuleComponent.h"

#define MIXER_PAN_CURVE

class Mixer : public ModuleComponent
{
public:
    Mixer(double sampleRate);
    ~Mixer() override;

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
    double scale[2] = { 0,0 };
    bool controlsStale = true;
private:
    std::vector<std::string> controlNames = { "Mix Vol.", "Mix Bal.", "A Ampl.", "B Ampl.", "C Ampl.", "D Ampl." };
    std::vector<std::string> cableNames = { "Mix Out", "A In", "B In", "C In", "D In" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::vector<std::string> sliderNames = { "Mix Vol.", "Mix Bal.", "A Ampl.", "B Ampl.", "C Ampl.", "D Ampl." };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
