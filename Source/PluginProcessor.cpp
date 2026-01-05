#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include <cmath>

KoruzAudioProcessor::KoruzAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Rate: 0.1Hz to 2.0Hz
    rateParam = std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("rate", 1), 
        "Rate", 
        juce::NormalisableRange<float>(0.1f, 2.0f, 0.01f), 
        0.8f
    );
    addParameter(rateParam.get());

    // Depth: 0% to 100%
    depthParam = std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("depth", 1), 
        "Depth", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 
        0.4f
    );
    addParameter(depthParam.get());

    // Mix: 0% to 100%
    mixParam = std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mix", 1), 
        "Mix", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 
        0.5f
    );
    addParameter(mixParam.get());
}

KoruzAudioProcessor::~KoruzAudioProcessor()
{
    releaseResources();
}

void KoruzAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    phase = 0.0f;
    lfoSmoothed = 0.5f;
    
    releaseResources();
    
    int numChannels = getTotalNumInputChannels();
    if (numChannels == 0) numChannels = 2;
    
    // Buffer de delay optimizado (35ms)
    int delayBufferSize = static_cast<int>(sampleRate * 0.035);
    delayBufferSize = std::max(delayBufferSize, 1024);
    
    delayBuffers.clear();
    writePositions.clear();
    
    for (int i = 0; i < numChannels; ++i)
    {
        delayBuffers.emplace_back(delayBufferSize, 0.0f);
        writePositions.push_back(0);
    }
    
    isPrepared = true;
}

void KoruzAudioProcessor::releaseResources()
{
    delayBuffers.clear();
    writePositions.clear();
    isPrepared = false;
}

void KoruzAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    if (!isPrepared) return;
        
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (totalNumInputChannels == 0 || delayBuffers.empty()) return;

    float rate = rateParam->get();
    float depth = depthParam->get();
    float mix = mixParam->get();

    // ALGORITMO PROFESIONAL SIN NOISE
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        if (channel >= delayBuffers.size()) continue;
        
        auto* channelData = buffer.getWritePointer(channel);
        auto& delayData = delayBuffers[channel];
        int& writePos = writePositions[channel];
        const int delayBufferSize = static_cast<int>(delayData.size());
        
        if (delayBufferSize < 4) continue;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const float inputSample = channelData[sample];
            
            // LFO ULTRA SUAVE CON ANTI-ALIASING
            phase += static_cast<float>(rate / currentSampleRate);
            if (phase >= 1.0f) phase -= 1.0f;
            
            // LFO con forma de seno suavizado
            float lfoValue = 0.5f + 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * phase);
            
            // Suavizado exponencial del LFO
            lfoSmoothed = 0.9995f * lfoSmoothed + 0.0005f * lfoValue;
            
            // CURVA DE DEPTH OPTIMIZADA
            float depthCurve = depth * depth;
            float modulatedDepth = depthCurve * lfoSmoothed;
            
            // RANGO MUSICAL OPTIMIZADO: 8-22ms
            const float baseDelayMs = 15.0f;
            const float depthRangeMs = 7.0f;
            float delayTimeMs = baseDelayMs + depthRangeMs * modulatedDepth;
            
            float delayTimeSamples = delayTimeMs * static_cast<float>(currentSampleRate) / 1000.0f;
            
            // LIMITACIÓN INTELIGENTE
            delayTimeSamples = juce::jlimit(10.0f, static_cast<float>(delayBufferSize - 10), delayTimeSamples);
            
            // ESCRITURA EN BUFFER
            delayData[writePos] = inputSample * 0.999f;
            
            // CÁLCULO DE POSICIÓN DE LECTURA
            float readPosition = writePos - delayTimeSamples;
            while (readPosition < 0) readPosition += delayBufferSize;
            while (readPosition >= delayBufferSize) readPosition -= delayBufferSize;
            
            // INTERPOLACIÓN CÚBICA DE 4 PUNTOS
            int idx0 = static_cast<int>(readPosition) - 1;
            if (idx0 < 0) idx0 += delayBufferSize;
            int idx1 = static_cast<int>(readPosition);
            int idx2 = (idx1 + 1) % delayBufferSize;
            int idx3 = (idx2 + 1) % delayBufferSize;
            
            float frac = readPosition - idx1;
            
            // Coeficientes Catmull-Rom
            float y0 = delayData[idx0];
            float y1 = delayData[idx1];
            float y2 = delayData[idx2];
            float y3 = delayData[idx3];
            
            float c0 = y1;
            float c1 = 0.5f * (y2 - y0);
            float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
            float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
            
            float delayedSample = c0 + c1 * frac + c2 * frac * frac + c3 * frac * frac * frac;
            
            // SUAVIZADO ADAPTATIVO
            float adaptiveSmoothing = 1.0f;
            if (depth > 0.3f) {
                float depthFactor = (depth - 0.3f) / 0.7f;
                adaptiveSmoothing = 1.0f - depthFactor * 0.2f;
            }
            delayedSample *= adaptiveSmoothing;
            
            // MEZCLA DRY/WET OPTIMIZADA
            float wetGain = mix * 0.95f;
            float dryGain = 1.0f - mix;
            
            float outputSample = (inputSample * dryGain) + (delayedSample * wetGain);
            
            // PROTECCIÓN CONTRA CLIPPING
            const float threshold = 0.99f;
            if (outputSample > threshold) {
                outputSample = threshold + (outputSample - threshold) * 0.3f;
            } else if (outputSample < -threshold) {
                outputSample = -threshold + (outputSample + threshold) * 0.3f;
            }
            
            channelData[sample] = outputSample;
            
            // ACTUALIZACIÓN DE POSICIÓN
            writePos = (writePos + 1) % delayBufferSize;
        }
    }
}

// Resto de funciones JUCE
const juce::String KoruzAudioProcessor::getName() const { return JucePlugin_Name; }
bool KoruzAudioProcessor::acceptsMidi() const { return false; }
bool KoruzAudioProcessor::producesMidi() const { return false; }
bool KoruzAudioProcessor::isMidiEffect() const { return false; }
double KoruzAudioProcessor::getTailLengthSeconds() const { return 0.035; }
int KoruzAudioProcessor::getNumPrograms() { return 1; }
int KoruzAudioProcessor::getCurrentProgram() { return 0; }
void KoruzAudioProcessor::setCurrentProgram (int index) {}
const juce::String KoruzAudioProcessor::getProgramName (int index) { return {}; }
void KoruzAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KoruzAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    
    return true;
}
#endif

juce::AudioProcessorEditor* KoruzAudioProcessor::createEditor()
{
    return new KoruzAudioProcessorEditor (*this);
}

bool KoruzAudioProcessor::hasEditor() const
{
    return true;
}

void KoruzAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void KoruzAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KoruzAudioProcessor();
}