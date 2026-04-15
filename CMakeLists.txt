cmake_minimum_required(VERSION 3.15)

project(Scalpel VERSION 0.1.0)

# Add JUCE as a subdirectory
add_subdirectory(JUCE)

# Enable all plugin formats (VST3, AU, Standalone)
# CLAP support is available in JUCE 7.0+
juce_add_plugin(Scalpel
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Scal
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Scalpel"
)

# Generate JUCE header
juce_generate_juce_header(Scalpel)

# Add source files
target_sources(Scalpel
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

# Compile definitions
target_compile_definitions(Scalpel
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE modules
target_link_libraries(Scalpel
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
