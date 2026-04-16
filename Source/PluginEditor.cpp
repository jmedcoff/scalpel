#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScalpelAudioProcessorEditor::ScalpelAudioProcessorEditor(ScalpelAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), waveformDisplay(p)
{
    addAndMakeVisible(waveformDisplay);
    
    gainInSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainInSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(gainInSlider);
    gainInLabel.setText("Gain In", juce::dontSendNotification);
    gainInLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainInLabel);
    gainInAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), ScalpelAudioProcessor::GAIN_IN_ID, gainInSlider);
    
    gainOutSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainOutSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(gainOutSlider);
    gainOutLabel.setText("Gain Out", juce::dontSendNotification);
    gainOutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainOutLabel);
    gainOutAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), ScalpelAudioProcessor::GAIN_OUT_ID, gainOutSlider);
    
    linkGainsButton.setButtonText("Link Gains");
    addAndMakeVisible(linkGainsButton);
    linkGainsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), ScalpelAudioProcessor::LINK_GAINS_ID, linkGainsButton);
    
    syncModeCombo.addItem("Free", 1);
    syncModeCombo.addItem("Synced", 2);
    addAndMakeVisible(syncModeCombo);
    syncModeLabel.setText("Sync Mode", juce::dontSendNotification);
    syncModeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(syncModeLabel);
    syncModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), ScalpelAudioProcessor::SYNC_MODE_ID, syncModeCombo);
    
    windowSizeFreeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    windowSizeFreeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addAndMakeVisible(windowSizeFreeSlider);
    windowSizeFreeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), ScalpelAudioProcessor::WINDOW_SIZE_FREE_ID, windowSizeFreeSlider);
    
    windowSizeSyncedCombo.addItem("1/4 measure", 1);
    windowSizeSyncedCombo.addItem("1/2 measure", 2);
    windowSizeSyncedCombo.addItem("1 measure", 3);
    windowSizeSyncedCombo.addItem("2 measures", 4);
    windowSizeSyncedCombo.addItem("4 measures", 5);
    windowSizeSyncedCombo.addItem("8 measures", 6);
    addAndMakeVisible(windowSizeSyncedCombo);
    windowSizeSyncedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), ScalpelAudioProcessor::WINDOW_SIZE_SYNCED_ID, windowSizeSyncedCombo);
    
    windowSizeLabel.setText("Window Size", juce::dontSendNotification);
    windowSizeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(windowSizeLabel);
    
    channelSelectCombo.addItem("Left", 1);
    channelSelectCombo.addItem("Right", 2);
    addAndMakeVisible(channelSelectCombo);
    channelSelectLabel.setText("Channel", juce::dontSendNotification);
    channelSelectLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(channelSelectLabel);
    channelSelectAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), ScalpelAudioProcessor::CHANNEL_SELECT_ID, channelSelectCombo);
    
    setSize(900, 600);
    waveformDisplay.setVisible(true);
    waveformDisplay.toFront(false);
}

ScalpelAudioProcessorEditor::~ScalpelAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void ScalpelAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff202020));
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("SCALPEL", 20, 10, 200, 40, juce::Justification::centredLeft);
    
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colours::grey);
    g.drawText("Jorts DSP Labs", 20, 45, 400, 20, juce::Justification::centredLeft);
}

void ScalpelAudioProcessorEditor::resized()
{
    // Start timer on first resize (20fps to reduce CPU load)
    if (!isTimerRunning())
        startTimerHz(20);
    
    auto bounds = getLocalBounds();
    
    auto titleArea = bounds.removeFromTop(70);
    auto controlArea = bounds.removeFromBottom(150);
    
    waveformDisplay.setBounds(bounds.reduced(10));
    
    auto leftControls = controlArea.removeFromLeft(controlArea.getWidth() / 2);
    auto rightControls = controlArea;
    
    // Gain controls
    auto gainArea = leftControls.reduced(20, 10);
    auto gainInArea = gainArea.removeFromLeft(120);
    auto linkArea = gainArea.removeFromLeft(100);
    auto gainOutArea = gainArea.removeFromLeft(120);
    
    gainInLabel.setBounds(gainInArea.removeFromTop(20));
    gainInSlider.setBounds(gainInArea);
    
    linkGainsButton.setBounds(linkArea.removeFromTop(40).withY(gainInArea.getY() + 30));
    
    gainOutLabel.setBounds(gainOutArea.removeFromTop(20));
    gainOutSlider.setBounds(gainOutArea);
    
    // Window and channel controls
    auto settingsArea = rightControls.reduced(20, 10);
    
    auto syncRow = settingsArea.removeFromTop(30);
    syncModeLabel.setBounds(syncRow.removeFromLeft(100));
    syncModeCombo.setBounds(syncRow.removeFromLeft(150));
    
    settingsArea.removeFromTop(10);
    
    auto windowRow = settingsArea.removeFromTop(30);
    windowSizeLabel.setBounds(windowRow.removeFromLeft(100));
    windowSizeFreeSlider.setBounds(windowRow);
    windowSizeSyncedCombo.setBounds(windowRow);
    
    settingsArea.removeFromTop(10);
    
    auto channelRow = settingsArea.removeFromTop(30);
    channelSelectLabel.setBounds(channelRow.removeFromLeft(100));
    channelSelectCombo.setBounds(channelRow.removeFromLeft(150));
}

//==============================================================================
void ScalpelAudioProcessorEditor::timerCallback()
{
    auto* linkGainsParam = audioProcessor.getValueTreeState().getRawParameterValue(ScalpelAudioProcessor::LINK_GAINS_ID);
    auto* gainInParam = audioProcessor.getValueTreeState().getRawParameterValue(ScalpelAudioProcessor::GAIN_IN_ID);
    auto* channelParam = audioProcessor.getValueTreeState().getRawParameterValue(ScalpelAudioProcessor::CHANNEL_SELECT_ID);
    
    if (!linkGainsParam || !gainInParam || !channelParam)
        return;
    
    bool linkGains = linkGainsParam->load() > 0.5f;
    gainOutSlider.setEnabled(!linkGains);
    
    if (linkGains)
    {
        float gainIn = gainInParam->load();
        gainOutSlider.setValue(-gainIn, juce::dontSendNotification);
    }
    
    updateWindowSizeControls();
    
    int channel = static_cast<int>(channelParam->load());
    waveformDisplay.setChannel(channel);
}

void ScalpelAudioProcessorEditor::updateWindowSizeControls()
{
    auto* syncModeParam = audioProcessor.getValueTreeState().getRawParameterValue(ScalpelAudioProcessor::SYNC_MODE_ID);
    if (!syncModeParam)
        return;
    
    bool isSynced = syncModeParam->load() > 0.5f;
    
    windowSizeFreeSlider.setVisible(!isSynced);
    windowSizeSyncedCombo.setVisible(isSynced);
    
    int windowSizeSamples = audioProcessor.calculateWindowSizeSamples();
    waveformDisplay.setWindowSizeSamples(windowSizeSamples);
}
