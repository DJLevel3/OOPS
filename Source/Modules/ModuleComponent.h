/*
  ==============================================================================

    ModuleComponent.h
    Created: 16 Mar 2025 11:45:01am
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Utility.h"

//==============================================================================
/*
*/

// Value, Name, Fresh, Input, Valid
struct CableConnection {
    double val[2] = { 0,0 };
    std::string name = "";
    bool fresh = false;
    bool input = false;
    bool valid = false;
};

// Value, Name, Stereo, Valid
struct ModuleControl {
    double val[2] = { 0,0 };
    std::string name = "";
    bool stereo = true;
    bool valid = false;
};

// Index, Input
struct CableIdentifier {
    int index;
    bool input = false;
};

// Standard Locations
// Controls:
// 0 - Transpose
// 1 - Tune
// 2 - Phase/Delay
// 3 - Waveform/Type
// 4 - Shape/Attack
// 5 - Decay/FM
// 6 - Sustain
// 7 - Release
// 8 - Clock Rate
// 9 - Clock Type
// 
// Cables:
// 0 - Output
// 1 - Input
// 2 - Pitch
// 3 - CV IN
// 4 - CV OUT
// 5 - Reset
// 6 - Clock
// 7 - FM1
// 8 - V/O
// 9 - V/O
//
class ModuleComponent : public juce::Component
{
public:
    ModuleComponent(double sampleRate);
    ~ModuleComponent() override;

    CableIdentifier findCable(std::string name);
    int findControl(std::string name);

    CableConnection getCable(int index);
    bool putCable(int index, CableConnection input);

    ModuleControl getControl(int index);
    bool putControl(int index, ModuleControl change);

    void setSampleRate(double sampleRate);

    virtual void reset() = 0;
    virtual void updateControls() = 0;
    virtual void run() = 0;

    static double clamp(double val, double min = 0, double max = 1) {
        return std::max(std::min(val, max), min);
    }

protected:
    std::vector<CableConnection> cables;
    std::vector<ModuleControl> controls;
    double sampleRate;
    double timeStep;
    double time = 0;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModuleComponent)
};
