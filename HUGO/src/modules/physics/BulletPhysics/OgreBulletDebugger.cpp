#include "OgreBulletDebugger.h"

OgreBulletDebugger::OgreBulletDebugger()
: mLines("")
,mDebugMode(DBG_DrawWireframe)
{
    // Change render queue of mLines so it renders last
    // mLines.setRenderQueueGroup( Ogre::RENDER_QUEUE_TRANSPARENTS);

    Ogre::Root* ogreRoot = Ogre::Root::getSingletonPtr();
    Ogre::SceneManager* sceneManager = ogreRoot->_getCurrentSceneManager();
    Ogre::SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(&mLines);
}

// OgreBulletDebugger::~OgreBulletDebugger() {

// }

void OgreBulletDebugger::clear() {
    mLines.clear();
}

void OgreBulletDebugger::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {

    // add vertices to manualObject
    Ogre::ColourValue col(color.x(), color.y(), color.z());
    mLines.position(Ogre::Vector3(from.x(), from.y(), from.y()));
    mLines.colour(col);
    mLines.position(Ogre::Vector3(to.x(), to.y(), to.z()));
    mLines.colour(col);
}

void  OgreBulletDebugger::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, 
                                            int, const btVector3& color) {

    drawLine(PointOnB, PointOnB + normalOnB * distance * 20, color);
}

void OgreBulletDebugger::setDebugMode(int mode) {
    mDebugMode = mode;

    if (mDebugMode == DBG_NoDebug)
        clear();
}

void OgreBulletDebugger::begin() {

    if(mLines.getSections().empty()) {
        mLines.setBufferUsage(Ogre::HBU_CPU_TO_GPU);
        mLines.begin(material, Ogre::RenderOperation::OT_LINE_LIST);
    }
    else if (mLines.getCurrentVertexCount() == 0) {
        mLines.beginUpdate(0);
    }
}


void OgreBulletDebugger::end() {
    if (!mLines.getSections().empty()) // begin was called
        mLines.end();
}
