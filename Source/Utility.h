/*
  ==============================================================================

    Utility.h
    Created: 16 Mar 2025 12:11:18pm
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <vector>
#include <map>
#include <string>
#include <algorithm>

#define NUM_VOICES 8

#define MAX_AUTOMATIONS 16

#define PI  3.14159265359
#define TAU 6.28318530718

// New modules MUST be added at the end of the list (before UndefinedType), or saved states break!
// Add menu ordering is done in the ModuleOrder map instead.
enum ModuleType {
    // THIS MUST BE FIRST
    NullType,

    // Types here
    MasterType,
    OscillatorType,
    HarmonicOscillatorType,
    EnvelopeType,
    RingModType,
    SwitchType,
    VoltageUtilityType,

    // THIS MUST BE LAST
    UndefinedType
};

// Ordering in the menu to add a module
const std::map<int, ModuleType> ModuleOrder = {
    {0, OscillatorType},
    {1, HarmonicOscillatorType},
    {2, RingModType},
    {3, EnvelopeType},
    {4, VoltageUtilityType}
};

const std::map<ModuleType, juce::String> ModuleStrings = {
    {MasterType, "Master"},
    {OscillatorType, "Oscillator"},
    {HarmonicOscillatorType, "Harmonic Oscillator"},
    {EnvelopeType, "Envelope"},
    {RingModType, "Ring Mod"},
    {SwitchType, "Switch"},
    {VoltageUtilityType, "Utility"}
};