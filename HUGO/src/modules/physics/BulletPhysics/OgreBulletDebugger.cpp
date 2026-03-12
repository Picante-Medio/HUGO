#include "OgreBulletDebugger.h"

OgreBulletDebugger::OgreBulletDebugger()
: mLines("")
,mDebugMode(DBG_DrawWireframe)
{
    // Change render queue of mLines so it renders last
}

// OgreBulletDebugger::~OgreBulletDebugger() {

// }

void OgreBulletDebugger::clear() {
    mLines.clear();
}

void OgreBulletDebugger::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    if(mLines.getSections().empty()) {
        mLines.setBufferUsage(Ogre::HBU_CPU_TO_GPU);
        mLines.begin(material);
    }
    else if (mLines.getCurrentVertexCount() == 0) {
        mLines.beginUpdate(0);
    }

    // add vertices to manualObject
    Ogre::ColourValue col(color.x(), color.y(), color.z());
    mLines.position(Ogre::Vector3(from.x(), from.y(), from.y()));
    mLines.colour(col);
    mLines.position(Ogre::Vector3(to.x(), to.y(), to.z()));
    mLines.colour(col);

    // Call update once all lines have been drawn
    // mLines.end();
}

void OgreBulletDebugger::setDebugMode(int mode) {
    mDebugMode = mode;

    if (mDebugMode == DBG_NoDebug)
        clear();
}