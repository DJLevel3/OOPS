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
    double basePitch[2] = { 261.63, 261.63 };
    double frequency[NUM_VOICES][2] = { 0, 0 };
    double phase[NUM_VOICES][2] = { 0, 0 };
    bool controlsStale = true;
    bool mono = false;
private:
    std::string controlNames[6] = { "Transpose", "Tune", "Phase", "Waveform", "Shape", "FM" };
    std::vector<std::string> cableNames = { "Output", "Input", "Pitch", "PWM", "FM"};
    std::vector<juce::Slider*> sliders;
    std::vector<juce::Label*> sliderLabels;
    std::string sliderNames[6] = { "Pitch", "Detune", "Phase", "FM", "Wave", "Shape" };
    juce::TextButton stereoButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oscillator)
};
