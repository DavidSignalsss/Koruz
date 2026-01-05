#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class KoruzAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   private juce::Timer  // ← Añade herencia de Timer
{
public:
    KoruzAudioProcessorEditor (KoruzAudioProcessor&);
    ~KoruzAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    KoruzAudioProcessor& audioProcessor;
    
    juce::Slider rateSlider;
    juce::Slider depthSlider;
    juce::Slider mixSlider;
    
    juce::Label rateLabel;
    juce::Label depthLabel;
    juce::Label mixLabel;
    juce::Label titleLabel;

    // Variables para la animación de la cuerda
    float stringPhase = 0.0f;
    float stringAmplitude = 0.0f;
    juce::Time lastUpdateTime;
    const float stringAnimationSpeed = 0.3f;

    // Timer para animación - QUITA EL 'override'
    void timerCallback() override;  // ← Solo esto, sin override aquí
    void updateStringAnimation();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KoruzAudioProcessorEditor)
};