#pragma once
#include <vector>
#include "System.h"
#include "Ogre.h"
#include "Ecs.h"

struct OgreObject{
    Ogre::Entity* ogreEntity;
    Ogre::SceneNode* sceneNode;
};

class RenderSystem : public System{
    std::shared_ptr<Ogre::Root> ogreRoot;
    Ogre::SceneManager* sceneManager;
    Ogre::RenderWindow* renderWindow; 
    Ogre::Viewport* viewport;

    Ogre::SceneNode* defaultCameraNode;

    std::vector<std::pair<int, int>> resolutions;

    void handleTag();
    OgreObject createOgreObject(EntityID entity, const std::string& meshName, const std::string& materialName="");
public:
    RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> root, Ogre::RenderWindow* window);
    void update(double dt) override;
    void fixedUpdate(double dt) override;

    void createTestEntity();
    
    void setfullScreen();
    void setWindowed(bool fullScreen);
    void resizeScreen(unsigned int widthPt, unsigned int heightPt);

    void setBackgroundColor(float r, float g, float b);
    void setCameraPosition(float x, float y, float z);
    void setAmbientLight(float r, float g, float b);
};
