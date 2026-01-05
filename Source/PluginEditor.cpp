#include "PluginProcessor.h"
#include "PluginEditor.h"

KoruzAudioProcessorEditor::KoruzAudioProcessorEditor (KoruzAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Rate Slider - COLOR DORADO
    rateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    rateSlider.setRange(0.1, 2.0, 0.01);
    rateSlider.setValue(0.8);
    rateSlider.setTextValueSuffix(" Hz");
    rateSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffffd700));
    rateSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff555555));
    rateSlider.setColour(juce::Slider::thumbColourId, juce::Colours::gold);
    rateSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    rateSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xaa333333));
    addAndMakeVisible(rateSlider);

    rateLabel.setText("RATE", juce::dontSendNotification);
    rateLabel.setJustificationType(juce::Justification::centred);
    juce::Font rateFont;
    rateFont.setHeight(14.0f);
    rateFont.setBold(true);
    rateLabel.setFont(rateFont);
    rateLabel.setColour(juce::Label::textColourId, juce::Colours::gold);
    addAndMakeVisible(rateLabel);

    // Depth Slider - COLOR ROJO
    depthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    depthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    depthSlider.setRange(0.0, 100.0, 1.0);
    depthSlider.setValue(40.0);
    depthSlider.setTextValueSuffix(" %");
    depthSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff6b6b));
    depthSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff555555));
    depthSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff4444));
    depthSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    depthSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xaa333333));
    addAndMakeVisible(depthSlider);

    depthLabel.setText("DEPTH", juce::dontSendNotification);
    depthLabel.setJustificationType(juce::Justification::centred);
    juce::Font depthFont;
    depthFont.setHeight(14.0f);
    depthFont.setBold(true);
    depthLabel.setFont(depthFont);
    depthLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff6b6b));
    addAndMakeVisible(depthLabel);

    // Mix Slider - COLOR AZUL
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    mixSlider.setRange(0.0, 100.0, 1.0);
    mixSlider.setValue(50.0);
    mixSlider.setTextValueSuffix(" %");
    mixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4ecdc4));
    mixSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff555555));
    mixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff00ffff));
    mixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    mixSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xaa333333));
    addAndMakeVisible(mixSlider);

    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    juce::Font mixFont;
    mixFont.setHeight(14.0f);
    mixFont.setBold(true);
    mixLabel.setFont(mixFont);
    mixLabel.setColour(juce::Label::textColourId, juce::Colour(0xff4ecdc4));
    addAndMakeVisible(mixLabel);

    // Title Label - COLOR DORADO
    titleLabel.setText("KORUZ", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    juce::Font titleFont;
    titleFont.setHeight(32.0f);
    titleFont.setBold(true);
    titleLabel.setFont(titleFont);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::gold);
    addAndMakeVisible(titleLabel);

    // Configurar listeners
    rateSlider.onValueChange = [this] {
        audioProcessor.getRateParam()->setValueNotifyingHost(static_cast<float>(rateSlider.getValue()));
    };

    depthSlider.onValueChange = [this] {
        audioProcessor.getDepthParam()->setValueNotifyingHost(static_cast<float>(depthSlider.getValue()) / 100.0f);
    };

    mixSlider.onValueChange = [this] {
        audioProcessor.getMixParam()->setValueNotifyingHost(static_cast<float>(mixSlider.getValue()) / 100.0f);
    };

    setSize(500, 400); // Aumentado a 400px de alto para la animación
    setOpaque(true);
    
    // Iniciar timer para animación (60 FPS)
    startTimerHz(60);
}

KoruzAudioProcessorEditor::~KoruzAudioProcessorEditor()
{
    stopTimer();
}

void KoruzAudioProcessorEditor::timerCallback()
{
    updateStringAnimation();
    repaint(); // Redibujar para animación
}

void KoruzAudioProcessorEditor::updateStringAnimation()
{
    // Obtener parámetros actuales para la animación
    float rate = static_cast<float>(rateSlider.getValue());
    float depth = static_cast<float>(depthSlider.getValue()) / 100.0f;
    float mix = static_cast<float>(mixSlider.getValue()) / 100.0f;
    
    // Actualizar fase de la animación basado en el rate
    stringPhase += rate * stringAnimationSpeed * 0.01f;
    if (stringPhase > juce::MathConstants<float>::twoPi) {
        stringPhase -= juce::MathConstants<float>::twoPi;
    }
    
    // Calcular amplitud basado en depth y mix
    stringAmplitude = depth * mix * 0.8f;
}

void KoruzAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fondo con gradiente NEGRO a GRIS OSCURO
    juce::ColourGradient gradient(
        juce::Colour(0xff111111), 0, 0,
        juce::Colour(0xff222222), 0, static_cast<float>(getHeight()), false
    );
    g.setGradientFill(gradient);
    g.fillAll();

    // Panel central con efecto metálico
    juce::Rectangle<int> panel(40, 70, getWidth() - 80, getHeight() - 160);
    
    // Gradiente dorado para el panel
    juce::ColourGradient panelGradient(
        juce::Colour(0xff332200), static_cast<float>(panel.getX()), static_cast<float>(panel.getY()),
        juce::Colour(0xff553300), static_cast<float>(panel.getRight()), static_cast<float>(panel.getBottom()), false
    );
    g.setGradientFill(panelGradient);
    g.fillRoundedRectangle(panel.toFloat(), 12.0f);

    // Borde dorado
    g.setColour(juce::Colours::gold.withAlpha(0.6f));
    g.drawRoundedRectangle(panel.toFloat(), 12.0f, 2.0f);

    // Título KORUZ con efecto dorado
    juce::Font mainTitleFont;
    mainTitleFont.setHeight(36.0f);
    mainTitleFont.setBold(true);
    g.setFont(mainTitleFont);
    
    g.setColour(juce::Colours::gold);
    g.drawText("KORUZ", getLocalBounds().removeFromTop(65), juce::Justification::centred);

    // Subtítulo
    juce::Font subtitleFont;
    subtitleFont.setHeight(16.0f);
    subtitleFont.setItalic(true);
    g.setFont(subtitleFont);
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.drawText("Premium Chorus", getLocalBounds().removeFromTop(100).removeFromBottom(20), juce::Justification::centred);

    // ANIMACIÓN DE CUERDA - Centro debajo de los knobs
    int stringY = 280; // Posición Y de la cuerda
    int stringWidth = getWidth() - 120;
    int stringX = 60;
    
    // Dibujar línea base de la cuerda
    g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
    g.drawLine(stringX, stringY, stringX + stringWidth, stringY, 1.5f);
    
    // Dibujar cuerda animada
    g.setColour(juce::Colours::gold.withAlpha(0.8f));
    
    const int numPoints = 20;
    juce::Path stringPath;
    
    // Punto inicial
    stringPath.startNewSubPath(stringX, stringY);
    
    // Crear puntos de la cuerda con onda sinusoidal
    for (int i = 1; i < numPoints; ++i)
    {
        float x = stringX + (stringWidth * i / static_cast<float>(numPoints - 1));
        float wave = std::sin(stringPhase + (i * 0.3f));
        float y = stringY + wave * stringAmplitude * 15.0f;
        
        stringPath.lineTo(x, y);
    }
    
    // Dibujar la cuerda
    g.strokePath(stringPath, juce::PathStrokeType(2.0f));
    
    // Dibujar puntos de anclaje
    g.setColour(juce::Colours::gold);
    g.fillEllipse(stringX - 3, stringY - 3, 6, 6);
    g.fillEllipse(stringX + stringWidth - 3, stringY - 3, 6, 6);
    
    // Efecto de vibración (partículas de sonido)
    if (stringAmplitude > 0.1f)
    {
        g.setColour(juce::Colours::white.withAlpha(stringAmplitude * 0.3f));
        for (int i = 0; i < 5; ++i)
        {
            float particleX = stringX + stringWidth * 0.5f + (std::sin(stringPhase + i * 0.5f) * 10.0f);
            float particleY = stringY - 20 - (i * 5);
            g.fillEllipse(particleX - 2, particleY - 2, 4, 4);
        }
    }

    // Footer
    juce::Font footerFont;
    footerFont.setHeight(12.0f);
    footerFont.setItalic(true);
    g.setFont(footerFont);
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText("by David Signals", getLocalBounds().removeFromBottom(30), juce::Justification::centred);
}

void KoruzAudioProcessorEditor::resized()
{
    int sliderSize = 90;
    int labelHeight = 20;
    int spacing = 25;
    int totalWidth = sliderSize * 3 + spacing * 2;
    int startX = (getWidth() - totalWidth) / 2;
    int yPos = 110;

    rateSlider.setBounds(startX, yPos, sliderSize, sliderSize);
    rateLabel.setBounds(startX, yPos + sliderSize + 5, sliderSize, labelHeight);

    depthSlider.setBounds(startX + sliderSize + spacing, yPos, sliderSize, sliderSize);
    depthLabel.setBounds(startX + sliderSize + spacing, yPos + sliderSize + 5, sliderSize, labelHeight);

    mixSlider.setBounds(startX + 2 * (sliderSize + spacing), yPos, sliderSize, sliderSize);
    mixLabel.setBounds(startX + 2 * (sliderSize + spacing), yPos + sliderSize + 5, sliderSize, labelHeight);

    titleLabel.setBounds(0, 15, getWidth(), 50);
}