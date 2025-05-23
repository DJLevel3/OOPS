/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Modules/Modules.h"
#include "Utility.h"

struct CableMap {
    int sourceM = 0;
    int sourceC = 0;
    int destM = 0;
    int destC = 0;
};

struct Note {
    int noteNumber;
    int voiceNumber;
};

struct Automator {
    int modN = -1;
    int autoN = -1;
    float lastValue = 0;
    float currentValue = 0;
    bool initialized = false;
    juce::AudioParameterFloat* param;
};

class OOPSAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    OOPSAudioProcessor();
    ~OOPSAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    int insertModule(int modDest, ModuleComponent* m);
    int insertNewModule(int modDest, ModuleType modType);
    void removeModule(int modDest);
    int moveModule(int modDest, int modSource);

    void insertCable(int sourceM, int sourceC, int destM, int destC);
    void removeCable(int cable);

    juce::String getAutomationName(int controlNumber);
    void addAutomation(int automationNumber, int controlNumber);

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    static bool compareCableMapsSource(const CableMap& map1, const CableMap& map2) {
        return map1.sourceM < map2.sourceM;
    }
    static bool compareCableMapsDest(const CableMap& map1, const CableMap& map2) {
        return map1.destM < map2.destM;
    }

    std::vector<ModuleComponent*> processingOrder;
    std::vector<CableMap> plugs;

    double sampleRateMemory = 96000;
    int voiceLimit = NUM_VOICES;
    int lastVoiceLimit = 0;
private:
    void actuallyRemoveModule(int modDest);
    void actuallyMoveModule(int modDest, int modSource);
    void defaultInit();
    void stateInit(std::vector<ModuleType> moduleTypes, std::vector<CableMap> cables);
    void clearProcessingOrder();
    std::vector<int> markedForRemoval;
    std::vector<std::vector<int>> markedForMovement;
    int latestVoice = NUM_VOICES-1;
    std::vector<Note> notes;
    int wave = 1;
    
    Automator automators[MAX_AUTOMATIONS];
    juce::AudioParameterBool* doNothingButUpdateTheDawLMAO;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OOPSAudioProcessor)
};
