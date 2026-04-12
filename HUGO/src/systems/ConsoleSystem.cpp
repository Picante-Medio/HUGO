#include "ConsoleSystem.h"
#include "LuaBinder.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "InputFieldComponent.h"

void ConsoleSystem::printLuaOutput(TextComponent& consoleTextCmp){

    for (auto& pack: scene.query<TextComponent>()){
        EntityID id = pack.getConst<TextComponent>().entity;
        if (id != consoleTextCmp.entity) continue;

        while(!ConsoleSystem::luaOutput.empty()){
            pack.modify<TextComponent>([this](TextComponent& t){
                t.text += logIni + ConsoleSystem::luaOutput.front() + "\n";
            });
            ConsoleSystem::luaOutput.pop();
        }
    }
}

void ConsoleSystem::printLogs(TextComponent& consoleTextCmp){
    ErrorOrType log = scene.getNextLog();

    for (auto& pack: scene.query<TextComponent>()){
        EntityID id = pack.getConst<TextComponent>().entity;
        if (id != consoleTextCmp.entity) continue;

        while (!log.isError())
        {
            // if not explicitly specified, no \n is included
            const HUGO::String& logText = log.getValue();
            pack.modify<TextComponent>([this, logText](TextComponent& t){
                t.text += logIni + logText;
            });
            log = scene.getNextLog();
        }

    }
}

void ConsoleSystem::printErrors(TextComponent& consoleTextCmp){
    ErrorOrType error = scene.getNextError();

    for (auto& pack: scene.query<TextComponent>()){
        EntityID id = pack.getConst<TextComponent>().entity;
        if (id != consoleTextCmp.entity) continue;

        // for errors, \n is included by default
        while(!error.isError()){
            const HUGO::String& errorText = error.getValue();
            pack.modify<TextComponent>([this, errorText](TextComponent& t){
                t.text += logIni + "ERROR: " + errorText + "\n";
            });
            error = scene.getNextError();
        }
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
    HUGO::String item = "";

    if(historyIndex < history.size()){
        // item inside of if so when history.size() is reached "" is returned
        historyIndex++;
        if(historyIndex < history.size()) item = history[historyIndex];
    }

    return item;
}

void ConsoleSystem::receiveInput(const HUGO::String& input){

    // Register input in history and log it to console
    history.push_back(input);
    historyIndex = history.size();
    scene.log(input + "\n");

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
            ErrorOrType<HUGO::Int> returnedInfo = LuaBinder::getInstance().getFromString<HUGO::Int>(command);
            handleReturn<HUGO::Int>(returnedInfo);
        }
        else if(type == "float"){
            ErrorOrType<HUGO::Float> returnedInfo = LuaBinder::getInstance().getFromString<HUGO::Float>(command);
            handleReturn<HUGO::Float>(returnedInfo);
        }
        else if (type == "bool"){
            ErrorOrType<bool> returnedInfo = LuaBinder::getInstance().getFromString<bool>(command);
            handleReturn<bool>(returnedInfo);
        }
        else if (type == "string"){
            ErrorOrType<HUGO::String> returnedInfo = LuaBinder::getInstance().getFromString<HUGO::String>(command);
            handleReturn<HUGO::String>(returnedInfo);
        }
        else scene.log(std::shared_ptr<Error>(new StringError("LUA: Invalid return type")));
    }
    else{
        ErrorOrVoid error = LuaBinder::getInstance().interpretString(command);
        if(error.isError()) scene.log(std::make_shared<StringError>(error.getError()));
    }
}

void ConsoleSystem::handleKeyInput(){

    HUGO::String result = "";
    if(active && scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::UP_ARROW)){
        result = getNextHistoric();
    }
    if(active && scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::DOWN_ARROW)){
        result = getPreviousHistoric();
    }

    for (auto& pack: scene.query<TextComponent, InputFieldComponent, PanelComponent>()){
        InputFieldComponent& inp = pack.getConst<InputFieldComponent&>();
        //search for the console input
        if (inp.entity != consoleInput || result.empty()) continue;
        
        pack.modify<InputFieldComponent>([result](InputFieldComponent& i){
            i.fullText = result;
        });
        pack.modify<TextComponent>([result](TextComponent& t){
            t.text = result;
        });
    }

    if(scene.getInput()->isKeyPressed(HUGO::KeyCode::LCTRL) &&
    scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::T))
    {
        active = !active;
        HUGO::Vector4 colour = {1.0, 1.0, 1.0, 1.0};
        if(!active) colour.w = 0;
        
        for (auto& pack: scene.query<PanelComponent>()){
            PanelComponent& panel = pack.getConst<PanelComponent&>();
            if (panel.entity != consoleInput && panel.entity != consoleText && panel.entity != consoleInfo)
                continue;
            
            pack.modify<PanelComponent>([this](PanelComponent& p){
                p.isVisible = active;
            });
        } 
    }
}

void ConsoleSystem::update(double){

    TextComponent& consoleTextCmp = scene.getComponent<TextComponent>(consoleText);

    // Logs
    printLogs(consoleTextCmp);

    // Lua output
    printLuaOutput(consoleTextCmp);

    // Errors
    printErrors(consoleTextCmp);

    handleKeyInput();
}

void ConsoleSystem::fixedUpdate(double){
    
}

void ConsoleSystem::onLoad(){
    
}

void ConsoleSystem::init(){
    
    LuaBinder::getInstance().setPrintFunction(consoleLuaPrint);
    active = true;
    historyIndex = 0;

    //---------------------------------------Input field
    consoleInput = scene.createEntity();

    std::string panelContainerName;

    scene.addComponent<PanelComponent>(consoleInput, [&panelContainerName](PanelComponent& panel){
        panel.overlayName = "Console/InputText";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "Console/InputBackground";
        panel.width = 0.3f;
        panel.height = 0.06f;
        panel.left = 0.7f;
        panel.top = 0.41f;
        
        panelContainerName = panel.containerName;
    });

    scene.addComponent<TextComponent>(consoleInput, [&panelContainerName](TextComponent& text){
        text.textAreaName = panelContainerName + "/Text";
        text.left = 0.01f;
        text.colourTop = HUGO::Vector4(1.0f, 0.0f, 0.0f, 1.f);
        text.colourBottom = HUGO::Vector4(1.0f, 0.0f, 1.0f, 1.f);
        text.characterHeight = 0.025f;
        text.top = -0.01f;
        text.spaceWidth = 0.01f;
        text.text = "-";
    });

    scene.addComponent<InputFieldComponent>(consoleInput, [this](InputFieldComponent& input){
        input.isActive = true;
        input.originalText = "";
        input.onChangeCallback = [this](const HUGO::String& input){
            receiveInput(input);
        };
    });

    //---------------------------------------Console text

    consoleText = scene.createEntity();

    // Reuse panelContainerName
    scene.addComponent<PanelComponent>(consoleText, [&panelContainerName](PanelComponent& panel2){
        panel2.overlayName = "Console/LogText";
        panel2.containerName = panel2.overlayName + "/Container";
        panel2.material = "Console/LogBackground";
        panel2.width = 0.3f;
        panel2.height = 0.31f;
        panel2.left = 0.7f;
        panel2.top = 0.1f;
        
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
    scene.addComponent<PanelComponent>(consoleInfo, [&panelContainerName](PanelComponent& panel3){
        panel3.overlayName = "Console/InfoText";
        panel3.containerName = panel3.overlayName + "/Container";
        panel3.material = "Console/InputBackground";
        panel3.width = 0.3f;
        panel3.height = 0.04f;
        panel3.left = 0.7f;
        panel3.top = 0.06f;
        panel3.zOrder = 501;
        panelContainerName = panel3.containerName;
    });

    scene.addComponent<TextComponent>(consoleInfo, [&panelContainerName](TextComponent& text3){
        text3.textAreaName = panelContainerName + "/Text";
        text3.colour = HUGO::Vector4(0.0f, 0.0f, 0.0f, 1.f);
        text3.left = 0.01f;
        text3.top = 0.01f;
        text3.characterHeight = 0.025f;
        text3.spaceWidth = 0.01f;
        text3.textFont = "Roboto";
        text3.text = "Show/Hide Console: Ctrl + T\n";
    });
}