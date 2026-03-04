#include "BulletSimulation.h"
#include "Scene.h"
#include "HugoMotionState.h"
#include "RigidBodyComponent.h"
#include "PhysicsTags.h"
#include "BulletUtils.h"

BulletSimulation::BulletSimulation(std::shared_ptr<Scene> scene)
: IPhysicsSimulation(scene)
{
    // We are using bullet's default collision configuration. Custom configuration is recommended for more complexity
    collisionConfig = new btDefaultCollisionConfiguration();

    // We are using single-thread dispatcher (narrowphase)
    dispatcher = new btCollisionDispatcher(collisionConfig);

    // We set the broadphase as a DBVT (tree/hierarchy broadphase)
    overlappingPairCache = new btDbvtBroadphase();

    // We are using single-thread constraint solver
    solver = new btSequentialImpulseConstraintSolver();

    // Finally we create the world and set its gravity
    btWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
    btWorld->setGravity(btVector3(0, -9.8, 0));
}

BulletSimulation::~BulletSimulation() {
    // remove and delete rigidbodies
    for (int i = btWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = btWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
        
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		btWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j].bulletShape;
		collisionShapes[j].bulletShape = 0;
        collisionShapes[j].id = HUGO::INVALID_BULLET_SHAPE_ID;
		delete shape;
	}


    delete btWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfig;

}

void BulletSimulation::performSimulation(double dt) {
    syncBodiesToBullet();
    stepSimulation(dt);
    // syncBodiesToGame();
}

void BulletSimulation::stepSimulation(double dt) {
    btWorld->stepSimulation(dt);
}

#pragma region ShapeObject
HUGO::CollisionShapeID BulletSimulation::createShapeObject(Shape& shape)
{

    btCollisionShape* resShape = nullptr;
    if(std::holds_alternative<BoxColliderInfo>(shape.info)){
        resShape = new btBoxShape(BulletUtils::vector3ToBullet(std::get<BoxColliderInfo>(shape.info).halfExtents));
    }
    else if(std::holds_alternative<SphereColliderInfo>(shape.info)){
        resShape = new btSphereShape(std::get<SphereColliderInfo>(shape.info).r);
    }
    else if(std::holds_alternative<CapsuleColliderInfo>(shape.info)){
        CapsuleColliderInfo aux = std::get<CapsuleColliderInfo>(shape.info);
        resShape = new btCapsuleShape(aux.r, aux.height);
    }
    else if(std::holds_alternative<CylinderColliderInfo>(shape.info)){
        resShape = new btCylinderShape(BulletUtils::vector3ToBullet(std::get<CylinderColliderInfo>(shape.info).halfExtents));
    }
    else{ // Cone
        ConeColliderInfo aux = std::get<ConeColliderInfo>(shape.info);
        resShape = new btConeShape(aux.r, aux.height);
    }

    HUGO::CollisionShapeID id;
    //reuse id
    if(!freeBulletShapeId.empty()){ 
        id= freeBulletShapeId.back();
        freeBulletShapeId.pop_back();
    }
    else{
        //generate new ID
        id = static_cast<HUGO::CollisionShapeID>(shapeIdToIndex.size());
        shapeIdToIndex.push_back(0);
    }

    //create Bullet Shape
    ShapeObject obj = {id, resShape};
    shape.id = id;

    //store object
    std::size_t index = collisionShapes.push_back(std::move(obj));
    shapeIdToIndex[id] = index;

    return id;
}

ShapeObject& BulletSimulation::getShapeObject(const HUGO::CollisionShapeID id)
{
    return collisionShapes[shapeIdToIndex[id]];
}

void BulletSimulation::removeShapeObject(HUGO::CollisionShapeID& id)
{
    // Must make sure any rigidbody that used the shape to be deleted doesn't exist anymore
    if (id != HUGO::INVALID_BULLET_SHAPE_ID){
        
        std::size_t index = shapeIdToIndex[id];
        std::size_t lastIndex = collisionShapes.size() - 1;

        destroyShapeObject(collisionShapes[index]);

        if (index != lastIndex) {
            // mover último objeto al hueco
            collisionShapes[index] = std::move(collisionShapes[lastIndex]);
            HUGO::CollisionShapeID movedID = collisionShapes[index].id;
            shapeIdToIndex[movedID] = index;
        }

        collisionShapes.remove(lastIndex);
        freeBulletShapeId.push_back(id);
        id = HUGO::INVALID_BULLET_SHAPE_ID;
    }
}

void BulletSimulation::destroyShapeObject(ShapeObject& shObj)
{
    if(shObj.bulletShape){
        delete shObj.bulletShape;
        shObj.bulletShape = nullptr;
    }
}
#pragma endregion


#pragma region BulletObject
HUGO::SimulationRBID BulletSimulation::createRigidBodyObject(RigidBodyComponent& hgRb, const TransformComponent& tf, bodyType type)
{

    // Create/reuse shape object
    btCollisionShape* shape = nullptr;
    if(hgRb.rbShape.id != HUGO::INVALID_BULLET_SHAPE_ID){
        shape = getShapeObject(hgRb.rbShape.id).bulletShape;
    }
    else{
        shape = getShapeObject(createShapeObject(hgRb.rbShape)).bulletShape;
    }

    // Create state
    HUGOMotionState* state = new HUGOMotionState(btTransform(), std::make_shared<TransformComponent>(tf));
    state->updateBulletTransform();

    // Create bullet rb
    btRigidBody* rb = new btRigidBody(hgRb.mass, state, shape);

    // Set flags for static, dynamic or kinematic
    switch(type){
        case bodyType::STATIC:
            rb->setFlags(rb->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
            break;
        case bodyType::KINEMATIC:
            rb->setFlags(rb->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            rb->setActivationState(DISABLE_DEACTIVATION);
            break;
        case bodyType::DYNAMIC:
        default:
            break;
    }

    HUGO::SimulationRBID id;
    //reuse id
    if(!freeBulletRBObjectId.empty()){ 
        id= freeBulletRBObjectId.back();
        freeBulletRBObjectId.pop_back();
    }
    else{
        //generate new ID
        id = static_cast<HUGO::SimulationRBID>(rbIdToIndex.size());
        rbIdToIndex.push_back(0);
    }
    
    //create RigidBodyObject
    RigidBodyObject obj = {id, rb};

    //store object
    std::size_t index = rigidBodies.push_back(std::move(obj));
    rbIdToIndex[id] = index;

    return id;
}
    
RigidBodyObject& BulletSimulation::getRigidBodyObject(const HUGO::SimulationRBID id)
{
    return rigidBodies[rbIdToIndex[id]];
}
    
void BulletSimulation::removeRigidBodyObject(HUGO::SimulationRBID& id)
{
    if (id != HUGO::INVALID_BULLET_RB_ID){
        
        std::size_t index = rbIdToIndex[id];
        std::size_t lastIndex = rigidBodies.size() - 1;

        destroyRigidBodyObject(rigidBodies[index]);

        if (index != lastIndex) {
            // mover último objeto al hueco
            rigidBodies[index] = std::move(rigidBodies[lastIndex]);
            HUGO::SimulationRBID movedID = rigidBodies[index].id;
            rbIdToIndex[movedID] = index;
        }

        rigidBodies.remove(lastIndex);
        freeBulletRBObjectId.push_back(id);
        id = HUGO::INVALID_BULLET_RB_ID;
    }
}
    
void BulletSimulation::destroyRigidBodyObject(RigidBodyObject& RbObj)
{
    btRigidBody* body = RbObj.bulletRB;

    if(body){
        if (body->getMotionState())
	    {
		    delete body->getMotionState();
	    }

	    btWorld->removeCollisionObject(body);
	    delete body;
        RbObj.bulletRB = nullptr;
    }
}
#pragma endregion

#pragma region RigidBody
void BulletSimulation::checkFlags(RigidBodyComponent& hgRB)
{
    if(hgRB.flags != 0){
        RigidBodyObject obj = rigidBodies[rbIdToIndex[hgRB.objectID]];

        if(hgRB.flags & RigidBodyComponent::RESTITUTION_UPDATED){
            obj.bulletRB->setRestitution(hgRB.restitution);
        }
        if(hgRB.flags & RigidBodyComponent::FRICTION_UPDATED){
            obj.bulletRB->setFriction(hgRB.friction);
        }
        if(hgRB.flags & RigidBodyComponent::MASS_UPDATED){
            obj.bulletRB->setMassProps(hgRB.mass, BulletUtils::vector3ToBullet( HUGO::Vector3()));
        }
        if(hgRB.flags & RigidBodyComponent::DAMPING_UPDATED){
            obj.bulletRB->setDamping(hgRB.linearDamping, hgRB.angularDamping);
        }
        if(hgRB.flags & RigidBodyComponent::LINEAR_VELOCITY_UPDATED){
            obj.bulletRB->setLinearVelocity(BulletUtils::vector3ToBullet(hgRB.linearVelocity));
        }
        if(hgRB.flags & RigidBodyComponent::ANGULAR_VELOCITY_UPDATED){
            obj.bulletRB->setAngularVelocity(BulletUtils::vector3ToBullet(hgRB.angularVelocity));
        }
        if(hgRB.flags & RigidBodyComponent::IMPULSE_APPLIED){
            obj.bulletRB->applyCentralImpulse(BulletUtils::vector3ToBullet(hgRB.impulseApplied));
            hgRB.linearVelocity = BulletUtils::btVector3ToVector3(obj.bulletRB->getLinearVelocity());
        }
        if(hgRB.flags & RigidBodyComponent::TORQUE_IMPULSE_APPLIED){
            obj.bulletRB->applyTorqueImpulse(BulletUtils::vector3ToBullet(hgRB.torqueImpulseApplied));
            hgRB.angularVelocity = BulletUtils::btVector3ToVector3(obj.bulletRB->getAngularVelocity());
        }
        if(hgRB.flags & RigidBodyComponent::FORCE_APPLIED){
            obj.bulletRB->applyCentralForce(BulletUtils::vector3ToBullet(hgRB.forceApplied));
        }
        if(hgRB.flags & RigidBodyComponent::TORQUE_APPLIED){
            obj.bulletRB->applyTorque(BulletUtils::vector3ToBullet(hgRB.torqueApplied));
        }
        if(hgRB.flags & RigidBodyComponent::TRANSFORM_OFFSET_UPDATED){
            btTransform btTrans = obj.bulletRB->getCenterOfMassTransform();
            btTrans.setRotation(btTrans.getRotation() + BulletUtils::quaternionToBullet(hgRB.rotationOffset));
            btTrans.setOrigin(btTrans.getOrigin() + BulletUtils::vector3ToBullet(hgRB.positionOffset));
            obj.bulletRB->setCenterOfMassTransform(btTrans);
        }

        hgRB.flags = 0;
    }
}

void BulletSimulation::syncBodiesToBullet() {
    // Add new rigidBodies to simulation and Update kinematic bodies
    // Static rigidBodies 
    for(auto& pack: scene->query<StaticRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();

        if (rb.objectID == HUGO::INVALID_BULLET_RB_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb, transform, bodyType::STATIC);
        }

        checkFlags(rb);
    }
    
    // Kinematic rigidBodies (creation and update)
    for(auto& pack: scene->query<KinematicRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_BULLET_RB_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb, transform, bodyType::KINEMATIC);
        }
        
        HUGOMotionState* motionState = static_cast<HUGOMotionState*>(getRigidBodyObject(rb.objectID).bulletRB->getMotionState());
        motionState->updateBulletTransform();

        checkFlags(rb);
    }
    
    // Dynamic rigidBodies
    for(auto& pack: scene->query<RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_BULLET_RB_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb, transform, bodyType::DYNAMIC);
        }
        
        checkFlags(rb);
    }
}
#pragma endregion