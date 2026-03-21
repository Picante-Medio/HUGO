#include "ConsoleSystem.h"
#include "UITags.h"
#include "LuaBinder.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "InputFieldComponent.h"
#include "LuaBinder.h"

#include <iostream>

void ConsoleSystem::printLogs(TextComponent& consoleTextCmp){
    ErrorOrType log = scene->getNextLog();

    while (!log.isError())
    {
        // if not explicitly specified, no \n is included
        consoleTextCmp.text += log.getValue();
        log = scene->getNextLog();
    }
    
}

void ConsoleSystem::printErrors(TextComponent& consoleTextCmp){
    ErrorOrType error = scene->getNextError();

    while (!error.isError())
    {
        // for errors, \n is included by default
        consoleTextCmp.text += "ERROR: " + error.getValue() + "\n";
        error = scene->getNextError();
    }
}

HUGO::String ConsoleSystem::getNextHistoric(){
    HUGO::String item = "";

    if(historyIndex > 0){ 
        historyIndex--;
    }
    // item out of if so when 0 is reached it is always returned
    item = history[historyIndex];
    return item;
}

HUGO::String ConsoleSystem::getPreviousHistoric(){
    HUGO::String item = "";

    if(++historyIndex < history.size()){
        // item inside of if so when history.size() is reached "" is returned
        item = history[historyIndex];
    }
    else historyIndex--;

    return item;
}

void ConsoleSystem::receiveInput(const HUGO::String& input){

    // Register input in history and log it to console
    history.push_back(input);
    historyIndex = history.size();
    scene->log(input + "\n");

    // Check input
    size_t start = 0;
    size_t end = input.find('|');
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
        if(type == "int"){
            ErrorOrType<HUGO::Int> returnedInfo = LuaBinder::Instance().getFromString<HUGO::Int>(command);
            handleReturn<HUGO::Int>(returnedInfo);
        }
        else if(type == "float"){
            ErrorOrType<HUGO::Float> returnedInfo = LuaBinder::Instance().getFromString<HUGO::Float>(command);
            handleReturn<HUGO::Float>(returnedInfo);
        }
        else if (type == "bool"){
            ErrorOrType<bool> returnedInfo = LuaBinder::Instance().getFromString<bool>(command);
            handleReturn<bool>(returnedInfo);
        }
        else if (type == "string"){
            ErrorOrType<HUGO::String> returnedInfo = LuaBinder::Instance().getFromString<HUGO::String>(command);
            handleReturn<HUGO::String>(returnedInfo);
        }
        else scene->log(std::shared_ptr<Error>(new StringError("LUA: Invalid return type")));
    }
    else{
        ErrorOrVoid error = LuaBinder::Instance().interpretString(command);
        if(error.isError()) scene->log(std::make_shared<StringError>(error.getError()));
    }
}

void ConsoleSystem::update(double){

    TextComponent& consoleTextCmp = scene->getComponent<TextComponent>(consoleText);

    // Text is completely white because there is no way right now to make text change colour in the same update

    // Logs
    //consoleTextCmp.colourTop = HUGO::Vector4(1.f, 1.f, 1.f, 1.f);
    //consoleTextCmp.colourBottom = HUGO::Vector4(1.f, 1.f, 1.f, 1.f);
    printLogs(consoleTextCmp);

    // Errors
    //consoleTextCmp.colourTop = HUGO::Vector4(1.f, 0.f, 0.f, 1.f);
    //consoleTextCmp.colourBottom = HUGO::Vector4(1.f, 0.f, 0.f, 1.f);
    printErrors(consoleTextCmp);

    scene->addComponent<TextDirtyTag>(consoleText);
}

void ConsoleSystem::fixedUpdate(double){

}

void ConsoleSystem::onLoad(){

}

void ConsoleSystem::init(){

    //---------------------------------------Input field
    consoleInput = scene->createEntity();

    PanelComponent& panel = scene->addAndGetComponent<PanelComponent>(consoleInput);
    panel.overlayName = "testCanvas/SimpleTest";
    panel.containerName = panel.overlayName + "/Container";
    panel.material = "UI/Test";

    scene->addComponent<CreatePanelDirtyTag>(consoleInput);

    TextComponent& text = scene->addAndGetComponent<TextComponent>(consoleInput);
    text.textAreaName = panel.containerName + "/Text";

    scene->addComponent<CreateTextDirtyTag>(consoleInput);

    InputFieldComponent& input = scene->addAndGetComponent<InputFieldComponent>(consoleInput);
    input.isActive = true;
    input.originalText = "";
    input.onChangeCallback = [this](const HUGO::String& input){
        receiveInput(input);
    };

    scene->addComponent<CreateInputFieldDirtyTag>(consoleInput);

    //---------------------------------------Console text

    consoleText = scene->createEntity();

    PanelComponent& panel2 = scene->addAndGetComponent<PanelComponent>(consoleText);
    panel2.overlayName = "testCanvas/ConsoleTextTest";
    panel2.containerName = panel2.overlayName + "/Container";
    panel2.material = "UI/Test";

    scene->addComponent<CreatePanelDirtyTag>(consoleText);

    TextComponent& text2 = scene->addAndGetComponent<TextComponent>(consoleText);
    text2.textAreaName = panel2.containerName + "/Text";

    scene->addComponent<CreateTextDirtyTag>(consoleText);

    scene->log("HOLA\n");
    StringError testError = StringError("error de prueba");
    scene->log(std::make_shared<StringError>(testError));
}