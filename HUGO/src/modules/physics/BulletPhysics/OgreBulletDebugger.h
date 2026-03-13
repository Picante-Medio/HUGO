#pragma once

#include "btIDebugDraw.h"
#include "Ogre.h"

class OgreBulletDebugger : public btIDebugDraw {
    // hardcoded for now, might switch to obtaining material pointer from Ogre singleton
    const Ogre::String material = "debug/DefaultMaterial";  

    Ogre::ManualObject mLines;
    int mDebugMode = DBG_DrawWireframe;

    void clear();

    public:
    OgreBulletDebugger();
    // ~OgreBulletDebugger();

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, 
                        int lifeTime, const btVector3& color) override;
    
    void reportErrorWarning(const char* warningString) override {}

	void draw3dText(const btVector3& location, const char* textString) override {}

    int getDebugMode() const override { return mDebugMode; }
    void setDebugMode(int mode) override;

    void begin();
    void end();
};