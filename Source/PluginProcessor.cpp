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
    ),
#else
    :
#endif
    apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    for (int ch = 0; ch < 2; ++ch)
    {
        visualizationBuffers[ch].resize(MAX_BUFFER_SIZE);
        writePositions[ch] = 0;
    }
}

ScalpelAudioProcessor::~ScalpelAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout ScalpelAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Gain In: -24 to +24 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(GAIN_IN_ID, 1),
        "Gain In",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " dB"; }
    ));
    
    // Gain Out: -24 to +24 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(GAIN_OUT_ID, 1),
        "Gain Out",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + " dB"; }
    ));
    
    // Link Gains
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID(LINK_GAINS_ID, 1),
        "Link Gains",
        false
    ));
    
    // Sync Mode: 0 = Free, 1 = Synced
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(SYNC_MODE_ID, 1),
        "Sync Mode",
        juce::StringArray{"Free", "Synced"},
        0
    ));
    
    // Window Size (Free mode): 100 to 10000 ms
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(WINDOW_SIZE_FREE_ID, 1),
        "Window Size (ms)",
        juce::NormalisableRange<float>(100.0f, 10000.0f, 1.0f, 0.3f), // Skewed toward lower values
        1000.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 0) + " ms"; }
    ));
    
    // Window Size (Synced mode): 0=1/4, 1=1/2, 2=1, 3=2, 4=4, 5=8 measures
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(WINDOW_SIZE_SYNCED_ID, 1),
        "Window Size (measures)",
        juce::StringArray{"1/4", "1/2", "1", "2", "4", "8"},
        2 // Default to 1 measure
    ));
    
    // Channel Select: 0=Left, 1=Right
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(CHANNEL_SELECT_ID, 1),
        "Channel",
        juce::StringArray{"Left", "Right"},
        0
    ));
    
    return { params.begin(), params.end() };
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
    currentSampleRate = sampleRate;
    
    for (int ch = 0; ch < 2; ++ch)
    {
        const juce::SpinLock::ScopedLockType lock(bufferLocks[ch]);
        std::fill(visualizationBuffers[ch].begin(), visualizationBuffers[ch].end(), VisualizationSample());
        writePositions[ch] = 0;
    }
}

void ScalpelAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ScalpelAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float gainInDB = apvts.getRawParameterValue(GAIN_IN_ID)->load();
    float gainOutDB = apvts.getRawParameterValue(GAIN_OUT_ID)->load();
    bool linkGains = apvts.getRawParameterValue(LINK_GAINS_ID)->load() > 0.5f;
    
    if (linkGains)
        gainOutDB = -gainInDB;
    
    float gainIn = juce::Decibels::decibelsToGain(gainInDB);
    float gainOut = juce::Decibels::decibelsToGain(gainOutDB);
    
    int numSamples = buffer.getNumSamples();
    
    for (int channel = 0; channel < juce::jmin(totalNumInputChannels, 2); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputSample = channelData[sample] * gainIn;
            bool isClipped = std::abs(inputSample) > 1.0f;
            
            float outputSample = juce::jlimit(-1.0f, 1.0f, inputSample);
            outputSample *= gainOut;
            channelData[sample] = outputSample;
            
            // Write to ring buffer (SpinLock keeps audio thread lock-free)
            {
                const juce::SpinLock::ScopedLockType lock(bufferLocks[channel]);
                int writePos = writePositions[channel];
                visualizationBuffers[channel][writePos] = VisualizationSample(inputSample, isClipped);
                writePositions[channel] = (writePos + 1) % MAX_BUFFER_SIZE;
            }
        }
    }
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
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ScalpelAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
void ScalpelAudioProcessor::getVisualizationData(int channel, std::vector<VisualizationSample>& outData, int numSamples)
{
    if (channel < 0 || channel >= 2)
        return;
    
    if (visualizationBuffers[channel].empty())
    {
        outData.clear();
        return;
    }
    
    numSamples = juce::jmin(numSamples, MAX_BUFFER_SIZE);
    outData.resize(numSamples);
    
    {
        const juce::SpinLock::ScopedLockType lock(bufferLocks[channel]);
        
        int writePos = writePositions[channel];
        int readStart = (writePos - numSamples + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
        
        for (int i = 0; i < numSamples; ++i)
        {
            int readPos = (readStart + i) % MAX_BUFFER_SIZE;
            outData[i] = visualizationBuffers[channel][readPos];
        }
    }
}

int ScalpelAudioProcessor::calculateWindowSizeSamples() const
{
    auto* syncModeParam = apvts.getRawParameterValue(SYNC_MODE_ID);
    if (!syncModeParam)
        return 48000;
    
    bool isSynced = syncModeParam->load() > 0.5f;
    
    if (!isSynced)
    {
        auto* windowMsParam = apvts.getRawParameterValue(WINDOW_SIZE_FREE_ID);
        if (!windowMsParam)
            return 48000;
            
        float windowMs = windowMsParam->load();
        return static_cast<int>(windowMs * 0.001f * currentSampleRate);
    }
    else
    {
        auto* windowSizeIndexParam = apvts.getRawParameterValue(WINDOW_SIZE_SYNCED_ID);
        if (!windowSizeIndexParam)
            return 48000;
            
        int windowSizeIndex = static_cast<int>(windowSizeIndexParam->load());
        
        double bpm = 120.0;
        int timeSigNumerator = 4;
        
        if (auto* playHead = getPlayHead())
        {
            if (auto positionInfo = playHead->getPosition())
            {
                if (positionInfo->getBpm().hasValue())
                    bpm = *positionInfo->getBpm();
                    
                if (positionInfo->getTimeSignature().hasValue())
                    timeSigNumerator = positionInfo->getTimeSignature()->numerator;
            }
        }
        
        double beatsPerMeasure = static_cast<double>(timeSigNumerator);
        
        const double measureMultipliers[] = { 0.25, 0.5, 1.0, 2.0, 4.0, 8.0 };
        double measures = measureMultipliers[juce::jlimit(0, 5, windowSizeIndex)];
        
        double totalBeats = measures * beatsPerMeasure;
        double seconds = (totalBeats * 60.0) / bpm;
        
        return static_cast<int>(seconds * currentSampleRate);
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ScalpelAudioProcessor();
}
