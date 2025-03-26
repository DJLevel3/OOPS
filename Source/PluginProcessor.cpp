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
    processingOrder.push_back(new HarmonicOscillator(96000));
    processingOrder.push_back(new Oscillator(96000));
    processingOrder.push_back(new Envelope(96000));
    processingOrder.push_back(new HarmonicOscillator(96000));
    processingOrder.push_back(new RingMod(96000));

    for (int i = 0; i < processingOrder.size(); i++) {
        processingOrder[i]->reset();
    }

    CableMap c1 = { 0, 0, 1, 7 };
    CableMap c2 = { 1, 0, 2, 1 };
    CableMap c3 = { 2, 0, 4, 1 };
    CableMap c4 = { 3, 0, 4, 2 };
    CableMap c5 = { 4, 0, -1, 1 };
    plugs.push_back(c1);
    plugs.push_back(c2);
    plugs.push_back(c3);
    plugs.push_back(c4);
    plugs.push_back(c5);
    std::sort(plugs.begin(), plugs.end(), compareCableMapsSource);
}

OOPSAudioProcessor::~OOPSAudioProcessor()
{
    for (int i = processingOrder.size(); i > 0; i--) {
        delete processingOrder[i - 1];
        continue;
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
    for (int s = 0; s < buffer.getNumSamples(); s++) {
        if (noteOnTimes.size() > 0) {
            while (s == noteOnTimes[stampOn]) {
                notesOn.push_back( noteOnNumbers[stampOn]);
                latestVoice = (latestVoice + 1) % voiceLimit;

                if (notes.size() == voiceLimit) {
                    panic.push_back(notes[0].voiceNumber);
                    notes.erase(notes.begin());
                }
                Note n = { noteOnNumbers[stampOn], latestVoice};
                notes.push_back(n);;

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
                processingOrder[i]->reset(k);
            }
            if (notesOn.size() > 0) {
                for (int k = 0; k < notesOn.size(); k++) {
                    int index = (((latestVoice + k - notesOn.size() + 1) % voiceLimit) + voiceLimit) % voiceLimit;
                    if (processingOrder[i]->needsGate) {
                        gateOn = processingOrder[i]->getCable(3);
                        gateOn.val[index][0] = 1;
                        gateOn.val[index][1] = 1;
                        processingOrder[i]->putCable(3, gateOn);
                    }
                    if (processingOrder[i]->needsPitch) {
                        pitch = processingOrder[i]->getCable(2);
                        pitch.val[index][0] = (notesOn[k] - 60.0) / 12.0;
                        pitch.val[index][1] = (notesOn[k] - 60.0) / 12.0;
                        processingOrder[i]->putCable(2, pitch);
                    }
                    if (processingOrder[i]->needsReset) {
                        processingOrder[i]->reset(index);
                    }
                }
            }
            if (notesOff.size() > 0 && processingOrder[i]->needsGate) {
                for (int k = 0; k < notesOff.size(); k++) {
                    gateOff = processingOrder[i]->getCable(3);
                    for (int l = 0; l < notes.size(); l++) {
                        if (notes[l].noteNumber == notesOff[k]) {
                            gateOff.val[notes[l].voiceNumber][0] = 0;
                            gateOff.val[notes[l].voiceNumber][1] = 0;
                        }
                    }
                    processingOrder[i]->putCable(3, gateOff);
                }
            }
            processingOrder[i]->run();
            if (j >= plugs.size()) continue;
            while (plugs[j].sourceM == i) {
                CableConnection c = processingOrder[i]->getCable(plugs[j].sourceC);
                if (plugs[j].destM == -1) {
                    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
                    {
                        float* channelData = buffer.getWritePointer(channel);
                        int channelSelector = channel % 2;
                        double voicesSum = 0;
                        for (int voice = 0; voice < voiceLimit; voice++) {
                            voicesSum = voicesSum + c.val[voice][channelSelector];
                        }
                        channelData[s] = voicesSum;
                    }
                }
                else if (plugs[j].destM < processingOrder.size() && plugs[j].destM >= 0) {
                    processingOrder[plugs[j].destM]->putCable(plugs[j].destC, c);
                }
                j++;
                if (j >= plugs.size()) break;
            }
        }
    }
}

int OOPSAudioProcessor::insertModule(int modDest, ModuleComponent* m) {
    if (modDest < 0 || modDest > processingOrder.size()) {
        processingOrder.push_back(m);
        return processingOrder.size() - 1;
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
    default:
        return -1;
    }
    return insertModule(modDest, m);
}

void OOPSAudioProcessor::removeModule(int modDest) {
    if (modDest < 0 || modDest >= processingOrder.size()) return;

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
    if (modSource < 0 || modSource >= processingOrder.size() || modSource == modDest) return -1;
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
void OOPSAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void OOPSAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OOPSAudioProcessor();
}
