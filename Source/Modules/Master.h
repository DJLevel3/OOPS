/*
  ==============================================================================

    Master.h
    Created: 27 Mar 2025 10:29:09am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModuleComponent.h"

//==============================================================================
/*
*/
class Master  : public ModuleComponent
{
public:
    Master(double sampleRate);
    ~Master() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void run(int numVoices) override;
    void automate(int channel, double newValue);

    juce::String getState();
    void setState(juce::String state);
    void reset() override {
        for (int voice = 0; voice < NUM_VOICES; voice++) reset(voice);
        time = 0;
    }
    void reset(int voice) override;
    void updateControls() override;
protected:
    double actualPitch[NUM_VOICES][2] = { 0 };
    double targetPitch[NUM_VOICES][2] = { 0 };
private:
    std::string controlNames[3] = { "Volume", "Octave", "Glide" };
    std::vector<std::string> cableNames = { "Input", "Output", "Pitch", "Gate" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::vector<std::string> sliderNames = { "Volume", "Octave", "Glide" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Master)
};
