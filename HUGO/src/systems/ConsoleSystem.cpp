#include "ConsoleSystem.h"
#include "ButtonComponent.h"
#include "LuaBinder.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "InputFieldComponent.h"
#include "Input.h"

HUGO::Int ConsoleSystem::AUX_CONSOLE_INDEX = 0;

void ConsoleSystem::printLuaOutput(const TextComponent& consoleTextCmp){
    while(!ConsoleSystem::luaOutput.empty()){
        scene.modifyComponent<TextComponent>(consoleTextCmp.entity, [this](TextComponent& t){
            t.text += logIni + ConsoleSystem::luaOutput.front() + "\n";
        });
        ConsoleSystem::luaOutput.pop();
    }
}

void ConsoleSystem::printLogs(const TextComponent& consoleTextCmp){
    HUGO::ErrorOrType log = scene.getNextLog();

    while (!log.isError())
    {
        // if not explicitly specified, no \n is included
        const HUGO::String& logText = log.getValue();

        scene.modifyComponent<TextComponent>(consoleTextCmp.entity, [this, logText](TextComponent& t){
            t.text += logIni + logText;
        });

        log = scene.getNextLog();
    }
}

void ConsoleSystem::printErrors(const TextComponent& consoleTextCmp){
    HUGO::ErrorOrType error = scene.getNextError();

    // for errors, \n is included by default
    while(!error.isError()){
        const HUGO::String& errorText = error.getValue();

        scene.modifyComponent<TextComponent>(consoleTextCmp.entity, [this, errorText](TextComponent& t){
            t.text += logIni + "ERROR: " + errorText + "\n";
        });

        error = scene.getNextError();
    }
}

HUGO::String ConsoleSystem::getNextHistoric(){
    HUGO::String item = "-";

    if(historyIndex > 0){ 
        historyIndex--;
    }
    // item out of if so when 0 is reached it is always returned
    if(!history.empty()) item = history[historyIndex];
    return item;
}

HUGO::String ConsoleSystem::getPreviousHistoric(){
    HUGO::String item = " ";

    if(historyIndex < history.size()){
        // item inside of if so when history.size() is reached "" is returned
        historyIndex++;
        if(historyIndex < history.size()) item = history[historyIndex];
    }
    return item;
}

void ConsoleSystem::receiveInput(const HUGO::String& inputText){

    // Register input in history and log it to console
    history.push_back(inputText);
    historyIndex = history.size();
    scene.log(inputText + "\n");

    // Check input
    size_t start = 0;
    size_t end = inputText.find('|');
    HUGO::String command = "";
    HUGO::String type = "";
    bool expectsReturn = false;

    if(end != std::string::npos) {
        type = inputText.substr(start, end - start);
        start = end+1;
        command = inputText.substr(start);
        expectsReturn = true;
    }
    else command = inputText;

    if(expectsReturn){
        if(type == "int"){
            HUGO::ErrorOrType<HUGO::Int> returnedInfo = LuaBinder::getInstance().getFromString<HUGO::Int>(command);
            handleReturn<HUGO::Int>(returnedInfo);
        }
        else if(type == "float"){
            HUGO::ErrorOrType<HUGO::Float> returnedInfo = LuaBinder::getInstance().getFromString<HUGO::Float>(command);
            handleReturn<HUGO::Float>(returnedInfo);
        }
        else if (type == "bool"){
            HUGO::ErrorOrType<bool> returnedInfo = LuaBinder::getInstance().getFromString<bool>(command);
            handleReturn<bool>(returnedInfo);
        }
        else if (type == "string"){
            HUGO::ErrorOrType<HUGO::String> returnedInfo = LuaBinder::getInstance().getFromString<HUGO::String>(command);
            handleReturn<HUGO::String>(returnedInfo);
        }
        else scene.log(std::shared_ptr<HUGO::Error>(new HUGO::StringError("LUA: Invalid return type")));
    }
    else{
        HUGO::ErrorOrVoid error = LuaBinder::getInstance().interpretString(command);
        if(error.isError()) scene.log(std::make_shared<HUGO::StringError>(error.getError()));
    }
}

void ConsoleSystem::updateInputField(HUGO::String nText){

    scene.modifyComponent<InputFieldComponent>(consoleInput, [nText](InputFieldComponent& i){
        i.fullText = nText;
    });
        
    scene.modifyComponent<TextComponent>(consoleInput, [nText](TextComponent& t){
        t.text = nText;
    });
}

void ConsoleSystem::handleKeyInput(){

    HUGO::String result = "";
    if(active && input->wasConsoleKeyJustPressed(HUGO::KeyCode::UP_ARROW)){
        result = getNextHistoric();
        updateInputField(result);
    }
    if(active && input->wasConsoleKeyJustPressed(HUGO::KeyCode::DOWN_ARROW)){
        result = getPreviousHistoric();
        updateInputField(result);
    }

    if((input->isConsoleKeyPressed(HUGO::KeyCode::LCTRL) && input->wasConsoleKeyJustPressed(HUGO::KeyCode::T)) ||
        (input->isKeyPressed(HUGO::KeyCode::LCTRL) && input->wasKeyJustPressed(HUGO::KeyCode::T)))
    {
        active = !active;
        input->setConsoleActive(active);

        HUGO::Vector4 colour = {1.0, 1.0, 1.0, 1.0};
        if(!active) colour.w = 0;

        scene.modifyComponent<PanelComponent>(consoleText, [this](PanelComponent& p){
            p.isVisible = active;
        });

        scene.modifyComponent<PanelComponent>(consoleInfo, [this](PanelComponent& p){
            p.isVisible = active;
        });

        scene.modifyComponent<PanelComponent>(consoleInput, [this](PanelComponent& p){
            p.isVisible = active;
        });
    }
}

void ConsoleSystem::update(uint32_t){

    const TextComponent& consoleTextCmp = scene.getComponentConst<TextComponent>(consoleText);

    // Logs
    printLogs(consoleTextCmp);

    // Lua output
    printLuaOutput(consoleTextCmp);

    // Errors
    printErrors(consoleTextCmp);

    handleKeyInput();
}

void ConsoleSystem::fixedUpdate(uint32_t){
    
}

void ConsoleSystem::onLoad(){
    
}

void ConsoleSystem::init(){
    
    LuaBinder::getInstance().setPrintFunction(consoleLuaPrint);
    active = false;
    historyIndex = 0;
    input = static_cast<HUGO::Input*>(&scene.getInput());
    input->setConsoleActive(active);
    
    //---------------------------------------Input field
    consoleInput = scene.createEntity();

    std::string panelContainerName;

    scene.addComponent<PanelComponent>(consoleInput, [this, &panelContainerName](PanelComponent& panel){
        panel.overlayName = "Console/InputText" + std::to_string(ConsoleSystem::AUX_CONSOLE_INDEX);
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "Console/InputBackground";
        panel.width = 0.3f;
        panel.height = 0.06f;
        panel.left = 0.7f;
        panel.top = 0.41f;
        panel.zOrder = 501;
        panelContainerName = panel.containerName;
        panel.isVisible = active;
    });

    scene.addComponent<TextComponent>(consoleInput, [&panelContainerName](TextComponent& text){
        text.textAreaName = panelContainerName + "/Text";
        text.left = 0.01f;
        text.colourTop = HUGO::Vector4(1.0f, 0.0f, 0.0f, 1.f);
        text.colourBottom = HUGO::Vector4(1.0f, 0.0f, 1.0f, 1.f);
        text.characterHeight = 0.025f;
        text.top = 0.01f;
        text.spaceWidth = 0.01f;
        text.text = "Click to write...";
    });

    scene.addComponent<InputFieldComponent>(consoleInput, [this](InputFieldComponent& input){
        input.isActive = active;
        input.originalText = "| ";
        input.deactiveText = "Click to write...";
        input.onChangeCallback = [this](const HUGO::String& inputText){
            receiveInput(inputText);
        };
    });

    //---------------------------------------Console text

    consoleText = scene.createEntity();

    // Reuse panelContainerName
    scene.addComponent<PanelComponent>(consoleText, [this, &panelContainerName](PanelComponent& panel2){
        panel2.overlayName = "Console/LogText"+std::to_string(ConsoleSystem::AUX_CONSOLE_INDEX);
        panel2.containerName = panel2.overlayName + "/Container";
        panel2.material = "Console/LogBackground";
        panel2.width = 0.3f;
        panel2.height = 0.31f;
        panel2.left = 0.7f;
        panel2.top = 0.1f;
        panel2.zOrder = 501;
        panel2.isVisible = active;
        panelContainerName = panel2.containerName;
    });

    scene.addComponent<TextComponent>(consoleText, [&panelContainerName](TextComponent& text2){
        text2.textAreaName = panelContainerName + "/Text";
        text2.colourTop = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.f);
        text2.colourBottom = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.f);
        text2.left = 0.005f;
        text2.top = 0.01f;
        text2.maxWidth = 0.26f;
        text2.maxHeight = 0.3f;
        text2.characterHeight = 0.024f;
        text2.spaceWidth = 0.01f;
    });

    //-------------------------------Console Input Info
    consoleInfo = scene.createEntity();
    scene.addComponent<PanelComponent>(consoleInfo, [this, &panelContainerName](PanelComponent& panel3){
        panel3.overlayName = "Console/InfoText"+std::to_string(ConsoleSystem::AUX_CONSOLE_INDEX);
        panel3.containerName = panel3.overlayName + "/Container";
        panel3.material = "Console/InputBackground";
        panel3.width = 0.3f;
        panel3.height = 0.04f;
        panel3.left = 0.7f;
        panel3.top = 0.06f;
        panel3.zOrder = 501;
        panel3.isVisible = active;
        panelContainerName = panel3.containerName;
    });

    scene.addComponent<TextComponent>(consoleInfo, [&panelContainerName](TextComponent& text3){
        text3.textAreaName = panelContainerName + "/Text";
        text3.colour = HUGO::Vector4(0.0f, 0.0f, 0.0f, 1.f);
        text3.left = 0.01f;
        text3.top = 0.01f;
        text3.characterHeight = 0.025f;
        text3.spaceWidth = 0.01f;
        text3.text = "Show/Hide Console: Ctrl + T\n";
    });

    AUX_CONSOLE_INDEX++;
}