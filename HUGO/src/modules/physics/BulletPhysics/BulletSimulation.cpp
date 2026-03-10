#include "BulletSimulation.h"
#include "Scene.h"
#include "HugoMotionState.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "PhysicsTags.h"
#include "BulletUtils.h"
#include "HugoDispatcher.h"

BulletSimulation::BulletSimulation(std::shared_ptr<Scene> scene)
: IPhysicsSimulation(scene)
{
    // We are using bullet's default collision configuration. Custom configuration is recommended for more complexity
    collisionConfig = new btDefaultCollisionConfiguration();

    // We are using single-thread dispatcher (narrowphase)
    dispatcher = new HUGODispatcher(collisionConfig);

    // We set the broadphase as a DBVT (tree/hierarchy broadphase)
    overlappingPairCache = new btDbvtBroadphase();

    // We are using single-thread constraint solver
    solver = new btSequentialImpulseConstraintSolver();

    // Finally we create the world and set its gravity
    btWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
    btWorld->setGravity(btVector3(0, -9.8f, 0));
}

BulletSimulation::~BulletSimulation() {
    destroySimulation();
}

void BulletSimulation::destroySimulation() {
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
        collisionShapes[j].id = HUGO::INVALID_SHAPE_ID;
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
    syncBodiesToGame();
}

void BulletSimulation::stepSimulation(double dt) {
    btWorld->stepSimulation(static_cast<btScalar>(dt));
}

#pragma region ShapeObject
HUGO::CollisionShapeID BulletSimulation::createShapeObject(ColliderComponent& col)
{

    btCollisionShape* resShape = nullptr;
    if(std::holds_alternative<BoxColliderInfo>(col.info)){
        resShape = new btBoxShape(BulletUtils::vector3ToBullet(std::get<BoxColliderInfo>(col.info).halfExtents));
    }
    else if(std::holds_alternative<SphereColliderInfo>(col.info)){
        resShape = new btSphereShape(std::get<SphereColliderInfo>(col.info).r);
    }
    else if(std::holds_alternative<CapsuleColliderInfo>(col.info)){
        CapsuleColliderInfo aux = std::get<CapsuleColliderInfo>(col.info);
        resShape = new btCapsuleShape(aux.r, aux.height);
    }
    else if(std::holds_alternative<CylinderColliderInfo>(col.info)){
        resShape = new btCylinderShape(BulletUtils::vector3ToBullet(std::get<CylinderColliderInfo>(col.info).halfExtents));
    }
    else{ // Cone
        ConeColliderInfo aux = std::get<ConeColliderInfo>(col.info);
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
    col.shapeID = id;

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
    if (id != HUGO::INVALID_SHAPE_ID){
        
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
        id = HUGO::INVALID_SHAPE_ID;
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
HUGO::CollisionObjectID BulletSimulation::createColliderObject(ColliderComponent& hgCol, TransformComponent& tf)
{
    // Create/reuse shape object
    btCollisionShape* shape = nullptr;
    if(hgCol.shapeID != HUGO::INVALID_SHAPE_ID){
        shape = getShapeObject(hgCol.shapeID).bulletShape;
    }
    else{
        shape = getShapeObject(createShapeObject(hgCol)).bulletShape;
    }

    // Create collision object
    btCollisionObject* obj = new btCollisionObject();
    obj->setCollisionShape(shape);

    HUGO::CollisionObjectID id;
    //reuse id
    if(!freeBulletRBObjectId.empty()){ 
        id = freeBulletRBObjectId.back();
        freeBulletRBObjectId.pop_back();
    }
    else{
        //generate new ID
        id = static_cast<HUGO::CollisionObjectID>(rbIdToIndex.size());
        rbIdToIndex.push_back(0);
    }
    
    //create CollisionObject
    CollisionObject colObj = {id, obj};

    //store object
    std::size_t index = rigidBodies.push_back(std::move(colObj));
    rbIdToIndex[id] = index;

    //add to simulation
    btWorld->addCollisionObject(obj);

    //save reference to HugoRB in bulletRB
    obj->setUserPointer(&hgCol);

    return id;
}

HUGO::CollisionObjectID BulletSimulation::upgradeToRigidBodyObject(ColliderComponent& hgCol, RigidBodyComponent& hgRb, TransformComponent& tf, HUGO::BodyType type)
{
    CollisionObject obj = getCollisionObject(hgCol.objectID);
    obj.bulletRB = btRigidBody::upcast(obj.bulletRB);

    // Create state
    HUGOMotionState* state = new HUGOMotionState(btTransform(), tf);
    state->updateBulletTransform();

    // Set bullet rb parameters
    btRigidBody* rb = static_cast<btRigidBody*>(obj.bulletRB);
    rb->setMassProps(hgRb.mass, rb->getLocalInertia());
    rb->setMotionState(state);

    // Set flags for static, dynamic or kinematic
    switch(type){
        case HUGO::BodyType::STATIC:
            rb->setCollisionFlags(rb->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
            break;
        case HUGO::BodyType::KINEMATIC:
            rb->setCollisionFlags(rb->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            rb->setActivationState(DISABLE_DEACTIVATION);
            break;
        case HUGO::BodyType::DYNAMIC:
        default:
            break;
    }

    return obj.id;
}
    
CollisionObject& BulletSimulation::getCollisionObject(const HUGO::CollisionObjectID id)
{
    return rigidBodies[rbIdToIndex[id]];
}

void BulletSimulation::removeRigidBodyObject(HUGO::CollisionObjectID& id)
{
    if(id != HUGO::INVALID_COL_OBJ_ID){
        CollisionObject obj = getCollisionObject(id);
        obj.bulletRB = static_cast<btCollisionObject*>(obj.bulletRB);
        id = HUGO::INVALID_COL_OBJ_ID;
    }
}

//TODO ME QUEDO REFACTORIZANDO AQUI
    
void BulletSimulation::removeColliderObject(HUGO::CollisionObjectID& id)
{
    if (id != HUGO::INVALID_COL_OBJ_ID){
        
        std::size_t index = rbIdToIndex[id];
        std::size_t lastIndex = rigidBodies.size() - 1;

        destroyCollisionObject(rigidBodies[index]);

        if (index != lastIndex) {
            // mover último objeto al hueco
            rigidBodies[index] = std::move(rigidBodies[lastIndex]);
            HUGO::CollisionObjectID movedID = rigidBodies[index].id;
            rbIdToIndex[movedID] = index;
        }

        rigidBodies.remove(lastIndex);
        freeBulletRBObjectId.push_back(id);
        id = HUGO::INVALID_COL_OBJ_ID;
    }
}
    
void BulletSimulation::destroyCollisionObject(CollisionObject& RbObj)
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
        CollisionObject obj = rigidBodies[rbIdToIndex[hgRB.objectID]];

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
            hgRB.impulseApplied = HUGO::Vector3();
        }
        if(hgRB.flags & RigidBodyComponent::TORQUE_IMPULSE_APPLIED){
            obj.bulletRB->applyTorqueImpulse(BulletUtils::vector3ToBullet(hgRB.torqueImpulseApplied));
            hgRB.torqueImpulseApplied = HUGO::Vector3();
        }
        if(hgRB.flags & RigidBodyComponent::FORCE_APPLIED){
            obj.bulletRB->applyCentralForce(BulletUtils::vector3ToBullet(hgRB.forceApplied));
            hgRB.forceApplied = HUGO::Vector3();
        }
        if(hgRB.flags & RigidBodyComponent::TORQUE_APPLIED){
            obj.bulletRB->applyTorque(BulletUtils::vector3ToBullet(hgRB.torqueApplied));
            hgRB.torqueApplied = HUGO::Vector3();
        }
        if(hgRB.flags & RigidBodyComponent::TRANSFORM_OFFSET_UPDATED){
            btTransform btTrans = obj.bulletRB->getCenterOfMassTransform();
            btTrans.setRotation(btTrans.getRotation() + BulletUtils::quaternionToBullet(hgRB.rotationOffset));
            btTrans.setOrigin(btTrans.getOrigin() + BulletUtils::vector3ToBullet(hgRB.positionOffset));
            obj.bulletRB->setCenterOfMassTransform(btTrans);
        }
        if(hgRB.flags & RigidBodyComponent::ACTIVE_UPDATED){
            if(hgRB.active)
            obj.bulletRB->forceActivationState(ACTIVE_TAG);
            else
            obj.bulletRB->setActivationState(DISABLE_SIMULATION);
        }
        if(hgRB.flags & RigidBodyComponent::SLEEP_UPDATED){
            if(!hgRB.asleep)
                // true isn't for setting if it's active or unactive but to force the activation even if the object isn't dynamic
                obj.bulletRB->activate(true);
            else
                obj.bulletRB->setActivationState(ISLAND_SLEEPING);
        }
        if(hgRB.flags & RigidBodyComponent::SLEEPING_THRESHOLD_UPDATED){
            obj.bulletRB->setSleepingThresholds(hgRB.linearSleepingThreshold, hgRB.angularSleepingThreshold);
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

        if (rb.objectID == HUGO::INVALID_COL_OBJ_ID) {
            //create a new CollisionObject
            rb.objectID = upgradeToRigidBodyObject(rb, transform, HUGO::BodyType::STATIC);
        }

        rb.collisionInfo.clear();
        checkFlags(rb);
    }
    
    // Kinematic rigidBodies (creation and update)
    for(auto& pack: scene->query<KinematicRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_COL_OBJ_ID) {
            //create a new CollisionObject
            rb.objectID = upgradeToRigidBodyObject(rb, transform, HUGO::BodyType::KINEMATIC);
        }
        
        HUGOMotionState* motionState = static_cast<HUGOMotionState*>(getCollisionObject(rb.objectID).bulletRB->getMotionState());
        motionState->updateBulletTransform();

        rb.collisionInfo.clear();
        checkFlags(rb);
    }
    
    // Dynamic rigidBodies
    for(auto& pack: scene->query<DynamicRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_COL_OBJ_ID) {
            //create a new CollisionObject
            rb.objectID = upgradeToRigidBodyObject(rb, transform, HUGO::BodyType::DYNAMIC);
        }
        
        rb.collisionInfo.clear();
        checkFlags(rb);
    }
}

void BulletSimulation::syncBodiesToGame() {

    for(auto& pack: scene->query<RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        btRigidBody* btRb = getCollisionObject(rb.objectID).bulletRB;
        
        if(btRb->wantsSleeping())
            rb.asleep = true;
        else
            rb.asleep = false;
        
        // Update velocity if the object collided
        rb.linearVelocity = BulletUtils::btVector3ToVector3(btRb->getLinearVelocity());
        rb.angularVelocity = BulletUtils::btVector3ToVector3(btRb->getAngularVelocity());

        // Clear forces applied this frame
        btRb->clearForces();
    }
}
#pragma endregion