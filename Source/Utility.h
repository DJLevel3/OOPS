/*
  ==============================================================================

    Utility.h
    Created: 16 Mar 2025 12:11:18pm
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once

#include <vector>
#include <string>
#include <algorithm>

#define NUM_VOICES 8

#define PI  3.14159265359
#define TAU 6.28318530718

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