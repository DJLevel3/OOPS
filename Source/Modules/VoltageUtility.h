/*
  ==============================================================================

    VoltageUtility.h
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
class VoltageUtility : public ModuleComponent
{
public:
    VoltageUtility(double sampleRate);
    ~VoltageUtility() override;

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
    std::vector<std::string> controlNames = { "Scale A", "Offset A", "Scale B", "Offset B", "Scale C", "Offset C" };
    std::vector<std::string> cableNames = { "A Out", "A In", "B Out", "B In", "C Out", "C In" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::vector<std::string> sliderNames = { "Scale A", "Offset A", "Scale B", "Offset B", "Scale C", "Offset C" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoltageUtility)
};
