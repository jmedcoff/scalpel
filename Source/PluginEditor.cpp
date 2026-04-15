#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScalpelAudioProcessorEditor::ScalpelAudioProcessorEditor(ScalpelAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set initial size - this can be adjusted for your visualization needs
    setSize(800, 600);
}

ScalpelAudioProcessorEditor::~ScalpelAudioProcessorEditor()
{
}

//==============================================================================
void ScalpelAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Draw placeholder text
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("Scalpel", getLocalBounds(), juce::Justification::centred, 1);
    
    g.setFont(14.0f);
    auto bounds = getLocalBounds();
    bounds.removeFromTop(bounds.getHeight() / 2 + 20);
    g.drawFittedText("Audio Plugin", bounds, juce::Justification::centredTop, 1);
}

void ScalpelAudioProcessorEditor::resized()
{
    // Layout your visualization components here
    // This is called when the editor is resized
}
