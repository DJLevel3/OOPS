/*
  ==============================================================================

    ModuleComponent.cpp
    Created: 16 Mar 2025 11:45:01am
    Author:  DJ_Level_3

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModuleComponent.h"

//==============================================================================
ModuleComponent::ModuleComponent(double rate) : sampleRate(rate), timeStep(1.0 / rate)
{
}

ModuleComponent::~ModuleComponent()
{
}

void ModuleComponent::setSampleRate(double rate) {
    sampleRate = rate;
    timeStep = 1.0 / rate;
}

int ModuleComponent::findCable(std::string name) {
    int cable = -1;
    for (int i = 0; i < cables.size(); i++) {
        if (cables[i].name == name) {
            cable = i;
            break;
        }
    }
    return cable;
}

int ModuleComponent::findControl(std::string name) {
    int index = -1;
    for (int i = 0; i < controls.size(); i++) {
        if (controls[i].name == name) {
            index = i;
            break;
        }
    }
    return index;
}

CableConnection ModuleComponent::getCable(int index) {
    CableConnection cable;
    if (index >= cables.size() || index < 0) return cable;
    return cables[index];
}

bool ModuleComponent::putCable(int index, CableConnection input) {
    if (index >= cables.size() || index < 0) return false;
    //if (cables[index].input == false) return false;
    std::string cableName = cables[index].name;
    cables[index] = input;
    cables[index].name = cableName;
    return true;
}

ModuleControl ModuleComponent::getControl(int index) {
    ModuleControl control;
    if (index >= controls.size() || index < 0) return control;
    return controls[index];
}

bool ModuleComponent::putControl(int index, ModuleControl change) {
    if (index >= controls.size() || index < 0) return false;
    controls[index] = change;
    updateControls();
    return true;
}

