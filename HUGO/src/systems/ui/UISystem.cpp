#include "UISystem.h"

#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"
#include "InputFieldComponent.h"
#include "ProgressBarComponent.h"
#include "SliderComponent.h"
#include <SDL.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include "OgreModule.h"
#include "OgreUtils.h"

#include "OgreTextAreaOverlayElement.h"
#include "OgreOverlaySystem.h"
#include "Input.h"

//====================================================
// DESTRUCTOR
//====================================================

UISystem::~UISystem()
{
    clearAllCaches();
    OgreModule::instance().getCurrentSceneManager()->removeRenderQueueListener(Ogre::OverlaySystem::getSingletonPtr());
}
//====================================================
// CACHE CREATION
//====================================================
std::shared_ptr<UIElementCache> UISystem::createElementCache(
    EntityID entity,
    const HUGO::String& overlayName,
    const HUGO::String& containerName)
{
    // already exists
    auto it = uiCache.find(entity);
    if (it != uiCache.end())
        return it->second;

    std::shared_ptr<UIElementCache> cache = std::make_shared<UIElementCache>();

    // overlay
    HUGO::ErrorOrType<Ogre::Overlay*> overlayRes = HUGO::OgreOverlayUtils::getOverlay(overlayName);
    if (!overlayRes.isError())
        cache->overlay = overlayRes.getValue();

    // container (ROOT UI ELEMENT)
    HUGO::ErrorOrType<Ogre::OverlayContainer*> containerRes = HUGO::OgreOverlayUtils::getContainer(containerName, cache->overlay);
    if (!containerRes.isError())
        cache->container = containerRes.getValue();

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
    int screenPixelsW, screenPixelsH;
    SDL_GL_GetDrawableSize(OgreModule::instance().getSDLWindow(), &screenPixelsW, &screenPixelsH);
    
    float mouseXPixels = mouseX * screenPixelsW;
    float mouseYPixels = mouseY * screenPixelsH;
    
    HUGO::Rect screenRect;
    
    switch (panel.metricsMode)
    {
        case HUGO::UIMetricsMode::GMM_RELATIVE:
        {
            screenRect.left = panel.left * screenPixelsW;
            screenRect.top = panel.top * screenPixelsH;
            screenRect.width = panel.width * screenPixelsW;
            screenRect.height = panel.height * screenPixelsH;
            break;
        }
            
        case HUGO::UIMetricsMode::GMM_PIXELS:
        {
            screenRect.left = panel.left;
            screenRect.top = panel.top;
            screenRect.width = panel.width;
            screenRect.height = panel.height;
            break;
        }
            
        case HUGO::UIMetricsMode::GMM_RELATIVE_ASPECT_ADJUSTED:
        {
            HUGO::Float baseSizePixels = std::min((float)screenPixelsW, (float)screenPixelsH);
            screenRect.left = panel.left * baseSizePixels;
            screenRect.top = panel.top * baseSizePixels;
            screenRect.width = panel.width * baseSizePixels;
            screenRect.height = panel.height * baseSizePixels;
            
            screenRect.left += (screenPixelsW - baseSizePixels) * 0.5f;
            screenRect.top += (screenPixelsH - baseSizePixels) * 0.5f;
            break;
        }
    }
    
    return isMouseOver(screenRect.left, screenRect.top, screenRect.width, screenRect.height, mouseXPixels, mouseYPixels);

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
    cache->setVisible(p.isVisible);
    cache->overlay->setZOrder(static_cast<Ogre::ushort>(p.zOrder));

    if (!p.material.empty()) cache->container->setMaterialName(p.material);
}

HUGO::String UISystem::formatText(const HUGO::String & text, Ogre::TextAreaOverlayElement *textArea, HUGO::Float maxWidth,HUGO::Float maxHeight)
{
    if (!textArea || !textArea->getFont()  ||!textArea->getFont()->isLoaded() || text.empty())
        return text;

    float screenWidth = static_cast<float>(renderWindow->getWidth());
    float screenHeight = static_cast<float>(renderWindow->getHeight());

    HUGO::Float height = (textArea->getMetricsMode() == Ogre::GMM_RELATIVE)? textArea->getCharHeight() * screenHeight : textArea->getCharHeight();
    HUGO::Float maxH = maxHeight * screenHeight;
    HUGO::Float maxW = maxWidth * screenWidth;
    int maxLines = static_cast<int>(maxH / height);   
    
    std::deque<HUGO::String> lines;
    HUGO::String currentLine;
    HUGO::Float currentWidth = 0.0f;

    for(char c : text){
        if (c == '\n')
        {
            lines.push_back(currentLine);
            currentLine.clear();
            currentWidth = 0.0f;
            continue;
        }
        HUGO::Float charWidth = textArea->getFont()->getGlyphAspectRatio(c) * height;
        if (currentWidth + charWidth > maxW)
        {
            lines.push_back(currentLine);
            currentLine.clear();
            currentWidth = 0.0f;
        }

        currentLine += c;
        currentWidth += charWidth;
    }

    if(!currentLine.empty())
        lines.push_back(currentLine);

    while(lines.size() > maxLines)
        lines.pop_front();

    HUGO::String result;
    for (const HUGO::String &l : lines)
        result += l + "\n";
    return result;
}

bool UISystem::isValidColour(const HUGO::Vector4& colour){
    return !(colour.x == -1.0f && colour.y == -1.0f && colour.z == -1.0f && colour.w == -1.0f);
}

void UISystem::syncText(const TextComponent &t)
{
    UIElementCache* cache = getCache(t.entity);
    Ogre::TextAreaOverlayElement *textArea = dynamic_cast<Ogre::TextAreaOverlayElement *>(cache->element);
    if (!textArea)
        return;
   
    textArea->setCaption(t.text);

    textArea->setCharHeight(t.characterHeight);
    if(isValidColour(t.colour)){
        textArea->setColour(Ogre::ColourValue(
            t.colour.x,
            t.colour.y,
            t.colour.z,
            t.colour.w));
    }
    if(isValidColour(t.colourTop)){
        textArea->setColourTop(Ogre::ColourValue(
            t.colourTop.x,
            t.colourTop.y,
            t.colourTop.z,
            t.colourTop.w));
    }
    if(isValidColour(t.colourBottom)){
        textArea->setColourBottom(Ogre::ColourValue(
            t.colourBottom.x,
            t.colourBottom.y,
            t.colourBottom.z,
            t.colourBottom.w));
    }
    textArea->setTop(t.top);
    textArea->setFontName(t.textFont);
    textArea->setSpaceWidth(t.spaceWidth);
    textArea->setCharHeight(t.characterHeight);
    textArea->setLeft(t.left);

}

void UISystem::syncButton(const ButtonComponent &button)
{
    UIElementCache* cache = getCache(button.entity);
    if (!cache || !cache->container) return;

    HUGO::String change ="";

    switch (button.state) {
        case HUGO::ButtonStateTypes::BS_UP :
            change = button.normalMaterial;
            break;
        case HUGO::ButtonStateTypes::BS_DOWN :
            change = button.pressedMaterial;
            break;
        case HUGO::ButtonStateTypes::BS_OVER :
            change = button.hoverMaterial;
            break;
        default:
            change = button.normalMaterial;
            break;
    }

    scene.modifyComponent<PanelComponent>(button.entity, [change](PanelComponent& p){
        p.material = change;
    });
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

void UISystem::syncSlider(const PanelComponent& p, SliderComponent& s)
{
    UIElementCache* cache = uiCache[s.entity].get();
    if (!cache || !cache->container || !cache->element || !cache->extra)
        return;

    if (s.currentValue != s.previousValue)
    {
        if (s.onChangeCallback)
            s.onChangeCallback(s.currentValue);

        s.previousValue = s.currentValue;
    }

    HUGO::Float percent = (s.currentValue - s.minValue) / (s.maxValue - s.minValue);
    percent = std::clamp(percent, 0.0f, 1.0f);
    HUGO::Float knobPos;
    HUGO::Float centerPos;
    // sync track and knob
    switch (s.orientation)
    {
    case HUGO::SliderOrientation::HORIZONTAL:
        cache->element->setLeft(0.0f);
        cache->element->setTop(0.0f);
        cache->element->setWidth(p.width * percent);
        cache->element->setHeight(p.height);
        
        // get the center position
        centerPos = p.width * percent;

        // 2. clamp del centro
        centerPos = std::clamp(centerPos, 0.0f, p.width);

        // 3. convertir a left (borde)
        knobPos = centerPos - s.knobWidth * 0.25f;

        // 4. aplicar offset visual
        cache->extra->setLeft(knobPos + s.knobLeft);
        cache->extra->setTop(s.knobTop);
        cache->extra->setWidth(s.knobWidth);
        cache->extra->setHeight(s.knobHeight);
        break;
    
    case HUGO::SliderOrientation::VERTICAL:
        cache->element->setLeft(0.0f);
        cache->element->setTop(p.height * (1.0f - percent));
        cache->element->setWidth(p.width);
        cache->element->setHeight(p.height * percent);
        
        knobPos = (p.height - s.knobHeight) * (1.0f - percent);
        knobPos = std::clamp(knobPos, 0.0f, p.height - s.knobHeight);
        
        cache->extra->setLeft(-s.knobHeight/2.0f);
        cache->extra->setTop(knobPos);
        cache->extra->setWidth(s.knobWidth);
        cache->extra->setHeight(s.knobHeight);
        break;
    }
    
    // apply materials or colors
    if(!s.trackMaterial.empty()) cache->element->setMaterialName(s.trackMaterial);
    if(!s.knobMaterial.empty()) cache->extra->setMaterialName(s.knobMaterial);
}

//====================================================
// PROCESSING
//====================================================
void UISystem::onUpdatePanelComponent(PanelComponent& panel)
{
   syncPanel(panel);
}

void UISystem::onUpdateButtonComponent(ButtonComponent& b)
{
    syncButton(b);
}

void UISystem::onUpdateTextComponent(TextComponent& t)
{
    Ogre::TextAreaOverlayElement* area = static_cast<Ogre::TextAreaOverlayElement*>(HUGO::OgreOverlayUtils::getElement(t.textAreaName).getValue());

    if(t.maxWidth || t.maxHeight){
        t.text = formatText(t.text, area, t.maxWidth, t.maxHeight);
    }
    syncText(t);
}

constexpr HUGO::Int MIN_ACSII = 32;
constexpr HUGO::Int MAX_ACSII = 126;  

void UISystem::processInput()
{
    HUGO::Input& input = static_cast<HUGO::Input&>(scene.getInput());
    
    // get normalized mouse position
    const HUGO::IInput::MouseRelativePosition& mouse = input.getMouseRelativePosition();

    bool anyFieldActive = false; // flag to control SDL text input
    // handle mouse click: activate/deactivate input fields
    if (input.wasMouseButtonJustPressed(HUGO::MouseCode::MOUSE_LEFT)) {
        for (auto& pack : scene.query<PanelComponent, TextComponent, InputFieldComponent>()) {
            const PanelComponent& panel = pack.getConst<PanelComponent&>();
            const InputFieldComponent& field = pack.getConst<InputFieldComponent&>();

            if (isMouseOverPanel(panel, mouse.x, mouse.y)) {
                if (!field.isActive){
                    // activate this field
                    pack.modify<TextComponent>([field](TextComponent& t){
                        t.text.clear();
                        t.text= field.originalText;
                    });
                    pack.modify<InputFieldComponent>([](InputFieldComponent& f){
                        f.isActive = true;
                        f.fullText.clear();
                        f.cursorPos = 0;
                    });
                    anyFieldActive = true;
                }
            }
            else {
                // deactivate this field
                pack.modify<InputFieldComponent>([](InputFieldComponent& f){
                    f.isActive = false;
                });
                if (field.fullText.empty()) {
                    pack.modify<TextComponent>([field](TextComponent& t){
                        t.text = field.deactiveText;
                    });
                }
            }
        }
        input.toggleTextInput(anyFieldActive);    

        for (auto& pack : scene.query<PanelComponent, ButtonComponent>())
        {
            const PanelComponent& panel = pack.getConst<PanelComponent&>();
            if (isMouseOverPanel(panel, mouse.x, mouse.y))
            {
                pack.modify<ButtonComponent>([](ButtonComponent& b){
                    b.state = HUGO::BS_DOWN;
                });
                break;
            }
        }
    } else if (input.wasMouseButtonJustReleased(HUGO::MouseCode::MOUSE_LEFT))
    {
    
        for (auto& pack : scene.query<PanelComponent, ButtonComponent>())
        {
            const PanelComponent& panel = pack.getConst<PanelComponent&>();
            bool isOver = isMouseOverPanel(panel, mouse.x, mouse.y);

            pack.modify<ButtonComponent>([isOver](ButtonComponent& b){
                b.state = isOver? HUGO::BS_OVER : HUGO::BS_UP;
                if (isOver)
                    if (b.onClickCallback) b.onClickCallback();
            });
        }
    }


    for (auto& pack : scene.query<PanelComponent, ButtonComponent>())
    {
        const PanelComponent& panel = pack.getConst<PanelComponent&>();
        const ButtonComponent& button = pack.getConst<ButtonComponent&>();

        bool isOver = isMouseOverPanel(panel, mouse.x, mouse.y);
        if (isOver && button.state != HUGO::BS_DOWN)
        {
            pack.modify<ButtonComponent>([](ButtonComponent& b){
                b.state = HUGO::BS_OVER;
            });
        }
        else if (!isOver && button.state != HUGO::BS_UP && button.state != HUGO::BS_DOWN)
        {
            pack.modify<ButtonComponent>([](ButtonComponent& b){
                b.state = HUGO::BS_UP;
            });
        }
    }

    for (auto &pack : scene.query<SliderComponent, PanelComponent>())
    {
        const SliderComponent& slider = pack.getConst<SliderComponent&>();
        const PanelComponent& panel = pack.getConst<PanelComponent&>();

        UIElementCache* cache = uiCache[slider.entity].get();
        if (!cache || !cache->container || !cache->element || !cache->extra)
            continue;
        
        // get knob position and size
        HUGO::Float knobLeft = cache->extra->getLeft();
        HUGO::Float knobTop = cache->extra->getTop();
        bool mouseOverKnob = isMouseOver(knobLeft, knobTop, slider.knobWidth, slider.knobHeight, mouse.x, mouse.y);
        bool mouseOverTrack = isMouseOverPanel(panel, mouse.x, mouse.y);

        // check for initial click to start dragging
        if ((mouseOverKnob || mouseOverTrack) && input.wasMouseButtonJustPressed(HUGO::MouseCode::MOUSE_LEFT))
        {
            pack.modify<SliderComponent>([](SliderComponent& s){
                s.isDragging = true;
            });
            
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
                pack.modify<SliderComponent>([percent](SliderComponent& s){
                    s.currentValue = s.minValue + percent * (s.maxValue - s.minValue);
                });
            }
        }
        
        // handle active dragging
        if (slider.isDragging)
        {
            if (input.isMouseButtonPressed(HUGO::MouseCode::MOUSE_LEFT))
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
                if (slider.currentValue != newValue){
                    pack.modify<SliderComponent>([newValue](SliderComponent& s){
                        s.currentValue = newValue;
                    });
                }
            }
            else
            {
                pack.modify<SliderComponent>([](SliderComponent& s){
                    s.isDragging = false;
                });
            }
        }
    }

    // process text for the active input field using SDL text buffer
    const HUGO::String& textInput = input.getTextInput();
    for (auto &pack : scene.query<InputFieldComponent, TextComponent, PanelComponent>()) {
        InputFieldComponent& field = pack.getConst<InputFieldComponent&>();
        if (!field.isActive) continue;

        const TextComponent& text = pack.getConst<TextComponent&>();
        const PanelComponent& panel = pack.getConst<PanelComponent&>();
        bool changed = false;

        // append new text input from SDL
        if (!textInput.empty()) {
            int c = static_cast<int>(textInput[0]);
            //only english printable ascii codes are accepted
            if (c >= MIN_ACSII && c <= MAX_ACSII){
                pack.modify<InputFieldComponent>([textInput](InputFieldComponent& f){
                    f.fullText.insert(f.cursorPos, 1, textInput[0]);
                    f.cursorPos++;
                });
                changed = true;
            }
        }

        // backspace -> remove last character
        if (input.wasConsoleKeyJustPressed(HUGO::KeyCode::BACKSPACE) && field.cursorPos > 0) {
            pack.modify<InputFieldComponent>([textInput](InputFieldComponent& f){
                f.fullText.erase(f.cursorPos - 1, 1);
                f.cursorPos--;
            });
            changed = true;
        }

        // enter -> save result and deactivate
        if (input.wasConsoleKeyJustPressed(HUGO::KeyCode::RETURN)) {
            pack.modify<TextComponent>([field](TextComponent& t){
                // recover the original text
                t.text = field.originalText;
            });

            pack.modify<InputFieldComponent>([](InputFieldComponent& f){
                f.result = f.fullText;
                if (f.onChangeCallback)
                    f.onChangeCallback(f.fullText);
                
                f.fullText.clear();
                f.cursorPos = 0;
            });

            continue;
        }
        // update visible text
        if (changed) {
            UIElementCache* cache = getCache(text.entity);
            if (!cache || !cache->element) continue;

            Ogre::TextAreaOverlayElement* area = dynamic_cast<Ogre::TextAreaOverlayElement*>(cache->element);
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
            pack.modify<TextComponent>([visible](TextComponent& t){
                t.text = visible;
            });
        }
    }
    
    // clear SDL text input buffer
    input.clearTextInput();

    // toggle SDL text input if any field is active
    anyFieldActive = false;
    for (auto& pack : scene.query<InputFieldComponent>()) {
        if (pack.getConst<InputFieldComponent&>().isActive) {
            anyFieldActive = true;
            break;
        }
    }
    input.toggleTextInput(anyFieldActive);
}

void UISystem::onUpdateProgressBarComponent(ProgressBarComponent& pb)
{
    scene.modifyComponent<PanelComponent>(pb.entity, [this, &pb](PanelComponent& p){
        syncProgressBar(p, pb);
    });
}

void UISystem::onUpdateSliderComponent(SliderComponent& s)
{
    scene.modifyComponent<PanelComponent>(s.entity, [this, &s](PanelComponent& p){
        syncSlider(p, s);
    });
}
//====================================================
// CACHE MANAGEMENT
//====================================================
void UISystem::hideCache(UIElementCache* cache, Ogre::OverlayManager& mng)
{
    if (!cache) return;

    if (cache->element)
    {
        mng.destroyOverlayElement(cache->element);
        cache->element = nullptr;
    }

    if (cache->extra) // knob or opcional
    {
        mng.destroyOverlayElement(cache->extra);
        cache->extra = nullptr;
    }

    if (cache->container)
    {
        mng.destroyOverlayElement(cache->container);
        cache->container = nullptr;
    }

    if (cache->overlay)
    {
        mng.destroy(cache->overlay);
        cache->overlay = nullptr;
    }
}

void UISystem::clearAllCaches()
{
    auto& overlayMng = Ogre::OverlayManager::getSingleton();
    for (auto ptr: uiCache){
        hideCache(ptr.second.get(), overlayMng);
    }
    uiCache.clear();


}


//====================================================
// LIFETIME & UPDATE
//====================================================

void UISystem::init()
{
    renderWindow = OgreModule::instance().getOgreRenderWindow();
    OgreModule::instance().getCurrentSceneManager()->addRenderQueueListener(Ogre::OverlaySystem::getSingletonPtr());

    //component events
    scene.onAddComponent<PanelComponent>().connect(this, &UISystem::onAddPanelComponent);
    scene.onAddComponent<TextComponent>().connect(this, &UISystem::onAddTextComponent);
    scene.onAddComponent<ButtonComponent>().connect(this, &UISystem::onUpdateButtonComponent);
    scene.onAddComponent<ProgressBarComponent>().connect(this, &UISystem::onAddProgressBarComponent);
    scene.onAddComponent<SliderComponent>().connect(this, &UISystem::onAddSliderComponent);

    scene.onUpdateComponent<PanelComponent>().connect(this, &UISystem::onUpdatePanelComponent);
    scene.onUpdateComponent<TextComponent>().connect(this, &UISystem::onUpdateTextComponent);
    scene.onUpdateComponent<ButtonComponent>().connect(this, &UISystem::onUpdateButtonComponent);
    scene.onUpdateComponent<ProgressBarComponent>().connect(this, &UISystem::onUpdateProgressBarComponent);
    scene.onUpdateComponent<SliderComponent>().connect(this, &UISystem::onUpdateSliderComponent);
}

void UISystem::onLoad() {
    for (auto& pack: scene.query<PanelComponent>()){
        
        syncPanel(pack.getConst<PanelComponent>());
    }
}
void UISystem::onExit(){
    for (auto ptr: uiCache){
        ptr.second->setVisible(false);
    }
}
void UISystem::update(uint32_t)
{
    processInput();
}

void UISystem::fixedUpdate(uint32_t) {}

void UISystem::onAddPanelComponent(PanelComponent &p)
{
    createElementCache(p.entity, p.overlayName, p.containerName);
    syncPanel(p);
}

void UISystem::onAddTextComponent(TextComponent &t)
{
    UIElementCache *cache = getCache(t.entity);
    if (!cache) 
        return;
    
    if (t.textAreaName.empty()){
        const PanelComponent& p = scene.getComponentConst<PanelComponent>(t.entity);
        t.textAreaName = p.containerName +"/Text";
    }

    HUGO::ErrorOrType<Ogre::OverlayElement *> elementResult = HUGO::OgreOverlayUtils::getElement(t.textAreaName, true, cache->container);
    if (elementResult.isError())
        return;
    Ogre::TextAreaOverlayElement *area = dynamic_cast<Ogre::TextAreaOverlayElement *>(elementResult.getValue());
    if (!area)
        return;
    
    cache->element = area;
    onUpdateTextComponent(t);
}

void UISystem::onAddProgressBarComponent(ProgressBarComponent &pb)
{
    UIElementCache *cache = getCache(pb.entity);
    if (!cache) 
        return;

    const PanelComponent& p = scene.getComponentConst<PanelComponent>(pb.entity);
    if (pb.fillElementName.empty()){
        pb.fillElementName = p.containerName +"/Fill";
    }
    HUGO::ErrorOrType<Ogre::OverlayElement *> elementResult = HUGO::OgreOverlayUtils::getElement(pb.fillElementName, false, cache->container);
    if (elementResult.isError())
        return;
    Ogre::OverlayElement *area = elementResult.getValue();
    if (!area)
        return;
    
    cache->element = area;

    syncProgressBar(p, pb);
}

void UISystem::onAddSliderComponent(SliderComponent &s)
{
    UIElementCache *cache = getCache(s.entity);
    if (!cache) 
        return;

    const PanelComponent& p = scene.getComponentConst<PanelComponent>(s.entity);

    if (s.trackElementName.empty()){
        s.trackElementName = p.containerName +"/Track";
    }
    if (s.knobElementName.empty()){
        s.knobElementName = p.containerName +"/Knob";
    }

    HUGO::ErrorOrType<Ogre::OverlayElement*> trackResult = HUGO::OgreOverlayUtils::getElement(s.trackElementName, false, cache->container);
    if (trackResult.isError())
        return;
    Ogre::OverlayElement *track = trackResult.getValue();
    if (!track)
        return;
    
    cache->element = track;

    HUGO::ErrorOrType<Ogre::OverlayElement*> knobResult = HUGO::OgreOverlayUtils::getElement(s.knobElementName, false, cache->container);
    if (knobResult.isError())
        return;
    Ogre::OverlayElement *knob = knobResult.getValue();
    if (!knob)
        return;
    
    cache->extra = knob;

    cache->container->removeChild(track->getName());
    cache->container->removeChild(knob->getName());

    cache->container->addChild(track);
    cache->container->addChild(knob);

    syncSlider(p, s);
}
