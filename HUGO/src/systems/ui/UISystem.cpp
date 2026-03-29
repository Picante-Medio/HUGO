#include "UISystem.h"

#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"
#include "ConsoleComponent.h"
#include "InputFieldComponent.h"
#include "ProgressBarComponent.h"
#include "SliderComponent.h"
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

std::shared_ptr<UIElementCache> UISystem::createElementCache(EntityID entity, const HUGO::String &overlayName, const HUGO::String &containerName, const HUGO::String &elementName,
                                                            bool isTextArea)
{
    std::shared_ptr<UIElementCache> cache = std::make_shared<UIElementCache>();
    
    ErrorOrType<Ogre::Overlay*> overlayResult = HUGO::OgreOverlayUtils::getOverlay(overlayName);
    if (!overlayResult.isError())
        cache->overlay = overlayResult.getValue();
    
    ErrorOrType<Ogre::OverlayContainer*> containerResult = HUGO::OgreOverlayUtils::getContainer(containerName, cache->overlay);
    if (!containerResult.isError())
        cache->container = containerResult.getValue();
    
    if (elementName != ""){
        ErrorOrType<Ogre::OverlayElement*> elementResult = HUGO::OgreOverlayUtils::getElement(elementName, isTextArea, cache->container);
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

bool UISystem::isMouseOver(HUGO::Float left, HUGO::Float top, HUGO::Float width, HUGO::Float height, HUGO::Float mouseX, HUGO::Float mouseY)
{
    return (mouseX >= left && mouseX <= (left + width) && mouseY >= top  && mouseY <= (top+height));
}

bool UISystem::isMouseOverPanel(const PanelComponent &panel, HUGO::Float mouseX, HUGO::Float mouseY)
{
    return isMouseOver(panel.left, panel.top, panel.width, panel.height, mouseX, mouseY);
}

//====================================================
// SYNCHRONIZATION
//====================================================

void UISystem::syncPanel(const PanelComponent &p)
{
    UIElementCache *cache = getCache(p.entity);
    if (!cache || !cache->container)
        return;
    
    cache->container->setLeft(p.left);
    cache->container->setTop(p.top);
    cache->container->setWidth(p.width);
    cache->container->setHeight(p.height);
    // cache->container->setHorizontalAlignment(OgreUtils::toOgre(p.alignmentX));
    // cache->container->setVerticalAlignment(OgreUtils::toOgre(p.alignmentY));
    cache->container->setMetricsMode(HUGO::OgreUtils::toOgre(p.metricsMode));

    if (!p.material.empty()) cache->container->setMaterialName(p.material);
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

void UISystem::syncButton(const ButtonComponent &button, PanelComponent &panel)
{
    UIElementCache* cache = getCache(button.entity);
    if (!cache || !cache->container) return;

    switch (button.state) {
        case HUGO::ButtonStateTypes::BS_UP :
            panel.material = button.normalMaterial;
            break;
        case HUGO::ButtonStateTypes::BS_DOWN :
            panel.material = button.pressedMaterial;
            break;
        case HUGO::ButtonStateTypes::BS_OVER :
            panel.material = button.hoverMaterial;
            break;
        default:
            panel.material = button.normalMaterial;
            break;
        }
    scene->addComponent<PanelComponent>(button.entity);
}

void UISystem::syncProgressBar(const PanelComponent& p, const ProgressBarComponent& pr)
{
    EntityID entity = p.entity;
    std::shared_ptr<UIElementCache> cache = uiCache[entity];
    if (!cache || !cache->container || !cache->element) return;

    cache->container->setWidth(p.width);
    cache->container->setHeight(p.height);
    // calculate percentaje
    HUGO::Float percent = (pr.currentValue - pr.minValue) / (pr.maxValue - pr.minValue);
    percent = std::clamp(percent, 0.0f, 1.0f);

    // sync fill
    switch (pr.fillMode)
    {
        case HUGO::ProgressBarFillMode::HORIZONTAL_LEFT_TO_RIGHT:
            cache->element->setLeft(0.0f);
            cache->element->setWidth(p.width * pr.fillWidth * percent);
            cache->element->setTop(0.0f);
            cache->element->setHeight(p.height * pr.fillHeight);
            break;

        case HUGO::ProgressBarFillMode::HORIZONTAL_RIGHT_TO_LEFT:
            cache->element->setLeft(p.width * pr.fillWidth * (1.0f - percent));
            cache->element->setWidth(p.width * pr.fillWidth *percent);
            cache->element->setTop(0.0f);
            cache->element->setHeight(p.height * pr.fillHeight);
            break;

        case HUGO::ProgressBarFillMode::VERTICAL_BOTTOM_TO_TOP:
            cache->element->setTop(p.height * pr.fillHeight * (1.0f - percent));
            cache->element->setHeight(p.height * pr.fillWidth * percent);
            cache->element->setLeft(0.0f);
            cache->element->setWidth(p.width* pr.fillWidth);
            break;

        case HUGO::ProgressBarFillMode::VERTICAL_TOP_TO_BOTTOM:
            cache->element->setTop(0.0f);
            cache->element->setHeight(p.height *pr.fillHeight * percent);
            cache->element->setLeft(0.0f);
            cache->element->setWidth(p.width * pr.fillWidth);
            break;
    }
    // sync materials
    if (!pr.fillMaterial.empty()) cache->element->setMaterialName(pr.fillMaterial);

    if (!p.material.empty()) cache->container->setMaterialName(p.material);
}

void UISystem::syncSlider(const PanelComponent& p, const SliderComponent& s)
{
    UISliderCache* cache = dynamic_cast<UISliderCache*>(uiCache[s.entity].get());
    if (!cache || !cache->container || !cache->element || !cache->knob)
        return;

    HUGO::Float percent = (s.currentValue - s.minValue) / (s.maxValue - s.minValue);
    percent = std::clamp(percent, 0.0f, 1.0f);
    HUGO::Float knobPos;
    // sync track and knob
    switch (s.orientation)
    {
    case HUGO::SliderOrientation::HORIZONTAL:
        cache->element->setLeft(0.0f);
        cache->element->setTop(0.0f);
        cache->element->setWidth(p.width * percent);
        cache->element->setHeight(p.height);
        
        knobPos = (p.width - s.knobWidth) * percent;
        knobPos = std::clamp(knobPos, 0.0f, p.width - s.knobWidth);
        
        cache->knob->setLeft(knobPos);
        cache->knob->setTop(-s.knobWidth/4.0f);
        cache->knob->setWidth(s.knobWidth);
        cache->knob->setHeight(s.knobHeight);
        break;
    
    case HUGO::SliderOrientation::VERTICAL:
        cache->element->setLeft(0.0f);
        cache->element->setTop(p.height * (1.0f - percent));
        cache->element->setWidth(p.width);
        cache->element->setHeight(p.height * percent);
        
        knobPos = (p.height - s.knobHeight) * (1.0f - percent);
        knobPos = std::clamp(knobPos, 0.0f, p.height - s.knobHeight);
        
        cache->knob->setLeft(-s.knobHeight/4.0f);
        cache->knob->setTop(knobPos);
        cache->knob->setWidth(s.knobWidth);
        cache->knob->setHeight(s.knobHeight);
        break;
    }
    
    // apply materials or colors
    if(!s.trackMaterial.empty()) cache->element->setMaterialName(s.trackMaterial);
    if(!s.knobMaterial.empty()) cache->knob->setMaterialName(s.knobMaterial);
}

//====================================================
// PROCESSING
//====================================================
void UISystem::processPanelChanges()
{
   for (auto &pack : scene->query<PanelComponent, PanelDirtyTag>())
    {
        PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
        syncPanel(panel);
        scene->removeComponent<PanelDirtyTag>(panel.entity);
    }
}

void UISystem::processVisibility()
{
    for (auto &pack : scene->query<PanelComponent, VisibilityDirtyTag>())
    {
        PanelComponent &panel = pack.getDeprecated<PanelComponent &>();
        UIElementCache *cache = getCache(panel.entity);

        if (cache && cache->overlay && cache->container)
        {
            cache->setVisible(panel.isVisible);
        }

        scene->removeComponent<VisibilityDirtyTag>(panel.entity);
    }
}


ErrorOrVoid UISystem::processComponentCreations()
{
    //panels
    for (auto &pack : scene->query<PanelComponent, CreatePanelDirtyTag>())
    {
        PanelComponent &panel = pack.getDeprecated<PanelComponent &>();
        CreatePanelDirtyTag &tag = pack.getDeprecated<CreatePanelDirtyTag &>();
        EntityID entity = panel.entity;
        createElementCache(entity, panel.overlayName, panel.containerName);
        scene->addComponent<VisibilityDirtyTag>(panel.entity);
        scene->addComponent<PanelDirtyTag>(entity);
        scene->removeComponent<CreatePanelDirtyTag>(entity);
    }
    
    
    for (auto &pack : scene->query<CreateTextDirtyTag, TextComponent, PanelComponent>())
    {
        TextComponent &text = pack.getDeprecated<TextComponent &>();
        CreateTextDirtyTag &tag = pack.getDeprecated<CreateTextDirtyTag &>();

        ErrorOrType<Ogre::OverlayElement *> elementResult = HUGO::OgreOverlayUtils::getElement(text.textAreaName, true);
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

    for (auto &pack : scene->query<CreateButtonDirtyTag, ButtonComponent, PanelComponent>())
    {
        ButtonComponent &button = pack.getDeprecated<ButtonComponent &>();
        PanelComponent &panel = pack.getDeprecated<PanelComponent &>();
        CreateButtonDirtyTag &tag = pack.getDeprecated<CreateButtonDirtyTag &>();
        switch (button.state)
        {
        case HUGO::ButtonStateTypes::BS_UP :
            panel.material = button.normalMaterial;
            break;
        case HUGO::ButtonStateTypes::BS_DOWN :
            panel.material = button.pressedMaterial;
            break;
        case HUGO::ButtonStateTypes::BS_OVER :
            panel.material = button.hoverMaterial;
            break;
        default:
            panel.material = button.normalMaterial;
            break;
        }

        scene->addComponent<PanelDirtyTag>(button.entity);
        scene->removeComponent<CreateButtonDirtyTag>(tag.entity);
    }
    return ErrorOrVoid();
}

void UISystem::processButtonChanges()
{
    for (auto &pack : scene->query<ButtonComponent, ButtonDirtyTag, PanelComponent>())
    {
        ButtonComponent &button = pack.getDeprecated<ButtonComponent&>();
        PanelComponent &panel = pack.getDeprecated<PanelComponent&>();
        syncButton(button, panel);
        scene->removeComponent<ButtonDirtyTag>(button.entity);
    }
}

void UISystem::processTextChanges()
{
    for (auto &pack : scene->query<PanelComponent, TextComponent, TextDirtyTag>())
    {
        TextComponent &text = pack.getDeprecated<TextComponent &>();
        TextDirtyTag &tag = pack.getDeprecated<TextDirtyTag &>();
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
            PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
            TextComponent& text = pack.getDeprecated<TextComponent&>();
            InputFieldComponent& field = pack.getDeprecated<InputFieldComponent&>();

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

        for (auto& pack : scene->query<PanelComponent, ButtonComponent>())
        {
            PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
            ButtonComponent& button = pack.getDeprecated<ButtonComponent&>();

            HUGO::Input::MouseRelativePosition mouse = input.getMouseRelativePosition();
            if (isMouseOverPanel(panel, mouse.x, mouse.y))
            {
                button.state = HUGO::BS_DOWN;
                scene->addComponent<ButtonDirtyTag>(button.entity);
                break;
            }
        }
    } else if (input.wasMouseButtonJustReleased(SDL_BUTTON_LEFT))
    {
    
        for (auto& pack : scene->query<PanelComponent, ButtonComponent>())
        {
            std::cout << "aaaaa\n";
            PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
            ButtonComponent& button = pack.getDeprecated<ButtonComponent&>();

            HUGO::Input::MouseRelativePosition mouse = input.getMouseRelativePosition();
            bool isOver = isMouseOverPanel(panel, mouse.x, mouse.y);

            if (isOver){
                if (button.onClickCallback) button.onClickCallback();
                button.state = HUGO::BS_OVER;
            }
            else 
                button.state = HUGO::BS_UP;
            
            scene->addComponent<ButtonDirtyTag>(panel.entity);
        }
    }

    for (auto& pack : scene->query<PanelComponent, ButtonComponent>())
        {
            PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
            ButtonComponent& button = pack.getDeprecated<ButtonComponent&>();

            HUGO::Input::MouseRelativePosition mouse = input.getMouseRelativePosition();
            bool isOver = isMouseOverPanel(panel, mouse.x, mouse.y);

            if (isOver && button.state != HUGO::BS_DOWN)
            {
                button.state = HUGO::BS_OVER;
                scene->addComponent<ButtonDirtyTag>(button.entity);
            }
            else if (!isOver && button.state != HUGO::BS_UP && button.state != HUGO::BS_DOWN)
            {
                button.state = HUGO::BS_UP;
                scene->addComponent<ButtonDirtyTag>(button.entity);
            }
        }

    // process text for the active input field using SDL text buffer
    const HUGO::String& textInput = input.getTextInput();
    for (auto &pack : scene->query<InputFieldComponent, TextComponent, PanelComponent>()) {
        InputFieldComponent& field = pack.getDeprecated<InputFieldComponent&>();
        if (!field.isActive) continue;

        TextComponent& text = pack.getDeprecated<TextComponent&>();
        PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
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
            text.text = visible;
            scene->addComponent<TextDirtyTag>(text.entity);
        }
    }

    // clear SDL text input buffer
    input.clearTextInput();

    // toggle SDL text input if any field is active
    anyFieldActive = false;
    for (auto& pack : scene->query<InputFieldComponent>()) {
        if (pack.getDeprecated<InputFieldComponent&>().isActive) {
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
        PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
        ProgressBarComponent& pr = pack.getDeprecated<ProgressBarComponent&>();
        //ProgressBarDirtyTag& tag = pack.get<ProgressBarDirtyTag &>();
        pr.currentValue = std::min(pr.maxValue, pr.currentValue + 0.1f);
        //syncText(panel, text, tag);
        syncProgressBar(panel, pr);
        //scene->removeComponent<ProgressBarDirtyTag>(tag.entity);
    }
}

void UISystem::processSliderInput()
{
    HUGO::Input& input = HUGO::Input::Instance();
    const HUGO::Input::MouseRelativePosition& mouse = input.getMouseRelativePosition();

    for (auto &pack : scene->query<SliderComponent, PanelComponent>())
    {
        SliderComponent& slider = pack.getDeprecated<SliderComponent&>();
        PanelComponent& panel   = pack.getDeprecated<PanelComponent&>();

        UISliderCache* cache = dynamic_cast<UISliderCache*>(uiCache[slider.entity].get());
        if (!cache || !cache->container || !cache->element || !cache->knob)
            continue;
        
        // get knob position and size
        HUGO::Float knobLeft = cache->knob->getLeft();
        HUGO::Float knobTop = cache->knob->getTop();
        bool mouseOverKnob = isMouseOver(knobLeft, knobTop, slider.knobWidth, slider.knobHeight, mouse.x, mouse.y);
        bool mouseOverTrack = isMouseOverPanel(panel, mouse.x, mouse.y);

        // check for initial click to start dragging
        if ((mouseOverKnob || mouseOverTrack) && input.wasMouseButtonJustPressed(SDL_BUTTON_LEFT))
        {
            slider.isDragging = true;
            
            // if clicking directly on the track, move knob to clicked position
            if (mouseOverTrack && !mouseOverKnob)
            {
                HUGO::Float percent = 0.0f;
                HUGO::Float trackHeight = panel.height - slider.knobHeight;                    
                HUGO::Float trackWidth = panel.width - slider.knobWidth;
                switch (slider.orientation)
                {
                case HUGO::SliderOrientation::HORIZONTAL:
                    percent = (mouse.x - panel.left) / trackWidth;
                    break;
                
                case HUGO::SliderOrientation::VERTICAL:
                    percent = 1.0f - (mouse.y - panel.top) / trackHeight;
                    break;
                }

                percent = std::clamp(percent, 0.0f, 1.0f);
                slider.currentValue = slider.minValue + percent * (slider.maxValue - slider.minValue);
            }
        }
        
        // handle active dragging
        if (slider.isDragging)
        {
            if (input.wasMouseButtonJustPressed(SDL_BUTTON_LEFT))
            {
                HUGO::Float percent = 0.0f;
                
                // value based on mouse position
                HUGO::Float trackWidth = panel.width - slider.knobWidth;
                HUGO::Float trackHeight = panel.height - slider.knobHeight;
                switch (slider.orientation)
                {
                    case HUGO::SliderOrientation::HORIZONTAL:
                        percent = (mouse.x - panel.left) / trackWidth;
                        percent = std::clamp(percent, 0.0f, 1.0f);
                        break;
                    
                    case HUGO::SliderOrientation::VERTICAL:
                        percent = 1.0f - (mouse.y - panel.top) / trackHeight;
                        percent = std::clamp(percent, 0.0f, 1.0f);
                        break;
                }
                
                HUGO::Float newValue = slider.minValue + percent * (slider.maxValue - slider.minValue);
                
                // only update if it is needed
                if (slider.currentValue != newValue)
                    slider.currentValue = newValue;
            }
            else
            {
                slider.isDragging = false;
            }
        }
    }
}
void UISystem::processSliderChanges()
{
   for (auto &pack : scene->query<PanelComponent, SliderComponent, SliderDirtyTag>())
    {
        PanelComponent& panel = pack.getDeprecated<PanelComponent&>();
        SliderComponent& slider = pack.getDeprecated<SliderComponent&>();
        syncSlider(panel, slider);
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

    processInput();
    processProgressBarChanges();
    processSliderInput();
    processSliderChanges();
    processPanelChanges();
    processTextChanges();
    processVisibility();
    processButtonChanges();
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

    panel.metricsMode = HUGO::OgreUtils::toHugo(container->getMetricsMode());
    // panel.alignmentX  = HUGO::OgreUtils::toHugo(container->getHorizontalAlignment());
    // panel.alignmentY  = HUGO::OgreUtils::toHugo(container->getVerticalAlignment());

    panel.isVisible = true;
    panel.material = "test/UI";
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

    ErrorOrType<Ogre::OverlayElement*> elementResult = HUGO::OgreOverlayUtils::getElement(text.textAreaName);
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
    pr.fillMode = HUGO::ProgressBarFillMode::HORIZONTAL_LEFT_TO_RIGHT;
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

    panel.metricsMode = HUGO::OgreUtils::toHugo(cache->container->getMetricsMode());
    // panel.alignmentX  = HUGO::OgreUtils::toHugo(cache->container->getHorizontalAlignment());
    // panel.alignmentY  = HUGO::OgreUtils::toHugo(cache->container->getVerticalAlignment());

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

EntityID UISystem::createSlider(const HUGO::String& overlayName)
{
    EntityID ent = scene->createEntity();

    PanelComponent& panel   = scene->addAndGetComponent<PanelComponent>(ent);
    SliderComponent& slider = scene->addAndGetComponent<SliderComponent>(ent);

    // names
    panel.overlayName = overlayName;
    panel.containerName = overlayName + "/Background"; // container
    slider.trackElementName = overlayName + "/Track"; // track
    slider.knobElementName = overlayName + "/Knob"; // knob

    slider.orientation = HUGO::SliderOrientation::HORIZONTAL;

    // create slider cache
    std::shared_ptr<UISliderCache> cache = std::make_shared<UISliderCache>();
    uiCache[ent] = cache;

    // get overlay and container (background)
    ErrorOrType<Ogre::Overlay*> overlayResult = HUGO::OgreOverlayUtils::getOverlay(panel.overlayName);
    if (!overlayResult.isError()) cache->overlay = overlayResult.getValue();

    ErrorOrType<Ogre::OverlayContainer*> containerResult = HUGO::OgreOverlayUtils::getContainer(panel.containerName, cache->overlay);
    if (!containerResult.isError()) cache->container = containerResult.getValue();

    // get track
    ErrorOrType<Ogre::OverlayElement*> trackResult = HUGO::OgreOverlayUtils::getElement(slider.trackElementName);
    if (!trackResult.isError()) cache->element = trackResult.getValue();
    // get knob
    ErrorOrType<Ogre::OverlayContainer*> knobContainerResult = HUGO::OgreOverlayUtils::getContainer(slider.knobElementName, cache->overlay);
    if (!knobContainerResult.isError()) cache->knob = knobContainerResult.getValue();

    if (!cache->container || !cache->element || !cache->knob) return 0;

    // initialize size and position of the background
    panel.left = cache->container->getLeft();
    panel.top = cache->container->getTop();
    panel.width = cache->container->getWidth();
    panel.height = cache->container->getHeight();
    panel.metricsMode = HUGO::OgreUtils::toHugo(cache->container->getMetricsMode());
    // panel.alignmentX  = HUGO::OgreUtils::toHugo(cache->container->getHorizontalAlignment());
    // panel.alignmentY  = HUGO::OgreUtils::toHugo(cache->container->getVerticalAlignment());

    panel.isVisible = true;
    
    panel.material = "UI/Background";
    
    // initialize slider 
    slider.knobWidth = cache->knob->getWidth() * panel.width;
    slider.knobHeight = cache->knob->getHeight() * panel.height;
    
    slider.currentValue = slider.minValue;
    
    // materials and colors
    //to use colors, the material fields must be blank.
    slider.trackMaterial = "UI/Track";
    slider.knobMaterial = "UI/Knob"; 

    // dirty tags
    scene->addComponent<PanelDirtyTag>(ent);
    scene->addComponent<VisibilityDirtyTag>(ent);
    scene->addComponent<SliderDirtyTag>(ent);

    return ent;
}
