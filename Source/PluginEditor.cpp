/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OOPSAudioProcessorEditor::OOPSAudioProcessorEditor (OOPSAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(false, false);
    setSize (1280, 800);

    
    addAndMakeVisible(voicesSlider);
    voicesSlider.setRange(1, NUM_VOICES, 1);
    voicesSlider.setSliderStyle(juce::Slider::Rotary);
    voicesSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    voicesSlider.onValueChange = [this] {
        audioProcessor.voiceLimit = (int)voicesSlider.getValue();
        };
    voicesSlider.setValue(audioProcessor.voiceLimit, juce::NotificationType::dontSendNotification);

    addAndMakeVisible(moduleTypeSlider            );
    addAndMakeVisible(moduleSelectSlider          );
    addAndMakeVisible(cableSourceModuleSlider     );
    addAndMakeVisible(cableSourceCableSlider      );
    addAndMakeVisible(cableDestinationModuleSlider);
    addAndMakeVisible(cableDestinationCableSlider );
    addAndMakeVisible(cableSelectSlider           );
    

    moduleTypeSlider            .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    moduleSelectSlider          .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    cableSourceModuleSlider     .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    cableSourceCableSlider      .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    cableDestinationModuleSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    cableDestinationCableSlider .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    cableSelectSlider           .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    moduleTypeSlider            .setSliderStyle(juce::Slider::LinearHorizontal);
    moduleSelectSlider          .setSliderStyle(juce::Slider::LinearHorizontal);
    cableSourceModuleSlider     .setSliderStyle(juce::Slider::LinearHorizontal);
    cableSourceCableSlider      .setSliderStyle(juce::Slider::LinearHorizontal);
    cableDestinationModuleSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cableDestinationCableSlider .setSliderStyle(juce::Slider::LinearHorizontal);
    cableSelectSlider           .setSliderStyle(juce::Slider::LinearHorizontal);

    moduleTypeSlider            .onValueChange = [this] { updatePanel(false); };
    moduleSelectSlider          .onValueChange = [this] { updatePanel(false); };
    cableSourceModuleSlider     .onValueChange = [this] { updatePanel(false); };
    cableSourceCableSlider      .onValueChange = [this] { updatePanel(false); };
    cableDestinationModuleSlider.onValueChange = [this] { updatePanel(false); };
    cableDestinationCableSlider .onValueChange = [this] { updatePanel(false); };
    cableSelectSlider           .onValueChange = [this] { updatePanel(false); };
    
    addAndMakeVisible(labelModuleTypeSlider            );
    addAndMakeVisible(labelModuleSelectSlider          );
    addAndMakeVisible(labelCableSourceModuleSlider     );
    addAndMakeVisible(labelCableSourceCableSlider      );
    addAndMakeVisible(labelCableDestinationModuleSlider);
    addAndMakeVisible(labelCableDestinationCableSlider );
    addAndMakeVisible(labelCableSelectSlider           );

    labelModuleTypeSlider            .attachToComponent(&moduleTypeSlider            , false);
    labelModuleSelectSlider          .attachToComponent(&moduleSelectSlider          , false);
    labelCableSourceModuleSlider     .attachToComponent(&cableSourceModuleSlider     , false);
    labelCableSourceCableSlider      .attachToComponent(&cableSourceCableSlider      , false);
    labelCableDestinationModuleSlider.attachToComponent(&cableDestinationModuleSlider, false);
    labelCableDestinationCableSlider .attachToComponent(&cableDestinationCableSlider , false);
    labelCableSelectSlider           .attachToComponent(&cableSelectSlider           , false);

    labelModuleTypeSlider            .setJustificationType(juce::Justification::bottomLeft);
    labelModuleSelectSlider          .setJustificationType(juce::Justification::bottomLeft);
    labelCableSourceModuleSlider     .setJustificationType(juce::Justification::bottomLeft);
    labelCableSourceCableSlider      .setJustificationType(juce::Justification::bottomLeft);
    labelCableDestinationModuleSlider.setJustificationType(juce::Justification::bottomLeft);
    labelCableDestinationCableSlider .setJustificationType(juce::Justification::bottomLeft);
    labelCableSelectSlider           .setJustificationType(juce::Justification::bottomLeft);

    addAndMakeVisible(addModuleButton   );
    addAndMakeVisible(moveModuleButton  );
    addAndMakeVisible(deleteModuleButton);
    addAndMakeVisible(addCableButton    );
    addAndMakeVisible(deleteCableButton );
    addModuleButton   .setButtonText("Add Module");
    moveModuleButton  .setButtonText("Move Module To End");
    deleteModuleButton.setButtonText("Delete Module");
    addCableButton    .setButtonText("Add Cable");
    deleteCableButton .setButtonText("Delete Cable");

    addModuleButton   .onClick = [this] { audioProcessor.insertNewModule(-1, ModuleOrder.at((int)moduleTypeSlider.getValue())); updatePanel(false); };
    moveModuleButton  .onClick = [this] { audioProcessor.moveModule(-1, (int)moduleSelectSlider.getValue() + 1); updatePanel(false); };
    deleteModuleButton.onClick = [this] { audioProcessor.removeModule((int)moduleSelectSlider.getValue() + 1); updatePanel(false); };
    addCableButton    .onClick = [this] { audioProcessor.insertCable(
        (int)cableSourceModuleSlider.getValue(),
        (int)cableSourceCableSlider.getValue(),
        (int)cableDestinationModuleSlider.getValue(),
        (int)cableDestinationCableSlider.getValue()
    ); updatePanel(false); };
    deleteCableButton .onClick = [this] {audioProcessor.removeCable((int)cableSelectSlider.getValue()); updatePanel(false); };

    updatePanel(true);
}

OOPSAudioProcessorEditor::~OOPSAudioProcessorEditor()
{
}

//==============================================================================
void OOPSAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (audioProcessor.processingOrder.size() != oldPOSize) resized();
    oldPOSize = (int)audioProcessor.processingOrder.size();
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(70.0f));
    auto area = getLocalBounds();
    area.removeFromRight(1080);
    g.drawText("OOPS", area.removeFromTop(80),
        juce::Justification::centred, true);
}

void OOPSAudioProcessorEditor::updatePanel(bool initialize) {
    moduleTypeSlider.setRange(0, ModuleOrder.size()-1, 1);
    moduleSelectSlider.setRange(0, audioProcessor.processingOrder.size() - 2, 1);
    cableSourceModuleSlider.setRange(0, audioProcessor.processingOrder.size() - 1, 1);
    cableDestinationModuleSlider.setRange(0, audioProcessor.processingOrder.size() - 1, 1);
    cableSelectSlider.setRange(0, audioProcessor.plugs.size()-1, 1);

    if (initialize) {
        cableSourceCableSlider.setRange(0, audioProcessor.processingOrder[0]->getNumCables(), 1);
        cableDestinationCableSlider.setRange(0, audioProcessor.processingOrder[0]->getNumCables(), 1);

        moduleTypeSlider            .setValue(0, juce::dontSendNotification);
        moduleSelectSlider          .setValue(0, juce::dontSendNotification);
        cableSourceModuleSlider     .setValue(0, juce::dontSendNotification);
        cableSourceCableSlider      .setValue(0, juce::dontSendNotification);
        cableDestinationModuleSlider.setValue(0, juce::dontSendNotification);
        cableDestinationCableSlider .setValue(0, juce::dontSendNotification);
        cableSelectSlider           .setValue(0, juce::dontSendNotification);
        
        addModuleButton   .setToggleState(false, juce::dontSendNotification);
        moveModuleButton  .setToggleState(false, juce::dontSendNotification);
        deleteModuleButton.setToggleState(false, juce::dontSendNotification);
        addCableButton    .setToggleState(false, juce::dontSendNotification);
        deleteCableButton .setToggleState(false, juce::dontSendNotification);
    }
    else {
        cableSourceCableSlider.setRange(0, audioProcessor.processingOrder[(int)cableSourceModuleSlider.getValue()]->getNumCables(), 1);
        cableDestinationCableSlider.setRange(0, audioProcessor.processingOrder[(int)cableDestinationModuleSlider.getValue()]->getNumCables(), 1);
    }

    juce::String s;

    s = "Type: ";
    s.append(ModuleStrings.at(ModuleOrder.at((int)moduleTypeSlider.getValue())), 128);
    labelModuleTypeSlider.setText(s, juce::dontSendNotification);

    s = "Module: ";
    s.append(juce::String((int)moduleSelectSlider.getValue() + 1), 128);
    s.append(" - ", 5);
    s.append(ModuleStrings.at(audioProcessor.processingOrder[(int)moduleSelectSlider.getValue() + 1]->moduleType), 128);
    labelModuleSelectSlider.setText(s, juce::dontSendNotification);

    s = "S: M";
    s.append(juce::String((int)cableSourceModuleSlider.getValue()), 128);
    s.append(" - ", 5);
    s.append(ModuleStrings.at(audioProcessor.processingOrder[(int)cableSourceModuleSlider.getValue()]->moduleType), 128);
    labelCableSourceModuleSlider.setText(s, juce::dontSendNotification);

    s = "S: C";
    s.append(juce::String((int)cableSourceCableSlider.getValue()), 128);
    s.append(" - ", 5);
    s.append(audioProcessor.processingOrder[(int)cableSourceModuleSlider.getValue()]->getCableName((int)cableSourceCableSlider.getValue()), 128);
    labelCableSourceCableSlider.setText(s, juce::dontSendNotification);

    s = "D: M";
    s.append(juce::String((int)cableDestinationModuleSlider.getValue()), 128);
    s.append(" - ", 5);
    s.append(ModuleStrings.at(audioProcessor.processingOrder[(int)cableDestinationModuleSlider.getValue()]->moduleType), 128);
    labelCableDestinationModuleSlider.setText(s, juce::dontSendNotification);

    s = "D: C";
    s.append(juce::String((int)cableDestinationCableSlider.getValue()), 128);
    s.append(" - ", 5);
    s.append(audioProcessor.processingOrder[(int)cableDestinationModuleSlider.getValue()]->getCableName((int)cableDestinationCableSlider.getValue()), 128);
    labelCableDestinationCableSlider.setText(s, juce::dontSendNotification);

    s = "Cable: ";
    s.append(juce::String((int)cableSelectSlider.getValue()), 128);
    s.append(" - ", 5);
    s.append(juce::String(audioProcessor.plugs[(int)cableSelectSlider.getValue()].sourceM), 128);
    s.append(":", 3);
    s.append(juce::String(audioProcessor.plugs[(int)cableSelectSlider.getValue()].sourceC), 128);
    s.append("->", 4);
    s.append(juce::String(audioProcessor.plugs[(int)cableSelectSlider.getValue()].destM), 128);
    s.append(":", 3);
    s.append(juce::String(audioProcessor.plugs[(int)cableSelectSlider.getValue()].destC), 128);
    labelCableSelectSlider.setText(s, juce::dontSendNotification);
}

void OOPSAudioProcessorEditor::resized()
{
    setSize(1280, 800);
    std::vector<juce::Rectangle<int>> moduleSlots;
    juce::Rectangle<int> panel = getLocalBounds();
    juce::Rectangle<int> area = panel.removeFromRight(1080);
    juce::Rectangle<int> area2 = area.removeFromBottom(400);
    panel.removeFromTop(80);

    // Modules
    for (int i = 0; i < 8; i++) {
        moduleSlots.push_back(area.removeFromLeft(135));
    }

    for (int i = 0; i < 8; i++) {
        moduleSlots.push_back(area2.removeFromLeft(135));
    }

    for (int i = 0; i < std::min(audioProcessor.processingOrder.size(), moduleSlots.size()); i++) {
        addAndMakeVisible(audioProcessor.processingOrder[i]);
        audioProcessor.processingOrder[i]->setBounds(moduleSlots[i]);
    }

    panel.expand(-5, -5);

    // Side panel
    voicesSlider.setBounds(panel.removeFromBottom(100).expanded(-5, -5));

    moduleTypeSlider            .setBounds(panel.removeFromTop(40).removeFromBottom(20));
    addModuleButton.setBounds(panel.removeFromTop(20));
    panel.removeFromTop(20);

    moduleSelectSlider          .setBounds(panel.removeFromTop(40).removeFromBottom(20));
    moveModuleButton.setBounds(panel.removeFromTop(20));
    panel.removeFromTop(5);
    deleteModuleButton.setBounds(panel.removeFromTop(20));
    panel.removeFromTop(20);

    cableSourceModuleSlider     .setBounds(panel.removeFromTop(40).removeFromBottom(20));
    cableSourceCableSlider      .setBounds(panel.removeFromTop(40).removeFromBottom(20));
    cableDestinationModuleSlider.setBounds(panel.removeFromTop(40).removeFromBottom(20));
    cableDestinationCableSlider .setBounds(panel.removeFromTop(40).removeFromBottom(20));
    addCableButton.setBounds(panel.removeFromTop(20));
    panel.removeFromTop(20);

    cableSelectSlider           .setBounds(panel.removeFromTop(40).removeFromBottom(20));
    deleteCableButton .setBounds(panel.removeFromTop(20));
    panel.removeFromTop(20);
}
