#pragma once
#include "System.h"

class RenderSystem : public System{
private:
    std::shared_ptr<Ogre::Root> ogreRoot;
    Ogre::SceneManager* sceneManager;
    Ogre::RenderWindow* renderWindow; 
    Ogre::Viewport* viewport;

    Ogre::SceneNode* defaultCameraNode;

    std::vector<std::pair<int, int>> resolutions;

    void handleTag();

public:
    RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> root, Ogre::RenderWindow* window);
    void update(double dt) override;
    void fixedUpdate(double dt) override;

    void setfullScreen();
    void setWindowed();
    void resizeScreen(unsigned int widthPt, unsigned int heightPt);
};