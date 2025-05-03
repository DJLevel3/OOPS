/*
  ==============================================================================

    Switch.h
    Created: 16 Mar 2025 11:48:31am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModuleComponent.h"

#define SWITCH_EQUAL_LOUDNESS

//==============================================================================
/*
*/
class Switch : public ModuleComponent
{
public:
    Switch(double sampleRate);
    ~Switch() override;

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
    std::vector<std::string> controlNames = { "Scale A-L", "Scale A-R", "Scale B-L", "Scale B-R", "Swap L-R", "Swap AL-BL" };
    std::vector<std::string> cableNames = { "Mix Out", "A In", "A Out", "B In", "B Out" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::vector<std::string> sliderNames = { "Scale A-L", "Scale A-R", "Scale B-L", "Scale B-R", "Swap L-R", "Swap AL-BL" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Switch)
};
