#include "ConsoleSystem.h"
#include "UITags.h"
#include "LuaBinder.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "InputFieldComponent.h"
#include "LuaBinder.h"

#include <iostream>

void ConsoleSystem::printLogs(TextComponent& consoleText){
    ErrorOrType log = scene->getNextLog();

    while (!log.isError())
    {
        // if not explicitly specified, no \n is included
        consoleText.text += log.getValue();
        log = scene->getNextLog();
    }
    
}

void ConsoleSystem::printErrors(TextComponent& consoleText){
    ErrorOrType error = scene->getNextError();

    while (!error.isError())
    {
        // for errors, \n is included by default
        consoleText.text += error.getValue() + "\n";
        error = scene->getNextError();
    }
}

void ConsoleSystem::receiveInput(const HUGO::String& input){

    size_t start = 0;
    size_t end = input.find(':');
    HUGO::String command = "";
    HUGO::String type = "";
    bool expectsReturn = false;

    if(end != std::string::npos) {
        type = input.substr(start, end - start);
        start = end+1;
        command = input.substr(start);
        expectsReturn = true;
    }
    else command = input;

    if(expectsReturn){
        if(type == "int");
        else if(type == "float");
        else if (type == "bool");
        else if (type == "string");
        else scene->log(std::shared_ptr<Error>(new StringError("LUA: Invalid return type")));
    }
    else LuaBinder::Instance().interpretString(command);
}

void ConsoleSystem::update(double){

    TextComponent& consoleText = scene->getComponent<TextComponent>(console);

    // Logs
    consoleText.colour = HUGO::Vector4(1.f, 1.f, 1.f, 1.f);
    printLogs(consoleText);

    // Errors
    consoleText.colour = HUGO::Vector4(1.f, 0.f, 0.f, 1.f);
    printErrors(consoleText);
}

void ConsoleSystem::fixedUpdate(double){

}

void ConsoleSystem::onLoad(){

}

void ConsoleSystem::init(){

    console = scene->createEntity();

    PanelComponent& panel = scene->addAndGetComponent<PanelComponent>(console);
    panel.overlayName = "testCanvas/SimpleTest";
    panel.containerName = panel.overlayName + "/Container";
    panel.left = 0.3f;
    panel.top = 0.7f;
    panel.height = 0.1f;
    panel.width = 0.1f;
    panel.isVisible = true;
    panel.zOrder = 550;
    panel.material = "UI/Test";

    scene->addComponent<VisibilityDirtyTag>(console);

    TextComponent& text = scene->addAndGetComponent<TextComponent>(console);
    text.textAreaName = panel.containerName + "/Text";
    text.characterHeight = 0.04f;
    text.text = "";
    text.colour = {1.f, 1.f, 1.f, 1.f};
    text.colourTop = {1.f, 1.f, 1.f, 1.f};
    text.colourBottom = {1.f, 1.f, 1.f, 1.f};

    InputFieldComponent& input = scene->addAndGetComponent<InputFieldComponent>(console);
    input.isActive = true;
    input.originalText = "";
    input.onChangeCallback = [this](const HUGO::String& input){
        receiveInput(input);
    };
}