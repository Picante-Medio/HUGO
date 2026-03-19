#include "UISystem.h"

#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"
#include "ConsoleComponent.h"
#include "InputFieldComponent.h"
#include "Input.h"
#include <SDL.h>

#include <fstream>
#include <iostream>

#include "OgreModule.h"
#include "OgreUtils.h"

#include "OgreTextAreaOverlayElement.h"

//====================================================
// DESTRUCTOR
//====================================================

UISystem::~UISystem()
{
    clearAllCaches();
}

//====================================================
// CACHE CREATION
//====================================================

std::shared_ptr<UIElementCache> UISystem::createElementCache(EntityID entity,
    Ogre::Overlay *o, Ogre::OverlayContainer *c, Ogre::OverlayElement *e)
{
    std::shared_ptr<UIElementCache> cache = std::make_shared<UIElementCache>();
    cache->overlay = o;
    cache->container = c;
    cache->element = e;

    uiCache[entity] =cache;

    return cache;
}

std::shared_ptr<UIElementCache> UISystem::createElementCache(EntityID entity, const HUGO::String &overlayName, const HUGO::String &containerName, const HUGO::String &elementName)
{
    std::shared_ptr<UIElementCache> cache = std::make_shared<UIElementCache>();
    
    ErrorOrType<Ogre::Overlay*> overlayResult = OgreOverlayUtils::getOverlay(overlayName);
    if (!overlayResult.isError())
        cache->overlay = overlayResult.getValue();
    
    ErrorOrType<Ogre::OverlayContainer*> containerResult = OgreOverlayUtils::getContainer(containerName);
    if (!containerResult.isError())
        cache->container = containerResult.getValue();
    
    if (elementName != ""){
        ErrorOrType<Ogre::OverlayElement*> elementResult = OgreOverlayUtils::getElement(elementName);
        if (!elementResult.isError())
            cache->element = elementResult.getValue();
    }

    uiCache[entity] = cache;
    return cache;
}

std::shared_ptr<UIProgressBarCache> UISystem::createProgressBarCache(
    EntityID entity,
    const HUGO::String& overlayName,
    const HUGO::String& containerName,
    const HUGO::String& descriptionName) // opcional
{
    // create base cache
    std::shared_ptr<UIElementCache> baseCache = createElementCache(entity, overlayName, containerName, descriptionName);
    std::shared_ptr<UIProgressBarCache> cache = std::make_shared<UIProgressBarCache>();

    // copy from base cache to cache
    cache->overlay = baseCache->overlay;
    cache->container = baseCache->container;

    // set specific elements of progress bar
    HUGO::String backgroundName = containerName + "/Background";
    ErrorOrType<Ogre::OverlayElement*> bgResult = OgreOverlayUtils::getElement(backgroundName);
    if (!bgResult.isError())
        cache->background = static_cast<Ogre::OverlayContainer*>(bgResult.getValue());

    HUGO::String fillName = containerName + "/Fill";
    ErrorOrType<Ogre::OverlayElement*> fillResult = OgreOverlayUtils::getElement(fillName);
    if (!fillResult.isError())
        cache->fill = static_cast<Ogre::OverlayContainer*>(fillResult.getValue());

    // registry in the map
    uiCache[entity] = cache;

    return cache;
}

UIElementCache* UISystem::getCache(EntityID entity)
{
    auto it = uiCache.find(entity);
    if (it == uiCache.end())
        return nullptr;

    return it->second.get();
}

bool UISystem::isMouseOverPanel(const PanelComponent &panel, HUGO::Float mouseX, HUGO::Float mouseY)
{
    HUGO::Float left   = panel.left;
    HUGO::Float top    = panel.top;
    HUGO::Float right  = panel.left + panel.width;
    HUGO::Float bottom = panel.top + panel.height;

    return (mouseX >= left && mouseX <= right &&
            mouseY >= top  && mouseY <= bottom);
}

//====================================================
// SYNCHRONIZATION
//====================================================

void UISystem::syncPanel(const PanelComponent &p)
{
    UIElementCache *cache = getCache(p.entity);
    if (!cache)
        return;

    cache->overlay->setZOrder(static_cast<Ogre::ushort>(p.zOrder));
    cache->container->setLeft(p.left);
    cache->container->setTop(p.top);
    cache->container->setWidth(p.width);
    cache->container->setHeight(p.height);
    cache->container->setHorizontalAlignment(OgreUtils::toOgre(p.alignmentX));
    cache->container->setVerticalAlignment(OgreUtils::toOgre(p.alignmentY));
    cache->container->setMetricsMode(OgreUtils::toOgre(p.metricsMode));

    if (!p.material.empty())
        cache->container->setMaterialName(p.material);
}

void UISystem::syncText(const TextComponent &t)
{
    UIElementCache *cache = getCache(t.entity);
    if (!cache)
        return;

    Ogre::TextAreaOverlayElement *textArea = dynamic_cast<Ogre::TextAreaOverlayElement *>(cache->element);
    if (!textArea)
        return;

    textArea->setCaption(t.text);
    textArea->setCharHeight(t.characterHeight);
    textArea->setColour(Ogre::ColourValue(
        t.colour.x,
        t.colour.y,
        t.colour.z,
        t.colour.w));
    textArea->setColourTop(Ogre::ColourValue(
        t.colourTop.x,
        t.colourTop.y,
        t.colourTop.z,
        t.colourTop.w));
    textArea->setColourBottom(Ogre::ColourValue(
        t.colourBottom.x,
        t.colourBottom.y,
        t.colourBottom.z,
        t.colourBottom.w));
}

void UISystem::syncButton(const ButtonComponent &b)
{
    UIElementCache* cache = getCache(b.entity);
    if (!cache || !cache->container) return;

    // cache->container->setLeft(b.left);
    // cache->container->setTop(b.top);
    // cache->container->setWidth(b.width);
    // cache->container->setHeight(b.height);
    // cache->overlay->setZOrder(b.zOrder);
    // cache->container->setMetricsMode(OgreUtils::toOgre(b.metricsMode));

    switch (b.state) {
        case HUGO::ButtonStateTypes::BS_UP:
            if (!b.normalMaterial.empty())
                cache->container->setMaterialName(b.normalMaterial);
            break;
        case HUGO::ButtonStateTypes::BS_OVER:
            if (!b.hoverMaterial.empty())
                cache->container->setMaterialName(b.hoverMaterial);
            break;
        case HUGO::ButtonStateTypes::BS_DOWN:
            if (!b.pressedMaterial.empty())
                cache->container->setMaterialName(b.pressedMaterial);
            break;
        default:
            if (!b.normalMaterial.empty())
                cache->container->setMaterialName(b.normalMaterial);
            break;
    }

    // if (cache->element) {
    //     Ogre::TextAreaOverlayElement* textArea =
    //         dynamic_cast<Ogre::TextAreaOverlayElement*>(cache->element);
    //     if (textArea) {
    //         textArea->setCaption(b.text);
    //         textArea->setCharHeight(b.textSize);
    //         textArea->setFontName(b.fontName);
    //     }
    // }
}

//====================================================
// PROCESSING
//====================================================
void UISystem::processPanelChanges()
{
   for (auto &pack : scene->query<PanelComponent, PanelDirtyTag>())
    {
        PanelComponent& panel = pack.get<PanelComponent&>();
        syncPanel(panel);
        scene->removeComponent<PanelDirtyTag>(panel.entity);
    }
}

void UISystem::processVisibility()
{
    for (auto &pack : scene->query<PanelComponent, VisibilityDirtyTag>())
    {
        PanelComponent &panel = pack.get<PanelComponent &>();
        UIElementCache *cache = getCache(panel.entity);

        if (cache && cache->overlay && cache->container)
        {
            if (panel.isVisible)
            {
                cache->overlay->show();
                cache->container->show();
                if (cache->element)
                    cache->element->show();
            }
            else
            {
                cache->overlay->hide();
                cache->container->hide();
                if (cache->element)
                    cache->element->hide();
            }
        }

        scene->removeComponent<VisibilityDirtyTag>(panel.entity);
    }
}


ErrorOrVoid UISystem::processComponentCreations()
{
    for (auto &pack : scene->query<PanelComponent, CreatePanelDirtyTag>())
    {
        PanelComponent &panel = pack.get<PanelComponent &>();
        CreatePanelDirtyTag &tag = pack.get<CreatePanelDirtyTag &>();
        
        ErrorOrType<Ogre::Overlay *> overlayResult = OgreOverlayUtils::getOverlay(panel.overlayName);
        if (overlayResult.isError())
            return ErrorOrVoid("[UISystem] Panel entity " + std::to_string(panel.entity) + " missing overlay");;

        Ogre::Overlay *overlay = overlayResult.getValue();
        panel.zOrder = overlay->getZOrder();

        ErrorOrType<Ogre::OverlayContainer *> containerResult = OgreOverlayUtils::getContainer(panel.containerName);
        if (containerResult.isError())
            return ErrorOrVoid("[UISystem] Panel entity " + std::to_string(panel.entity) + " missing container");


        Ogre::OverlayContainer *container = containerResult.getValue();

        panel.left = container->getLeft();
        panel.top = container->getTop();
        panel.width = container->getWidth();
        panel.height = container->getHeight();
        panel.metricsMode = OgreUtils::toHugo(container->getMetricsMode());
        panel.alignmentX = OgreUtils::toHugo(container->getHorizontalAlignment());
        panel.alignmentY = OgreUtils::toHugo(container->getVerticalAlignment());
        
        container->setMaterialName(panel.material);
        scene->addComponent<VisibilityDirtyTag>(panel.entity);
        // insert shared_ptr directly
        uiCache[panel.entity] = createElementCache(overlay, container);
        syncPanel(panel);
        scene->removeComponent<CreatePanelDirtyTag>(tag.entity);
    }
    
    for (auto &pack : scene->query<CreateTextDirtyTag, TextComponent, PanelComponent>())
    {
        TextComponent &text = pack.get<TextComponent &>();
        CreateTextDirtyTag &tag = pack.get<CreateTextDirtyTag &>();

        ErrorOrType<Ogre::OverlayElement *> elementResult = OgreOverlayUtils::getElement(text.textAreaName);
        if (elementResult.isError())
            return ErrorOrVoid("[UISystem] Text entity " + std::to_string(text.entity) + " missing element");

        Ogre::TextAreaOverlayElement *area = dynamic_cast<Ogre::TextAreaOverlayElement *>(elementResult.getValue());
        if (!area)
            return ErrorOrVoid("[UISystem] Text entity " + std::to_string(text.entity) + " missing container TextAreaOverlayElement");

        text.characterHeight = area->getCharHeight();
        text.text = area->getCaption();

        Ogre::ColourValue c = area->getColour();
        Ogre::ColourValue ct = area->getColourTop();
        Ogre::ColourValue cb = area->getColourBottom();

        text.colour = {c.r, c.g, c.b, c.a};
        text.colourTop = {ct.r, ct.g, ct.b, ct.a};
        text.colourBottom = {cb.r, cb.g, cb.b, cb.a};

        UIElementCache *cache = getCache(text.entity);
        if (cache)
            cache->element = area;

        syncText(text.entity);
        scene->removeComponent<CreateTextDirtyTag>(tag.entity);
    }

    // for (auto &pack : scene->query<CreateInputFieldDirtyTag, InputFieldComponent, PanelComponent, TextComponent>())
    // {
    //     InputFieldComponent &inputfield = pack.get<InputFieldComponent &>();
    //     CreateInputFieldDirtyTag &tag = pack.get<CreateInputFieldDirtyTag &>();
    //     scene->removeComponent<CreateInputFieldDirtyTag>(tag.entity);
    // }
    return ErrorOrVoid();
}

void UISystem::processTextChanges()
{
    for (auto &pack : scene->query<PanelComponent, TextComponent, TextDirtyTag>())
    {
        TextComponent &text = pack.get<TextComponent &>();
        TextDirtyTag &tag = pack.get<TextDirtyTag &>();
        //syncText(panel, text, tag);
        syncText(text);
        scene->removeComponent<TextDirtyTag>(tag.entity);
    }
}

void UISystem::processInput()
{
    HUGO::Input& input = HUGO::Input::Instance();

    bool anyFieldActive = false; // flag to control SDL text input

    // handle mouse click: activate/deactivate input fields
    if (input.wasMouseButtonJustPressed(SDL_BUTTON_LEFT)) {
        for (auto& pack : scene->query<PanelComponent, TextComponent, InputFieldComponent>()) {
            PanelComponent& panel = pack.get<PanelComponent&>();
            TextComponent& text = pack.get<TextComponent&>();
            InputFieldComponent& field = pack.get<InputFieldComponent&>();

            // get normalized mouse position
            HUGO::Input::MouseRelativePosition mp = input.getMouseRelativePosition();
            if (isMouseOverPanel(panel, mp.x, mp.y)) {
                if (!field.isActive){
                    // activate this field
                    field.isActive = true;
                    anyFieldActive = true;
                    if (field.originalText.empty()){
                        field.originalText = text.text;
                        text.text.clear();
                        scene->addComponent<TextDirtyTag>(text.entity);
                    }
                }
            }
            else {
                field.isActive = false;
            }
        }
        input.toggleTextInput(anyFieldActive);
    }

    // process text for the active input field using SDL text buffer
    const HUGO::String& textInput = input.getTextInput();
    for (auto &pack : scene->query<InputFieldComponent, TextComponent>()) {
        InputFieldComponent& field = pack.get<InputFieldComponent&>();
        if (!field.isActive) continue;

        TextComponent& text = pack.get<TextComponent&>();
        HUGO::String str = text.text; 
        bool changed = false;

        // append new text input from SDL
        if (!textInput.empty()) {
            int c = static_cast<int>(textInput[0]);
            //only english printable ascii codes are accepted
            if (c > 32 && c < 127){
                str += textInput;
                changed = true;
            }
        }

        // backspace -> remove last character
        if (input.wasKeyJustPressed(SDLK_BACKSPACE) && !str.empty()) {
            str.pop_back();
            changed = true;
        }

        // enter -> save result and deactivate
        if (input.wasActionJustPressed("SubmitInput")) {
            field.isActive = false;
            field.result = str;
            changed = true;
            if (field.onChangeCallback)
                field.onChangeCallback(str);
            text.text = field.originalText;
            field.originalText.clear();
            scene->addComponent<TextDirtyTag>(text.entity);
        }
        // update the text component if changed
        else if (changed && str != text.text) {
            text.text = str;
            scene->addComponent<TextDirtyTag>(text.entity);
        }
    }

    // clear the SDL text input buffer after processing
    input.clearTextInput();

    // check if any input field is still active and toggle SDL text input
    anyFieldActive = false;
    for (auto &pack : scene->query<InputFieldComponent>()) {
        if (pack.get<InputFieldComponent&>().isActive) {
            anyFieldActive = true;
            break;
        }
    }
    input.toggleTextInput(anyFieldActive);
}

//====================================================
// CACHE MANAGEMENT
//====================================================

void UISystem::clearAllCaches()
{
    uiCache.clear();
}

//====================================================
// LIFETIME & UPDATE
//====================================================

void UISystem::init()
{
    renderWindow = OgreModule::Instance().getOgreRenderWindow();
}

void UISystem::onLoad() {}

void UISystem::update(double)
{
    processComponentCreations();
    processPanelChanges();
    processTextChanges();
    processVisibility();
    processInput();
}

void UISystem::fixedUpdate(double) {}

//====================================================
// TEST UI
//====================================================

EntityID UISystem::createTestCanvas()
{
    EntityID ent = scene->createEntity();
    PanelComponent &panel = scene->addAndGetComponent<PanelComponent>(ent);

    panel.overlayName = "testCanvas/SimpleTest";
    panel.containerName = panel.overlayName + "/Container";

    std::shared_ptr<UIElementCache> cache = createElementCache(ent, panel.overlayName, panel.containerName);
    Ogre::OverlayContainer* container = cache->container;

    panel.left = container->getLeft();
    panel.top = container->getTop();
    panel.width = container->getWidth();
    panel.height = container->getHeight();
    panel.metricsMode = OgreUtils::toHugo(container->getMetricsMode());
    panel.alignmentX = OgreUtils::toHugo(container->getHorizontalAlignment());
    panel.alignmentY = OgreUtils::toHugo(container->getVerticalAlignment());
    panel.material = "UI/Test";
    container->setMaterialName(panel.material);

    panel.isVisible = true;
    scene->addComponent<VisibilityDirtyTag>(ent);
    scene->addComponent<PanelDirtyTag>(ent);
    return ent;
}

EntityID UISystem::createButton()
{
    EntityID entity = createTestText();
    ButtonComponent& button = scene->addAndGetComponent<ButtonComponent>(entity);
    button.state = HUGO::ButtonStateTypes::BS_UP;

    // ErrorOrType<Ogre::Overlay*> overlayResult = OgreOverlayUtils::getOverlay(overlayName);
    // ErrorOrType<Ogre::OverlayContainer*> containerResult = OgreOverlayUtils::getContainer(button.containerName);
    // ErrorOrType<Ogre::OverlayElement*> elementResult = OgreOverlayUtils::getElement(button.textAreaName);

    // if (!overlayResult.isError() && !containerResult.isError()) {
    //     Ogre::Overlay* overlay = overlayResult.getValue();
    //     Ogre::OverlayContainer* container = containerResult.getValue();
    //     Ogre::OverlayElement* element = elementResult.isError() ? nullptr : elementResult.getValue();

    //     button.zOrder = overlay->getZOrder();
    //     button.metricsMode = OgreUtils::toHugo(container->getMetricsMode());

    //     uiCache[entity] = createElementCache(overlay, container, element);

    //     container->setMaterialName(button.normalMaterial);

    //     if (element) {
    //         Ogre::TextAreaOverlayElement* textArea = dynamic_cast<Ogre::TextAreaOverlayElement*>(element);
    //         if (textArea) {
    //             textArea->setCaption(text);
    //             textArea->setCharHeight(button.textSize);
    //             if (!button.fontName.empty())textArea->setFontName(button.fontName);
    //         }
    //     }

    //     scene->addComponent<VisibilityDirtyTag>(entity);
    // }

    return entity;
}

EntityID UISystem::createTestText()
{
    EntityID ent = scene->createEntity();
    PanelComponent &panel = scene->addAndGetComponent<PanelComponent>(ent);
    TextComponent &text = scene->addAndGetComponent<TextComponent>(ent);
    panel.overlayName = "testCanvas/SimpleTest";
    panel.containerName = panel.overlayName + "/Container";
    text.textAreaName = panel.containerName + "/Text";

    std::shared_ptr<UIElementCache> cache = createElementCache(ent, 
        panel.overlayName, panel.containerName, text.textAreaName);

    Ogre::TextAreaOverlayElement* area = dynamic_cast<Ogre::TextAreaOverlayElement *>(cache->element);
    if (!area)
        return 0;

    text.characterHeight = area->getCharHeight();
    text.text = area->getCaption();

    Ogre::ColourValue c = area->getColour();
    Ogre::ColourValue ct = area->getColourTop();
    Ogre::ColourValue cb = area->getColourBottom();

    text.colour = {c.r, c.g, c.b, c.a};
    text.colourTop = {ct.r, ct.g, ct.b, ct.a};
    text.colourBottom = {cb.r, cb.g, cb.b, cb.a};

    return ent;
}
<<<<<<< HEAD
=======

EntityID UISystem::createConsole()
{
    EntityID entity = createTestText();

    InputFieldComponent& inputField = scene->addAndGetComponent<InputFieldComponent>(entity);
    inputField.isActive = false;
    //inputField.onChangeCallback = [this](const std::string& input) { };
    HUGO::Input &input = HUGO::Input::Instance();
    input.registerAction("SubmitInput", SDL_KeyCode::SDLK_RETURN); // for submitting input
    input.registerAction("EraseInput", SDL_KeyCode::SDLK_BACKSPACE); // for erasing input
    return entity;
}

void UISystem::saveConsoleToFile(const std::string& input)
{
    std::ofstream file(CONSOLE_HISTORY_FILE, std::ios::app);
    if (file.is_open()) {
        file << input << std::endl;
        file.close();
    }else{
        std::cout << "No se pudo abrir el txt de la consola\n";
    }
}
>>>>>>> e9f6947 (crear barra de progreso)
