#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * WaveformDisplay Component
 * 
 * Displays the input waveform with distinct coloring for clipped samples.
 */
class WaveformDisplay : public juce::Component,
                       private juce::Timer
{
public:
    WaveformDisplay(ScalpelAudioProcessor& processor);
    ~WaveformDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Set which channel to display (0=Left, 1=Right)
    void setChannel(int channel);
    
    // Set window size in samples
    void setWindowSizeSamples(int samples);

private:
    void timerCallback() override;
    
    ScalpelAudioProcessor& audioProcessor;
    
    int displayChannel = 0;
    int windowSizeSamples = 48000;
    
    std::vector<VisualizationSample> displayData;
    juce::CriticalSection dataLock;
    
    juce::Colour waveformColour = juce::Colours::lightgreen;
    juce::Colour clippedColour = juce::Colours::red;
    juce::Colour backgroundColour = juce::Colour(0xff1a1a1a);
    juce::Colour gridColour = juce::Colour(0xff2a2a2a);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
