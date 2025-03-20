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
    ModuleControl phaseInit = { {0, 0.25}, "Phase", true, true };
    osc.putControl(2, phaseInit);

    osc.reset();
    fmOsc.reset();
    env.reset();
}

OOPSAudioProcessor::~OOPSAudioProcessor()
{
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
    osc.setSampleRate(sampleRate);
    fmOsc.setSampleRate(sampleRate);
    env.setSampleRate(sampleRate);
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
    std::vector<int> noteNumbers;
    std::vector<int> noteOffTimes;
    for (auto it : midiMessages)
    {
        auto msg = it.getMessage();
        if (msg.isNoteOn()) {
            noteOnTimes.push_back((int)msg.getTimeStamp());
            noteNumbers.push_back(msg.getNoteNumber());
        }
        if (msg.isNoteOff()) {
            noteOffTimes.push_back((int)msg.getTimeStamp());
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
    for (int s = 0; s < buffer.getNumSamples(); s++) {
        if (noteOnTimes.size() > 0) {
            if (s == noteOnTimes[stampOn]) {
                osc.reset();
                fmOsc.reset();

                double n = (noteNumbers[stampOn] - 60) / 12.0;
                CableConnection pitch = { {n, n}, "Pitch", true, true, true };
                osc.putCable(2, pitch);
                CableConnection p2 = { {n, n}, "Pitch", true, true, true };
                fmOsc.putCable(2, p2);

                CableConnection gateOn = { {1, 1}, "Trigger", true, true, true };
                env.putCable(3, gateOn);
                stampOn = (stampOn + 1) % noteOnTimes.size();
            }
        }
        if (noteOffTimes.size() > 0) {
            if (s == noteOffTimes[stampOff]) {
                CableConnection gateOff = { {0, 0}, "Trigger", true, true, true };
                env.putCable(3, gateOff);
                stampOff = (stampOff + 1) % noteOffTimes.size();
            }
        }

        fmOsc.run();
        CableConnection c = fmOsc.getCable(0);
        c.name = "FM";
        c.input = true;
        osc.putCable(7, c);
        osc.run();
        c = osc.getCable(0);
        c.name = "Input";
        c.input = true;
        env.putCable(1, c);
        env.run();

        c = env.getCable(0);
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            channelData[s] = (channel % 2 == 0) ? (c.val[0]) : (c.val[1]);
        }
    }
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
