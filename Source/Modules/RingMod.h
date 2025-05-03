/*
  ==============================================================================

    RingMod.h
    Created: 16 Mar 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModuleComponent.h"

//==============================================================================
/*
*/
class RingMod  : public ModuleComponent
{
public:
    RingMod(double sampleRate);
    ~RingMod() override;

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
    juce::Slider factor;
    juce::Slider factorMod;
    juce::Label factorText;
    juce::Label factorModText;
private:
    std::vector<std::string> controlNames = { "Factor", "CV Mod" };
    std::vector<std::string> cableNames = { "Output", "Carrier", "Modulator", "Factor"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingMod)
};
