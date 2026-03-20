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
            handleReturn(returnedInfo);
        }
        else if(type == "float"){
            ErrorOrType<HUGO::Float> returnedInfo = LuaBinder::Instance().getFromString<HUGO::Float>(command);
            handleReturn(returnedInfo);
        }
        else if (type == "bool"){
            ErrorOrType<bool> returnedInfo = LuaBinder::Instance().getFromString<bool>(command);
            handleReturn(returnedInfo);
        }
        else if (type == "string"){
            ErrorOrType<HUGO::String> returnedInfo = LuaBinder::Instance().getFromString<HUGO::String>(command);
            handleReturn(returnedInfo);
        }
        else scene->log(std::shared_ptr<Error>(new StringError("LUA: Invalid return type")));
    }
    else LuaBinder::Instance().interpretString(command);
}

void ConsoleSystem::update(double){

    //TextComponent& consoleTextCmp = scene->getComponent<TextComponent>(consoleText);

    // Logs
    //consoleTextCmp.colour = HUGO::Vector4(1.f, 1.f, 1.f, 1.f);
    //printLogs(consoleTextCmp);

    // Errors
    //consoleTextCmp.colour = HUGO::Vector4(1.f, 0.f, 0.f, 1.f);
    //printErrors(consoleTextCmp);
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

    //consoleText = scene->createEntity();

    //PanelComponent& panel2 = scene->addAndGetComponent<PanelComponent>(consoleText);
    //panel2.overlayName = "testCanvas/SimpleTest";
    //panel2.containerName = panel.overlayName + "/Container";
    //panel2.material = "UI/Test";
    //panel2.top = 0.3;
    //panel2.left = 0.3;

    //scene->addComponent<CreatePanelDirtyTag>(consoleText);

    //TextComponent& text2 = scene->addAndGetComponent<TextComponent>(consoleText);
    //text2.textAreaName = panel.containerName + "/Text";

    //scene->addComponent<CreateTextDirtyTag>(consoleText);

    scene->log("HOLA\n");
    StringError testError = StringError("error de prueba");
    scene->log(std::make_shared<StringError>(testError));
}