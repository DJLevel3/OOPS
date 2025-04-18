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

//==============================================================================
/*
*/

/*
class Switch : public ModuleComponent
{
public:
    Switch(double sampleRate);
    ~Switch() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void updateControls() override;
    void run(int numVoices) override;
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
    std::vector<std::string> controlNames = { "Scale A", "Offset A", "Scale B", "Offset B", "Scale C", "Offset C", "Scale D", "Offset D", };
    std::vector<std::string> cableNames = { "A Out", "A In", "B Out", "B In", "C Out", "C In", "D Out", "D In" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::string sliderNames[6] = { "Numerator", "Denominator", "Phase", "FM", "Wave", "Shape" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Switch)
};
*/