#pragma once

#include <JuceHeader.h>

class KoruzAudioProcessor  : public juce::AudioProcessor
{
public:
    KoruzAudioProcessor();
    ~KoruzAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Parameters
    juce::AudioParameterFloat* getRateParam() { return rateParam.get(); }
    juce::AudioParameterFloat* getDepthParam() { return depthParam.get(); }
    juce::AudioParameterFloat* getMixParam() { return mixParam.get(); }

private:
    // Chorus parameters
    std::unique_ptr<juce::AudioParameterFloat> rateParam;
    std::unique_ptr<juce::AudioParameterFloat> depthParam;
    std::unique_ptr<juce::AudioParameterFloat> mixParam;

    // Chorus variables
    double currentSampleRate = 44100.0;
    float phase = 0.0f;
    float lfoSmoothed = 0.5f;  // ← AÑADE ESTA LÍNEA
    
    // Delay buffer - usando std::vector para mayor seguridad
    std::vector<std::vector<float>> delayBuffers;
    std::vector<int> writePositions;
    bool isPrepared = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KoruzAudioProcessor)
};