cmake_minimum_required(VERSION 3.15)

project(Koruz VERSION 1.0.0)

set(JUCE_PROJECT_NAME "Koruz")

if(NOT DEFINED JUCE_ROOT)
    set(JUCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../JUCE)
endif()

find_package(JUCE REQUIRED CONFIG)

juce_add_plugin(${JUCE_PROJECT_NAME}
    VERSION 1.0.0
    COMPANY_NAME "DavidSignals"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    FORMATS VST3 Standalone
    PRODUCT_NAME "Koruz"
    
    MODULES 
        juce_audio_basics 
        juce_audio_processors 
        juce_audio_utils 
        juce_core 
        juce_data_structures 
        juce_events 
        juce_graphics 
        juce_gui_basics 
        juce_gui_extra
)

target_sources(${JUCE_PROJECT_NAME} PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(${JUCE_PROJECT_NAME} PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

juce_generate_juce_header(${JUCE_PROJECT_NAME})