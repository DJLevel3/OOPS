/*
  ==============================================================================

    Oscillator.h
    Created: 16 Mar 2025 11:47:04am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModuleComponent.h"

//==============================================================================
/*
*/
class Oscillator  : public ModuleComponent
{
public:
    Oscillator(double sampleRate);
    ~Oscillator() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void updateControls() override;
    void run() override;
    void reset() override;
protected:
    double basePitch[2] = { 261.63, 261.63 };
    double frequency[2] = { 0, 0 };
    double phase[2] = { 0, 0 };
    double calculateChannel(int channel);
    bool controlsStale = true;
    bool mono = false;
private:
    std::string controlNames[6] = { "Transpose", "Tune", "Phase", "Waveform", "Shape", "FM" };
    std::string cableNames[8] = { "Output", "Input", "Pitch", "PWM", "", "", "", "FM"};
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::string sliderNames[6] = { "Pitch", "Detune", "Phase", "FM", "Wave", "Shape" };
    juce::TextButton stereoButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator)
};
