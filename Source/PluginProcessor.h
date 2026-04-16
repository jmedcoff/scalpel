#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Holds a visualization sample: input value before clipping and clipping flag
 */
struct VisualizationSample
{
    float inputValue;
    bool isClipped;    // True if |inputValue| > 1.0
    
    VisualizationSample() : inputValue(0.0f), isClipped(false) {}
    VisualizationSample(float val, bool clipped) : inputValue(val), isClipped(clipped) {}
};

//==============================================================================
class ScalpelAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    ScalpelAudioProcessor();
    ~ScalpelAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    //==============================================================================
    juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts; }
    
    // Thread-safe access to visualization ring buffers
    void getVisualizationData(int channel, std::vector<VisualizationSample>& outData, int numSamples);
    
    // Calculate window size in samples (depends on sync mode, tempo, and user parameters)
    int calculateWindowSizeSamples() const;
    
    // Parameter IDs
    static constexpr const char* GAIN_IN_ID = "gainIn";
    static constexpr const char* GAIN_OUT_ID = "gainOut";
    static constexpr const char* LINK_GAINS_ID = "linkGains";
    static constexpr const char* SYNC_MODE_ID = "syncMode";
    static constexpr const char* WINDOW_SIZE_FREE_ID = "windowSizeFree";
    static constexpr const char* WINDOW_SIZE_SYNCED_ID = "windowSizeSynced";
    static constexpr const char* CHANNEL_SELECT_ID = "channelSelect";

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // Ring buffers for visualization (L/R channels)
    static constexpr int MAX_BUFFER_SIZE = 768000; // ~16 seconds at 48kHz
    std::array<std::vector<VisualizationSample>, 2> visualizationBuffers;
    std::array<int, 2> writePositions;
    std::array<juce::SpinLock, 2> bufferLocks;
    
    double currentSampleRate = 44100.0;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalpelAudioProcessor)
};
