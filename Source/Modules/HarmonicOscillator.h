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
class HarmonicOscillator : public ModuleComponent
{
public:
    HarmonicOscillator(double sampleRate);
    ~HarmonicOscillator() override;

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
    double basePitch[2] = { 261.63, 261.63 };
    double frequency[NUM_VOICES][2] = { 0, 0 };
    double phase[NUM_VOICES][2] = { 0, 0 };
    bool controlsStale = true;
    bool mono = false;
private:
    std::string controlNames[6] = { "Numerator", "Denominator", "Phase", "Waveform", "Shape", "FM" };
    std::vector<std::string> cableNames = { "Output", "Input", "Pitch", "PWM", "", "", "", "FM" };
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::string sliderNames[6] = { "Numerator", "Denominator", "Phase", "FM", "Wave", "Shape" };
    juce::TextButton stereoButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HarmonicOscillator)
};
