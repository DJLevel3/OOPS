/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OOPSAudioProcessor::OOPSAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    std::vector<ModuleType> initModuleTypes = {
        MasterType,
        HarmonicOscillatorType,
        OscillatorType,
        EnvelopeType,
        HarmonicOscillatorType,
        RingModType,
        VoltageUtilityType
    };

    std::vector<CableMap> initCables = {
        { 0, 2, 1, 2 },
        { 0, 2, 2, 2 },
        { 0, 2, 4, 2 },
        { 0, 3, 1, 3 },
        { 0, 3, 2, 3 },
        { 0, 3, 3, 3 },
        { 0, 3, 4, 3 },
        { 1, 0, 2, 7 },
        { 2, 0, 3, 1 },
        { 3, 0, 5, 1 },
        { 4, 0, 5, 2 },
        { 5, 0, 0, 1 },
    };
    stateInit(initModuleTypes, initCables);
}

OOPSAudioProcessor::~OOPSAudioProcessor()
{
    clearProcessingOrder();
}

void OOPSAudioProcessor::defaultInit() {
    std::vector<ModuleType> moduleTypes = {
        MasterType,
        OscillatorType,
        EnvelopeType,
    };

    std::vector<CableMap> cables = {
        { 0, 2, 1, 2 },
        { 0, 3, 1, 3 },
        { 0, 3, 2, 3 },
        { 1, 0, 2, 1 },
        { 2, 0, 0, 1 },
    };
    stateInit(moduleTypes, cables);
}

void OOPSAudioProcessor::stateInit(std::vector<ModuleType> moduleTypes, std::vector<CableMap> cables) {
    if (moduleTypes[0] != MasterType) return defaultInit();
    clearProcessingOrder();
    for (ModuleType m : moduleTypes) {
        int i = insertNewModule(-3, m);
        if (i >= 0 && i < processingOrder.size()) {
            processingOrder[i]->reset();
        }
    }
    for (CableMap c : cables) {
        if (c.sourceM < processingOrder.size() && c.destM < processingOrder.size()) plugs.push_back(c);
    }
    std::sort(plugs.begin(), plugs.end(), compareCableMapsSource);
}

void OOPSAudioProcessor::clearProcessingOrder() {
    plugs.clear();
    for (int i = (int)processingOrder.size() - 1; i >= 0; i--) {
        delete processingOrder[i];
        processingOrder.erase(processingOrder.begin() + i);
    }
}

//==============================================================================
const juce::String OOPSAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OOPSAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OOPSAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OOPSAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OOPSAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OOPSAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OOPSAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OOPSAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OOPSAudioProcessor::getProgramName (int index)
{
    return {};
}

void OOPSAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void OOPSAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    std::unique_ptr<juce::XmlElement> xml = std::make_unique<juce::XmlElement>("OOPS");
    xml->setAttribute("version", ProjectInfo::versionNumber);

    auto moduleList = xml->createNewChildElement("moduleList");
    auto controlList = xml->createNewChildElement("controlList");
    juce::String mList = "";
    juce::String cList = "";
    for (ModuleComponent* m : processingOrder) {
        mList.append(juce::String(static_cast<int>(m->moduleType)), 5);
        mList.append(",", 5);
        cList.append(m->getState(), 2048);
        cList.append(",", 5);
    }
    mList = mList.trimCharactersAtEnd(",");
    cList = cList.trimCharactersAtEnd(",");
    moduleList->setAttribute("list", mList);
    controlList->setAttribute("list", cList);

    auto cableList = xml->createNewChildElement("cableList");
    cList = "";
    for (CableMap p : plugs) {
        cList.append(juce::String(p.sourceM), 5);
        cList.append(":", 1);
        cList.append(juce::String(p.sourceC), 5);
        cList.append(":", 1);
        cList.append(juce::String(p.destM), 5);
        cList.append(":", 1);
        cList.append(juce::String(p.destC), 5);
        cList.append(",", 1);
    }
    cList = cList.trimCharactersAtEnd(",");
    cableList->setAttribute("list", cList);

    auto globalList = xml->createNewChildElement("globalList");
    globalList->setAttribute("voiceLimit", voiceLimit);

    copyXmlToBinary(*xml, destData);
}

void OOPSAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    if (xml.get() != nullptr && xml->hasTagName("OOPS")) {
        // Check to make sure version is current or older
        auto version = (xml->hasAttribute("version")) ? (xml->getIntAttribute("version")) : ProjectInfo::versionNumber;
        if (version > ProjectInfo::versionNumber) return defaultInit();

        // Ensure we have a module list
        auto moduleList = xml->getChildByName("moduleList");
        if (moduleList == nullptr) return defaultInit();

        // Ensure we have a plug list
        auto cableList = xml->getChildByName("cableList");
        if (cableList == nullptr) return defaultInit();

        // Get optional control value list
        auto controlList = xml->getChildByName("controlList");

        // Get optional global list
        auto globalList = xml->getChildByName("globalList");

        juce::StringArray list;

        // Module types
        std::vector<ModuleType> moduleTypes;
        list.addTokens(moduleList->getStringAttribute("list"), ",", "");
        for (int i = 0; i < (int)list.size(); i++) {
            juce::String mod = list[i];
            int type = mod.getIntValue();
            if (type >= static_cast<int>(UndefinedType) || type <= static_cast<int>(NullType)) continue;
            moduleTypes.push_back(static_cast<ModuleType>(type));
        }
        list.clear();

        // Cable mappings
        std::vector<CableMap> cableMaps;
        list.addTokens(cableList->getStringAttribute("list"), ",", "");
        for (int i = 0; i < (int)list.size(); i++) {
            juce::String map = list[i];
            juce::StringArray sublist;
            sublist.addTokens(map, ":", "");
            if (sublist.size() == 4) {
                cableMaps.push_back(CableMap{ sublist[0].getIntValue(), sublist[1].getIntValue(), sublist[2].getIntValue(), sublist[3].getIntValue() });
            }
        }
        list.clear();

        stateInit(moduleTypes, cableMaps);

        if (controlList != nullptr) {
            list.addTokens(controlList->getStringAttribute("list"), ",", "");
            if (list.size() == processingOrder.size()) {
                for (int i = 0; i < (int)processingOrder.size(); i++) {
                    processingOrder[i]->setState(list[i]);
                }
            }
        }

        if (globalList != nullptr) {
            voiceLimit = globalList->getIntAttribute("voiceLimit", NUM_VOICES);
        }
    }
    else {
        return defaultInit();
    }
}

//==============================================================================
void OOPSAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    for (int i = 0; i < processingOrder.size(); i++) {
        processingOrder[i]->setSampleRate(sampleRate);
    }
    sampleRateMemory = sampleRate;
}

void OOPSAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OOPSAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OOPSAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    std::vector<int> noteOnTimes;
    std::vector<int> noteOnNumbers;
    std::vector<int> noteOffTimes;
    std::vector<int> noteOffNumbers;
    for (auto it : midiMessages)
    {
        auto msg = it.getMessage();
        if (msg.isNoteOn()) {
            noteOnTimes.push_back((int)msg.getTimeStamp());
            noteOnNumbers.push_back(msg.getNoteNumber());
        }
        if (msg.isNoteOff()) {
            noteOffTimes.push_back((int)msg.getTimeStamp());
            noteOffNumbers.push_back(msg.getNoteNumber());
        }
    }
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    int stampOn = 0;
    int stampOff = 0;
    CableConnection pitch = { {0}, "Pitch", true, true, true };
    CableConnection gateOn = { {0}, "Trigger", true, true, true };
    CableConnection gateOff = { {0}, "Trigger", true, true, true };
    std::vector<int> panic;
    std::vector<int> notesOn;
    std::vector<int> notesOff;
    if (lastVoiceLimit != voiceLimit) {
        notes.clear();
        latestVoice = 0;
        for (int i = 0; i < processingOrder.size(); i++) {
            processingOrder[i]->reset();
        }
        lastVoiceLimit = voiceLimit;
    }
    for (int s = 0; s < buffer.getNumSamples(); s++) {
        if (noteOnTimes.size() > 0) {
            while (s == noteOnTimes[stampOn]) {
                if (notes.size() == voiceLimit) {
                    panic.push_back(notes[0].voiceNumber);
                    notes.erase(notes.begin());
                }

                bool voiceAvailable = false;
                int startVoice = latestVoice;
                while (!voiceAvailable) {
                    latestVoice = (latestVoice + 1) % voiceLimit;
                    voiceAvailable = true;
                    for (int i = 0; i < notes.size(); i++) {
                        voiceAvailable = voiceAvailable && (notes[i].voiceNumber != latestVoice);
                    }
                    if (latestVoice == (startVoice % voiceLimit) && !voiceAvailable) {
                        break;
                    }
                }

                Note n = { noteOnNumbers[stampOn], latestVoice};
                notes.push_back(n);;

                notesOn.push_back(noteOnNumbers[stampOn]);
                stampOn = (stampOn + 1) % noteOnTimes.size();
                if (stampOn == 0) break;
            }
        }
        if (noteOffTimes.size() > 0) {
            while (s == noteOffTimes[stampOff]) {
                notesOff.push_back(noteOffNumbers[stampOff]);
                stampOff = (stampOff + 1) % noteOffTimes.size();
                if (stampOff == 0) break;
            }
        }

        int j = 0;
        for (int i = 0; i < processingOrder.size(); i++) {
            for (int k = 0; k < panic.size(); k++) {
                processingOrder[i]->reset(panic[k]);
            }
            if (notesOn.size() > 0) {
                for (int k = 0; k < notesOn.size(); k++) {
                    int index = (((latestVoice + k - notesOn.size() + 1) % voiceLimit) + voiceLimit) % voiceLimit;
                    if (processingOrder[i]->isMaster) {
                        gateOn = processingOrder[i]->getCable(3);
                        gateOn.val[index][0] = 1;
                        gateOn.val[index][1] = 1;
                        processingOrder[i]->putCable(3, gateOn);

                        ModuleControl m = processingOrder[i]->getControl(1);
                        pitch = processingOrder[i]->getCable(2);
                        pitch.val[index][0] = (notesOn[k] - 60.0) / 12.0 + m.val[0];
                        pitch.val[index][1] = (notesOn[k] - 60.0) / 12.0 + m.val[1];
                        processingOrder[i]->putCable(2, pitch);
                    }
                    if (processingOrder[i]->needsReset) {
                        processingOrder[i]->reset(index);
                    }
                }
            }
            if (notesOff.size() > 0 && processingOrder[i]->isMaster) {
                for (int k = 0; k < notesOff.size(); k++) {
                    gateOff = processingOrder[i]->getCable(3);
                    for (int l = (int)notes.size() - 1; l >= 0; l--) {
                        if (notes[l].noteNumber == notesOff[k]) {
                            gateOff.val[notes[l].voiceNumber][0] = 0;
                            gateOff.val[notes[l].voiceNumber][1] = 0;
                            notes.erase(notes.begin() + l);
                        }
                    }
                    processingOrder[i]->putCable(3, gateOff);
                }
            }
            if (processingOrder[i]->isMaster) {
                CableConnection c = processingOrder[i]->getCable(0);
                for (int voice = 0; voice < voiceLimit; voice++) {
                    c.val[voice][0] = buffer.getSample(0, s);
                    c.val[voice][1] = buffer.getSample(1, s);
                }
            }
            processingOrder[i]->run(voiceLimit);
            if (j >= plugs.size()) continue;
            while (plugs[j].sourceM == i) {
                CableConnection c = processingOrder[i]->getCable(plugs[j].sourceC);
                if (plugs[j].destM < processingOrder.size() && plugs[j].destM >= 0) {
                    processingOrder[plugs[j].destM]->putCable(plugs[j].destC, c);
                }
                j++;
                if (j >= plugs.size()) break;
            }
        }
        CableConnection c = processingOrder[0]->getCable(1);
        ModuleControl m = processingOrder[0]->getControl(0);
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            int channelSelector = channel % 2;
            double voicesSum = 0;
            for (int voice = 0; voice < voiceLimit; voice++) {
                voicesSum = voicesSum + m.val[channelSelector] * c.val[voice][channelSelector];
            }
            channelData[s] = (float)voicesSum;
        }
    }
}

int OOPSAudioProcessor::insertModule(int modDest, ModuleComponent* m) {
    if (modDest < 1 || modDest > processingOrder.size()) {
        processingOrder.push_back(m);
        return (int)processingOrder.size() - 1;
    }
    processingOrder.insert(processingOrder.begin() + modDest, m);
    for (int i = 0; i < plugs.size(); i++) {
        if (plugs[i].sourceM >= modDest) plugs[i].sourceM++;
        if (plugs[i].destM >= modDest) plugs[i].destM++;
    }
    return modDest;
}

int OOPSAudioProcessor::insertNewModule(int modDest, ModuleType modType) {
    ModuleComponent* m;
    switch (modType) {
    case MasterType:
        m = new Master{ sampleRateMemory };
        break;
    case OscillatorType:
        m = new Oscillator{ sampleRateMemory };
        break;
    case HarmonicOscillatorType:
        m = new HarmonicOscillator{ sampleRateMemory };
        break;
    case EnvelopeType:
        m = new Envelope{ sampleRateMemory };
        break;
    case RingModType:
        m = new RingMod{ sampleRateMemory };
        break;
    case VoltageUtilityType:
        m = new VoltageUtility{ sampleRateMemory };
        break;
    default:
        return -1;
    }
    return insertModule(modDest, m);
}

void OOPSAudioProcessor::removeModule(int modDest) {
    if (modDest < 1 || modDest >= processingOrder.size()) return;

    std::vector<int> toRemove;
    for (int i = 0; i < plugs.size(); i++) {
        if (plugs[i].sourceM == modDest || plugs[i].destM == modDest) toRemove.push_back(i);
        if (plugs[i].sourceM > modDest) plugs[i].sourceM--;
        if (plugs[i].destM > modDest) plugs[i].destM--;
    }
    for (int i = 0; i < toRemove.size(); i++) {
        plugs.erase(plugs.begin() + toRemove[toRemove.size() - 1 - i]);
    }
    delete processingOrder[modDest];
    processingOrder.erase(processingOrder.begin() + modDest);
}

int OOPSAudioProcessor::moveModule(int modDest, int modSource) {
    if (modSource < 1 || modSource >= processingOrder.size()) return -1;
    if (modSource == modDest) return modDest;
    int transferSource = modSource;
    modDest = insertModule(modDest, processingOrder[modSource]);
    if (modDest < modSource) transferSource++;
    for (int i = 0; i < plugs.size(); i++) {
        if (plugs[i].sourceM == transferSource) plugs[i].sourceM = modDest;
        if (plugs[i].destM == transferSource) plugs[i].destM = modDest;

        if (plugs[i].sourceM > transferSource) plugs[i].sourceM--;
        if (plugs[i].destM > transferSource) plugs[i].destM--;
    }
    processingOrder.erase(processingOrder.begin() + transferSource);
    return modDest;
}

//==============================================================================
bool OOPSAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OOPSAudioProcessor::createEditor()
{
    return new OOPSAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OOPSAudioProcessor();
}
