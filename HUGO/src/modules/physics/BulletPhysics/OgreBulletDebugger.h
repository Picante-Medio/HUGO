#pragma once

#include "btIDebugDraw.h"
#include "Ogre.h"

class OgreBulletDebugger : public btIDebugDraw {
    // hardcoded for now, might switch to obtaining material pointer from Ogre singleton
    const Ogre::String material = "DefaultDebugMaterial";  

    Ogre::ManualObject mLines;
    int mDebugMode;

    void clear();

    public:
    OgreBulletDebugger();
    // ~OgreBulletDebugger();

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

    int getDebugMode() const override { return mDebugMode; }
    void setDebugMode(int mode) override;
};