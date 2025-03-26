/*
  ==============================================================================

    Modules.h
    Created: 16 Mar 2025 1:31:39pm
    Author:  DJ_Level_3

  ==============================================================================
*/

#pragma once
#include "ModuleComponent.h"
#include "Oscillator.h"
#include "HarmonicOscillator.h"
#include "Envelope.h"
#include "RingMod.h"

enum ModuleType {
    OscillatorType,
    HarmonicOscillatorType,
    EnvelopeType,
    RingModType
};