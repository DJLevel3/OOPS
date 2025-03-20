/*
  ==============================================================================

    VCA.h
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
class VCA  : public ModuleComponent
{
public:
    VCA(double sampleRate);
    ~VCA() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void run() override;
    void reset() override;
protected:
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VCA)
};
