#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(ScalpelAudioProcessor& processor)
    : audioProcessor(processor)
{
    startTimerHz(20);
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();
}

//==============================================================================
void WaveformDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    g.fillAll(backgroundColour);
    
    g.setColour(gridColour);
    
    float centerY = bounds.getCentreY();
    g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, 1.0f);
    
    float topLine = bounds.getY() + bounds.getHeight() * 0.1f;
    float bottomLine = bounds.getBottom() - bounds.getHeight() * 0.1f;
    g.setOpacity(0.5f);
    g.drawLine(bounds.getX(), topLine, bounds.getRight(), topLine, 1.0f);
    g.drawLine(bounds.getX(), bottomLine, bounds.getRight(), bottomLine, 1.0f);
    g.setOpacity(1.0f);
    
    g.setColour(juce::Colours::darkgrey);
    g.drawRect(bounds, 1.0f);
    
    std::vector<VisualizationSample> dataCopy;
    {
        const juce::ScopedLock lock(dataLock);
        dataCopy = displayData;
    }
    
    int numSamples = static_cast<int>(dataCopy.size());
    
    if (numSamples > 0)
    {
        float width = bounds.getWidth();
        float height = bounds.getHeight();
        
        int numPixels = static_cast<int>(width);
        int stride = juce::jmax(1, numSamples / numPixels);
        
        float waveformCenterY = bounds.getCentreY();
        float scaleX = width / static_cast<float>((numSamples + stride - 1) / stride);
        float scaleY = height * 0.4f; // Scale to 80% of height (10% padding)
        
        juce::Path waveformPath;
        
        float x0 = bounds.getX();
        float y0 = waveformCenterY - dataCopy[0].inputValue * scaleY;
        y0 = juce::jlimit(bounds.getY(), bounds.getBottom(), y0);
        waveformPath.startNewSubPath(x0, y0);
        
        for (int i = stride; i < numSamples; i += stride)
        {
            float x = bounds.getX() + (i / stride) * scaleX;
            float y = waveformCenterY - dataCopy[i].inputValue * scaleY;
            y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);
            
            waveformPath.lineTo(x, y);
        }
        
        g.setColour(juce::Colours::white);
        g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
        
        for (int i = 0; i < numSamples; i += stride)
        {
            if (dataCopy[i].isClipped)
            {
                float x = bounds.getX() + (i / stride) * scaleX;
                float y = waveformCenterY - dataCopy[i].inputValue * scaleY;
                y = juce::jlimit(bounds.getY(), bounds.getBottom(), y);
                
                g.setColour(clippedColour);
                g.fillEllipse(x - 2.0f, y - 2.0f, 4.0f, 4.0f);
            }
        }
    }
}

void WaveformDisplay::resized()
{
}

//==============================================================================
void WaveformDisplay::timerCallback()
{
    int clampedWindowSize = juce::jmin(windowSizeSamples, 480000); // Max 10s at 48kHz
    
    std::vector<VisualizationSample> newData;
    audioProcessor.getVisualizationData(displayChannel, newData, clampedWindowSize);
    
    {
        const juce::ScopedLock lock(dataLock);
        displayData = std::move(newData);
    }
    
    repaint();
}

void WaveformDisplay::setChannel(int channel)
{
    displayChannel = juce::jlimit(0, 1, channel);
}

void WaveformDisplay::setWindowSizeSamples(int samples)
{
    windowSizeSamples = juce::jlimit(1000, 480000, samples);
}
