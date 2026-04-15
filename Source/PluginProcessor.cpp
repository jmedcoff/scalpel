#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScalpelAudioProcessor::ScalpelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
}

ScalpelAudioProcessor::~ScalpelAudioProcessor()
{
}

//==============================================================================
const juce::String ScalpelAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ScalpelAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ScalpelAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ScalpelAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ScalpelAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ScalpelAudioProcessor::getNumPrograms()
{
    return 1;
}

int ScalpelAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ScalpelAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String ScalpelAudioProcessor::getProgramName(int index)
{
    return {};
}

void ScalpelAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void ScalpelAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare any DSP components here
}

void ScalpelAudioProcessor::releaseResources()
{
    // Release any resources
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ScalpelAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // Support mono or stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output layouts must match
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void ScalpelAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't have input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Main processing loop - currently just passes audio through
    // This is where your DSP code will go
}

//==============================================================================
bool ScalpelAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ScalpelAudioProcessor::createEditor()
{
    return new ScalpelAudioProcessorEditor(*this);
}

//==============================================================================
void ScalpelAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state here
}

void ScalpelAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state here
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ScalpelAudioProcessor();
}
