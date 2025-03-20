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
    void run() override;
    void reset() override;
protected:
    double triggered[2] = { 0,0 };
    double phase[2] = { 0,0 };
    double currentValue[2] = { 0.0 };
    double rates[5][2] = { {0,0}, {0,0}, {0,0}, {0,0}, {0,0} };
    double delayTime[2] = { 0,0 };
    double delayCounter[2] = { 0,0 };
    bool linear[2];
    bool controlsStale = true;
private:
    std::string controlNames[8] = { "", "", "Delay", "Shape", "Attack", "Decay", "Sustain", "Release" };
    std::string cableNames[6] = { "Output", "Input", "", "Trigger", "Envelope", "Reset" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::string sliderNames[5] = { "d", "A", "D", "S", "R" };
    juce::TextButton shapeButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Envelope)
};
