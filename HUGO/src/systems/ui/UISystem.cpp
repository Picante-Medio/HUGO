#include "UISystem.h"

#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"
#include "ConsoleComponent.h"
#include "InputFieldComponent.h"
#include "ProgressBarComponent.h"
#include "Input.h"
#include <SDL.h>

#include <algorithm>
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

void UISystem::syncProgressBar(const PanelComponent& p, const ProgressBarComponent& pr)
{
    EntityID entity = p.entity;
    std::shared_ptr<UIElementCache> cache = uiCache[entity];
    if (!cache || !cache->container || !cache->element) return;

    cache->container->setWidth(p.width);
    cache->container->setHeight(p.height);
    // calculate percentaje

    //testing
    HUGO::Float percent = (pr.currentValue - pr.minValue) / (pr.maxValue - pr.minValue);
    percent = std::clamp(percent, 0.0f, 1.0f);

    // sync fill
    switch (pr.fillMode)
    {
        case ProgressBarComponent::HORIZONTAL_LEFT_TO_RIGHT:
            cache->element->setLeft(0.0f);
            cache->element->setWidth(p.width * pr.fillWidth * percent);
            cache->element->setTop(0.0f);
            cache->element->setHeight(p.height * pr.fillHeight);
            break;

        case ProgressBarComponent::HORIZONTAL_RIGHT_TO_LEFT:
            cache->element->setLeft(p.width * pr.fillWidth * (1.0f - percent));
            cache->element->setWidth(p.width * pr.fillWidth *percent);
            cache->element->setTop(0.0f);
            cache->element->setHeight(p.height * pr.fillHeight);
            break;

        case ProgressBarComponent::VERTICAL_BOTTOM_TO_TOP:
            cache->element->setTop(p.height * pr.fillHeight * (1.0f - percent));
            cache->element->setHeight(p.height * pr.fillWidth * percent);
            cache->element->setLeft(0.0f);
            cache->element->setWidth(p.width* pr.fillWidth);
            break;

        case ProgressBarComponent::VERTICAL_TOP_TO_BOTTOM:
            cache->element->setTop(0.0f);
            cache->element->setHeight(p.height *pr.fillHeight * percent);
            cache->element->setLeft(0.0f);
            cache->element->setWidth(p.width * pr.fillWidth);
            break;
    }
    // optional: sync colores/materiales
    if (!pr.fillMaterial.empty())
        cache->element->setMaterialName(pr.fillMaterial);

    if (!p.material.empty())
        cache->container->setMaterialName(p.material);
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
                // hide all children overlay element
                for (auto child : cache->container->getChildren())
                    child.second->hide();

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
        EntityID entity = panel.entity;
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
        createElementCache(entity, overlay, container);
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

                        field.fullText.clear();
                        field.cursorPos = 0;
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
    for (auto &pack : scene->query<InputFieldComponent, TextComponent, PanelComponent>()) {
        InputFieldComponent& field = pack.get<InputFieldComponent&>();
        if (!field.isActive) continue;

        TextComponent& text = pack.get<TextComponent&>();
        PanelComponent& panel = pack.get<PanelComponent&>();
        bool changed = false;

        // append new text input from SDL
        if (!textInput.empty()) {
            int c = static_cast<int>(textInput[0]);
            //only english printable ascii codes are accepted
            if (c > 32 && c < 127){
                field.fullText.insert(field.cursorPos, 1, textInput[0]);
                field.cursorPos++;
                changed = true;
            }
        }

        // backspace -> remove last character
        if (input.wasKeyJustPressed(SDLK_BACKSPACE) && field.cursorPos > 0) {
            field.fullText.erase(field.cursorPos - 1, 1);
            field.cursorPos--;
            changed = true;
        }

        // enter -> save result and deactivate
        if (input.wasActionJustPressed("SubmitInput")) {
            field.isActive = false;
            field.result = field.fullText;

            if (field.onChangeCallback)
                field.onChangeCallback(field.fullText);

            // recover the original text
            text.text = field.originalText;
            field.originalText.clear();

            scene->addComponent<TextDirtyTag>(text.entity);
            continue;
        }
        // update visible text
        if (changed) {
            UIElementCache* cache = getCache(text.entity);
            if (!cache || !cache->element) continue;

            Ogre::TextAreaOverlayElement* area =
                dynamic_cast<Ogre::TextAreaOverlayElement*>(cache->element);
            if (!area) continue;

            const HUGO::String& full = field.fullText;
            HUGO::Float panelWidth = panel.width;

            HUGO::String visible;
            HUGO::Float width = 0.0f;

            // show only the part that fits from the end
            for (int i = static_cast<int>(full.size() - 1); i >= 0; --i) {
                char c = full[i];
                HUGO::Float w = area->getFont()->getGlyphAspectRatio(c) * text.characterHeight;

                if (width + w > panelWidth)
                    break;

                visible.insert(visible.begin(), c);
                width += w;
            }

            // optionally, add a blinking cursor
            if (field.isActive)
                visible += "|";

            text.text = visible;
            scene->addComponent<TextDirtyTag>(text.entity);
        }
    }

    // clear SDL text input buffer
    input.clearTextInput();

    // toggle SDL text input if any field is active
    anyFieldActive = false;
    for (auto& pack : scene->query<InputFieldComponent>()) {
        if (pack.get<InputFieldComponent&>().isActive) {
            anyFieldActive = true;
            break;
        }
    }
    input.toggleTextInput(anyFieldActive);
}
void UISystem::processProgressBarChanges()
{
    for (auto &pack : scene->query<PanelComponent, ProgressBarComponent, ProgressBarDirtyTag>())
    {
        PanelComponent& panel = pack.get<PanelComponent&>();
        ProgressBarComponent& pr = pack.get<ProgressBarComponent&>();
        //ProgressBarDirtyTag& tag = pack.get<ProgressBarDirtyTag &>();
        pr.currentValue = std::min(pr.maxValue, pr.currentValue + 0.1f);
        //syncText(panel, text, tag);
        syncProgressBar(panel, pr);
        //scene->removeComponent<ProgressBarDirtyTag>(tag.entity);
    }
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
    HUGO::Input::Instance().registerAction("SubmitInput", {SDLK_RETURN});
}

void UISystem::onLoad() {}

void UISystem::update(double)
{
    ErrorOrVoid e = processComponentCreations();
    if(e.isError()){
        std::cerr<<e.getError().errorMessage;
    }

    processPanelChanges();
    processTextChanges();
    processVisibility();
    processInput();
    processProgressBarChanges();
}

void UISystem::fixedUpdate(double) {}

//====================================================
// TEST UI
//====================================================

EntityID UISystem::createPanel(const HUGO::String& overlayName)
{
    EntityID ent = scene->createEntity();
    PanelComponent& panel = scene->addAndGetComponent<PanelComponent>(ent);

    // assign names
    panel.overlayName = overlayName;
    panel.containerName = overlayName + "/Container";

    // create cache
    std::shared_ptr<UIElementCache> cache = createElementCache(ent, panel.overlayName, panel.containerName);
    Ogre::OverlayContainer* container = cache->container;

    if (!container)
        return 0;

    // initialize panel from OGRE values
    panel.left   = container->getLeft();
    panel.top    = container->getTop();
    panel.width  = container->getWidth();
    panel.height = container->getHeight();

    panel.metricsMode = OgreUtils::toHugo(container->getMetricsMode());
    panel.alignmentX  = OgreUtils::toHugo(container->getHorizontalAlignment());
    panel.alignmentY  = OgreUtils::toHugo(container->getVerticalAlignment());

    panel.isVisible = true;

    // mark dirty for sync
    scene->addComponent<VisibilityDirtyTag>(ent);
    scene->addComponent<PanelDirtyTag>(ent);

    return ent;
}

EntityID UISystem::createButton()
{
    EntityID entity = 1;

    return entity;
}

EntityID UISystem::createText(const HUGO::String& overlayName)
{
    EntityID ent = createPanel(overlayName);

    PanelComponent& panel = scene->getComponent<PanelComponent>(ent);
    TextComponent& text   = scene->addAndGetComponent<TextComponent>(ent);

    text.textAreaName = panel.containerName + "/Text";

    UIElementCache* cache = getCache(ent);

    ErrorOrType<Ogre::OverlayElement*> elementResult = OgreOverlayUtils::getElement(text.textAreaName);
    if (!elementResult.isError())
        cache->element = elementResult.getValue();

    Ogre::TextAreaOverlayElement* area = dynamic_cast<Ogre::TextAreaOverlayElement*>(cache->element);
    if (!area)
        return 0;

    // initialize from OGRE
    text.text = area->getCaption();
    text.characterHeight = area->getCharHeight();

    const Ogre::ColourValue& c  = area->getColour();
    const Ogre::ColourValue& ct = area->getColourTop();
    const Ogre::ColourValue& cb = area->getColourBottom();

    text.colour       = {c.r,  c.g,  c.b,  c.a};
    text.colourTop    = {ct.r, ct.g, ct.b, ct.a};
    text.colourBottom = {cb.r, cb.g, cb.b, cb.a};

    scene->addComponent<TextDirtyTag>(ent);

    return ent;
}

EntityID UISystem::createProgressBar(const HUGO::String& overlayName)
{
    EntityID ent = scene->createEntity();

    PanelComponent& panel = scene->addAndGetComponent<PanelComponent>(ent);
    ProgressBarComponent& pr = scene->addAndGetComponent<ProgressBarComponent>(ent);

    // assign names
    panel.overlayName = overlayName;
    panel.containerName = overlayName + "/Background";
    pr.fillElementName = overlayName + "/Fill";
    pr.fillMode = ProgressBarComponent::FillMode::HORIZONTAL_LEFT_TO_RIGHT;
    // create cache
    std::shared_ptr<UIElementCache> cache = createElementCache(ent, 
        panel.overlayName, panel.containerName, pr.fillElementName);

    if (!cache || !cache->container || !cache->element)
        return 0;

    // initialize background from OGRE
    panel.left   = cache->container->getLeft();
    panel.top    = cache->container->getTop();
    panel.width  = cache->container->getWidth();
    panel.height = cache->container->getHeight();

    panel.metricsMode = OgreUtils::toHugo(cache->container->getMetricsMode());
    panel.alignmentX  = OgreUtils::toHugo(cache->container->getHorizontalAlignment());
    panel.alignmentY  = OgreUtils::toHugo(cache->container->getVerticalAlignment());

    panel.isVisible = true;

    // initialize progress bar defaults
    pr.currentValue = pr.minValue;

    // materials
    panel.material = "UI/Test";
    pr.fillMaterial = "UI/Test2";
    
    pr.fillWidth = cache->element->getWidth();
    pr.fillHeight = cache->element->getHeight();

    // mark dirty
    scene->addComponent<PanelDirtyTag>(ent);
    scene->addComponent<VisibilityDirtyTag>(ent);
    scene->addComponent<ProgressBarDirtyTag>(ent);

    return ent;
}

