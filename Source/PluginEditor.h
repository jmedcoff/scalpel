#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveformDisplay.h"

//==============================================================================
class ScalpelAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   private juce::Timer
{
public:
    ScalpelAudioProcessorEditor(ScalpelAudioProcessor&);
    ~ScalpelAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateWindowSizeControls();
    
    ScalpelAudioProcessor& audioProcessor;
    
    WaveformDisplay waveformDisplay;
    
    juce::Slider gainInSlider;
    juce::Slider gainOutSlider;
    juce::Slider windowSizeFreeSlider;
    
    juce::ToggleButton linkGainsButton;
    juce::ComboBox syncModeCombo;
    juce::ComboBox windowSizeSyncedCombo;
    juce::ComboBox channelSelectCombo;
    
    juce::Label gainInLabel;
    juce::Label gainOutLabel;
    juce::Label syncModeLabel;
    juce::Label windowSizeLabel;
    juce::Label channelSelectLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainInAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainOutAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> windowSizeFreeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> linkGainsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> syncModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> windowSizeSyncedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> channelSelectAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalpelAudioProcessorEditor)
};
