#include "UISystem.h"

#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"

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

std::shared_ptr<UIElementCache> UISystem::createElementCache(
    Ogre::Overlay* o, Ogre::OverlayContainer* c, Ogre::OverlayElement* e)
{
    std::shared_ptr<UIElementCache> cache = std::make_shared<UIElementCache>();
    cache->overlay = o;
    cache->container = c;
    cache->element = e;
    return cache;
}

std::shared_ptr<UIElementCache> UISystem::createElementCache(
    const HUGO::String& overlayName, 
    const HUGO::String& containerName, 
    const HUGO::String& elementName)
{
    std::shared_ptr<UIElementCache> cache = std::make_shared<UIElementCache>();

    ErrorOrType<Ogre::Overlay*> overlayResult = OgreOverlayUtils::getOverlay(overlayName);
    if (!overlayResult.isError())
        cache->overlay = overlayResult.getValue();

    ErrorOrType<Ogre::OverlayContainer*> containerResult = OgreOverlayUtils::getContainer(containerName);
    if (!containerResult.isError())
        cache->container = containerResult.getValue();

    if (!elementName.empty())
    {
        ErrorOrType<Ogre::OverlayElement*> elementResult = OgreOverlayUtils::getElement(elementName);
        if (!elementResult.isError())
            cache->element = elementResult.getValue();
    }

    return cache;
}

UIElementCache* UISystem::getCache(EntityID entity)
{
    auto it = uiCache.find(entity);
    if (it == uiCache.end()) 
        return nullptr;

    return it->second.get();
}

//====================================================
// SYNCHRONIZATION
//====================================================

void UISystem::syncPanel(const PanelComponent& p, const PanelDirtyTag& tag)
{
    UIElementCache* cache = getCache(p.entity);
    if (!cache) return;

    cache->overlay->setZOrder(p.zOrder);
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

void UISystem::syncText(const PanelComponent& p, const TextComponent& t, const TextDirtyTag& tag)
{
    UIElementCache* cache = getCache(p.entity);
    if (!cache) return;

    Ogre::TextAreaOverlayElement* textArea = dynamic_cast<Ogre::TextAreaOverlayElement*>(cache->element);
    if (!textArea) return;

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

//====================================================
// PROCESSING
//====================================================

void UISystem::processVisibility()
{
    for (auto& pack : scene->query<PanelComponent, VisibilityDirtyTag>())
    {
        PanelComponent& panel = pack.get<PanelComponent&>();
        UIElementCache* cache = getCache(panel.entity);

        if (cache && cache->overlay && cache->container)
        {
            if (panel.isVisible)
            {
                cache->overlay->show();
                cache->container->show();
                if (cache->element) cache->element->show();
            }
            else
            {
                cache->overlay->hide();
                cache->container->hide();
                if (cache->element) cache->element->hide();
            }
        }

        scene->removeComponent<VisibilityDirtyTag>(panel.entity);
    }
}

void UISystem::processPanelChanges()
{
    for (auto& pack : scene->query<PanelComponent, PanelDirtyTag>())
    {
        PanelComponent& panel = pack.get<PanelComponent&>();
        PanelDirtyTag& tag = pack.get<PanelDirtyTag&>();
        syncPanel(panel, tag);
        scene->removeComponent<PanelDirtyTag>(tag.entity);
    }
}

void UISystem::processTextChanges()
{
    for (auto& pack : scene->query<PanelComponent, TextComponent, TextDirtyTag>())
    {
        PanelComponent& panel = pack.get<PanelComponent&>();
        TextComponent& text = pack.get<TextComponent&>();
        TextDirtyTag& tag = pack.get<TextDirtyTag&>();
        syncText(panel, text, tag);
        scene->removeComponent<TextDirtyTag>(tag.entity);
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
    createTestText();
}

void UISystem::onLoad() {}

void UISystem::update(double)
{
    processPanelChanges();
    processTextChanges();
    processVisibility();
}

void UISystem::fixedUpdate(double) {}

//====================================================
// TEST UI
//====================================================

const PanelComponent& UISystem::createTestCanvas()
{
    EntityID ent = scene->createEntity();
    PanelComponent& panel = scene->addAndGetComponent<PanelComponent>(ent);

    panel.overlayName = "testCanvas/SimpleTest";
    panel.containerName = panel.overlayName + "/Container";

    ErrorOrType<Ogre::Overlay*> overlayResult = OgreOverlayUtils::getOverlay(panel.overlayName);
    if (overlayResult.isError()) return panel;

    Ogre::Overlay* overlay = overlayResult.getValue();
    panel.zOrder = overlay->getZOrder();

    ErrorOrType<Ogre::OverlayContainer*> containerResult = OgreOverlayUtils::getContainer(panel.containerName);
    if (containerResult.isError()) return panel;

    Ogre::OverlayContainer* container = containerResult.getValue();

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

    // insert shared_ptr directly
    uiCache[ent] = createElementCache(overlay, container);

    return panel;
}

void UISystem::createTestText()
{
    const PanelComponent& p = createTestCanvas();
    EntityID ent = p.entity;
    TextComponent& text = scene->addAndGetComponent<TextComponent>(ent);
    text.textAreaName = p.containerName + "/Text";
    ErrorOrType<Ogre::OverlayElement*> elementResult = OgreOverlayUtils::getElement(text.textAreaName);
    if (elementResult.isError()) return;

    Ogre::TextAreaOverlayElement* area = dynamic_cast<Ogre::TextAreaOverlayElement*>(elementResult.getValue());
    if (!area) return;

    text.characterHeight = area->getCharHeight();
    text.text = area->getCaption();

    Ogre::ColourValue c = area->getColour();
    Ogre::ColourValue ct = area->getColourTop();
    Ogre::ColourValue cb = area->getColourBottom();

    text.colour = {c.r, c.g, c.b, c.a};
    text.colourTop = {ct.r, ct.g, ct.b, ct.a};
    text.colourBottom = {cb.r, cb.g, cb.b, cb.a};

    UIElementCache* cache = getCache(ent);
    if (cache) cache->element = area;
}