#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 * Scalpel Audio Processor Editor
 * 
 * This is the GUI for the Scalpel plugin, with emphasis on visual feedback.
 */
class ScalpelAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ScalpelAudioProcessorEditor(ScalpelAudioProcessor&);
    ~ScalpelAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ScalpelAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalpelAudioProcessorEditor)
};
