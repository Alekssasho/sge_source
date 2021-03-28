#include "GameWorld.h"
#include "EngineGlobal.h"
#include "GameInspector.h"
#include "GameSerialization.h"
#include "IWorldScript.h"
#include "InspectorCmd.h"
#include "SDL.h"
#include "sge_core/ICore.h"
#include "sge_utils/utils/strings.h"
#include "traits/TraitCamera.h"
#include <functional>
#include <thread>

namespace sge {

ObjectId GameWorld::getNewId() {
	ObjectId id(m_nextObjectId);
	m_nextObjectId++;
	return id;
}

bool GameWorld::isIdTaken(ObjectId const id) const {
	bool result = false;
	iterateOverPlayingObjects(
	    [&](const GameObject* obj) -> bool {
		    if (obj->getId() == id) {
			    result = true;
			    return false;
		    }

		    return true;
	    },
	    true);


	return result;
}

GameObject* GameWorld::getObjectById(const ObjectId& id) {
	debug.numCallsToGetObjectByIdThisFrame++;

	GameObject* result = nullptr;
	iterateOverPlayingObjects(
	    [&](GameObject* obj) -> bool {
		    if (obj->getId() == id) {
			    result = obj;
			    return false;
		    }

		    return true;
	    },
	    true);

	return result;
}

Actor* GameWorld::getActorById(const ObjectId& id) {
	GameObject* obj = getObjectById(id);
	return obj ? obj->getActor() : nullptr;
}

GameObject* GameWorld::getObjectByName(const char* name) {
	GameObject* result = nullptr;
	iterateOverPlayingObjects(
	    [&](GameObject* obj) -> bool {
		    if (obj->getDisplayName() == name) {
			    result = obj;
			    return false;
		    }

		    return true;
	    },
	    true);

	return result;
}

Actor* GameWorld::getActorByName(const char* name) {
	GameObject* obj = getObjectByName(name);
	return obj ? obj->getActor() : nullptr;
}

void GameWorld::iterateOverPlayingObjects(const std::function<bool(GameObject*)>& lambda, bool includeAwaitCreationObject) {
	// Caution:
	// If you do any changes here to them in the const method equivalednt!
	for (auto& itrActorByType : playingObjects) {
		bool isDoneIterating = false;
		for (int t = 0; t < itrActorByType.second.size(); ++t) {
			GameObject* const object = itrActorByType.second[t];
			sgeAssert(object->getType() == itrActorByType.first);

			sgeAssert(object);
			if (object) {
				if (lambda(object) == false) {
					isDoneIterating = true;
					break;
				}
			}
		}

		if (isDoneIterating) {
			break;
		}
	}

	if (includeAwaitCreationObject) {
		for (GameObject* const object : awaitsCreationObjects) {
			sgeAssert(object);
			if (object) {
				if (lambda(object) == false) {
					break;
				}
			}
		}
	}
}

void GameWorld::iterateOverPlayingObjects(const std::function<bool(const GameObject*)>& lambda, bool includeAwaitCreationObject) const {
	// Caution:
	// If you do any changes here to them in the non-const method equivalednt!

	for (auto& itrActorByType : playingObjects) {
		bool isDoneIterating = false;
		for (int t = 0; t < itrActorByType.second.size(); ++t) {
			GameObject* const object = itrActorByType.second[t];
			sgeAssert(object->getType() == itrActorByType.first);

			sgeAssert(object);
			if (object) {
				if (lambda(object) == false) {
					isDoneIterating = true;
					break;
				}
			}
		}

		if (isDoneIterating) {
			break;
		}
	}

	if (includeAwaitCreationObject) {
		for (GameObject* const object : awaitsCreationObjects) {
			sgeAssert(object);
			if (object) {
				if (lambda(object) == false) {
					break;
				}
			}
		}
	}
}

GameObject* GameWorld::allocObject(TypeId const type, ObjectId const specificId, const char* name) {
	// If a specific id is desiered first check if this id is available for use.
	if (!specificId.isNull()) {
		if (isIdTaken(specificId)) {
			sgeAssert(false && "Trying to allocate an object with specific id, but the id is already taken!");
			return nullptr;
		}
	}

	const TypeDesc* const newActorTypeDesc = typeLib().find(type);

	const bool hasDesc = newActorTypeDesc != nullptr;
	const bool doesInheritsObject = hasDesc && newActorTypeDesc->doesInherits(sgeTypeId(GameObject));
	const bool isAllocateable = hasDesc && newActorTypeDesc->newFn != nullptr;

	if_checked(hasDesc && doesInheritsObject && isAllocateable) {
		// Caution:
		// Object must be the leftmost parent!
		GameObject* const object = reinterpret_cast<GameObject*>(newActorTypeDesc->newFn());

		if (!object) {
			sgeAssert(false && "Failed to allocate GameObject");
			return nullptr;
		}

		ObjectId const newObjectId = specificId.isNull() ? getNewId() : specificId;
		std::string displayName =
		    (name != nullptr) ? name : std::string(typeLib().find(type)->name) + string_format("_%d", getNextNameIndex());

		object->worldInitializeMe(this, newObjectId, type, std::move(displayName));
		object->create();

		awaitsCreationObjects.push_back(object);

		return object;
	}

	return nullptr;
}

Actor* GameWorld::allocActor(TypeId const type, ObjectId const specificId, const char* name) {
	const TypeDesc* const td = typeLib().find(type);
	if (td == nullptr || td->doesInherits(sgeTypeId(Actor)) == false) {
		sgeAssert(false && "Trying to alocate an Actor with type which is not an actor!");
		return nullptr;
	}

	Actor* actor = dynamic_cast<Actor*>(allocObject(type, specificId, name));
	sgeAssert(actor && "The allocActor expects the allocated object to be an Actor");
	return actor;
}

void GameWorld::objectDelete(const ObjectId& id) {
	objectsWantingPermanentKill.add(id);
}

void GameWorld::create() {
	physicsWorld.create();
	physicsWorld.dynamicsWorld->setGravity(toBullet(m_defaultGravity));
	physicsWorld.dynamicsWorld->setDebugDrawer(&m_physicsDebugDraw);
}

void GameWorld::clear() {
	m_workingFilePath.clear();

	m_nextObjectId = 1;
	std::string p;
	int c = 0;
	for (auto& itrObjByType : playingObjects)
		for (int t = 0; t < itrObjByType.second.size(); ++t) {
			GameObject* const object = itrObjByType.second[t];
			sgeAssert(object->getType() == itrObjByType.first);
			object->onPlayStateChanged(false);
			p = typeLib().find(object->getType())->name;
			c = physicsWorld.dynamicsWorld->getCollisionObjectArray().size();
		}

	for (auto& itrObjByType : playingObjects)
		for (int t = 0; t < itrObjByType.second.size(); ++t) {
			GameObject* const object = itrObjByType.second[t];
			delete object;
		}

	playingObjects.clear();

	for (GameObject* const object : awaitsCreationObjects) {
		delete object;
	}
	awaitsCreationObjects.clear();

	m_nextNameIndex = 0;
	totalStepsTaken = 0;
	gameTime = 0.f;

	m_defaultGravity = vec3f(0.f, -10.f, 0.f);
	m_physicsSimNumSubSteps = 3;

	m_skyColorBottom = vec3f(0.419f);
	m_skyColorTop = vec3f(0.133f);

	m_childernOf.clear();
	m_parentOf.clear();

	physicsWorld.destroy();
	m_physicsManifoldList.clear();

	onWorldLoaded.discardAllCallbacks();

	m_postSceneUpdateTasks.clear();

	gridShouldDraw = true;
	gridNumSegments = vec2i(10);
	gridSegmentsSpacing = 1.f;

	needsLockedCursor = false;
	m_cameraPovider = ObjectId();
	m_useEditorCamera = true;
	m_editorCamera.m_orbitCamera.yaw = -sgeHalfPi;
	m_editorCamera.m_orbitCamera.pitch = deg2rad(35.f);
	m_editorCamera.m_orbitCamera.radius = 20.f;
	m_editorCamera.m_projSets.fov = deg2rad(60.f);
	m_editorCamera.m_projSets.aspectRatio = 1.f; // Just some default to be overriden.
	m_editorCamera.m_projSets.near = 0.1f;
	m_editorCamera.m_projSets.far = 10000.f;
}

void GameWorld::update(const GameUpdateSets& updateSets) {
	if (needsLockedCursor && m_useEditorCamera == false) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	} else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	if (debug.forceSleepMs != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(debug.forceSleepMs));
	}

	debug.numCallsToGetObjectByIdThisFrame = 0;

	m_cachedUpdateSets = updateSets;

	// Add the objects that were created.
	for (int t = 0; t < awaitsCreationObjects.size(); ++t) {
		GameObject* const object = awaitsCreationObjects[t];
		playingObjects[object->getType()].emplace_back(object);
		object->onPlayStateChanged(true);
	}
	awaitsCreationObjects.clear();

	// Remove the objects that want to be suspended.
	// Caution: Assumes that awaitsCreationObjects is already processed, as objects in that list do not
	// yet participate in the playing actors list.
	sgeAssert(awaitsCreationObjects.empty());
	for (const ObjectId objToKillId : objectsWantingPermanentKill) {
		GameObject* const object = this->getObjectById(objToKillId);
		if (object != nullptr) {
			if (inspector)
				inspector->deselect(objToKillId);

			// Signal the object that we are going to suspend it.
			object->onPlayStateChanged(false);

			// Now erase the object form the playing actors list.
			auto& actorsOfType = playingObjects[object->getType()];
			for (int t = 0; t < actorsOfType.size(); ++t) {
				if (actorsOfType[t]->getId() == objToKillId) {
					GameObject* objectToKill = actorsOfType[t];
					// Unparent the object, so it's current no longer thinks that the deleted object
					// is present.
					if (objectToKill->isActor()) {
						setParentOf(objectToKill->getId(), ObjectId());

						// Unparent all childrend objects.
						vector_set<ObjectId> childrenList = getChildensOfAsList(actorsOfType[t]->getId());
						for (int iChild = 0; iChild < childrenList.size(); iChild++) {
							setParentOf(childrenList.getNth(iChild), ObjectId());
						}
					}
					actorsOfType.erase(actorsOfType.begin() + t);
					delete objectToKill;
					objectToKill = nullptr;
					break;
				}
			}
		}
	}
	objectsWantingPermanentKill.clear();

	// Call pre update for scripts.
	for (ObjectId scriptObj : m_scriptObjects) {
		if (IWorldScript* script = dynamic_cast<IWorldScript*>(getObjectById(scriptObj))) {
			script->onPreUpdate(updateSets);
		}
	}

	// Do the simulation step.
	{
		// Update the physics
		if (updateSets.isGamePaused() == false) {
			const int numSubStepsForPhysics = std::max(1, m_physicsSimNumSubSteps);
			physicsWorld.dynamicsWorld->stepSimulation(updateSets.dt, numSubStepsForPhysics, updateSets.dt / float(numSubStepsForPhysics));

			// Get all collision manifolds
			m_physicsManifoldList.clear();

			btDynamicsWorld* const dworld = physicsWorld.dynamicsWorld.get();
			int const numManifolds = dworld->getDispatcher()->getNumManifolds();
			for (int t = 0; t < numManifolds; ++t) {
				const btPersistentManifold* const manifold = dworld->getDispatcher()->getManifoldByIndexInternal(t);

				if (manifold->getNumContacts() != 0) {
					const RigidBody* const rb0 = fromBullet(manifold->getBody0());
					const RigidBody* const rb1 = fromBullet(manifold->getBody1());

					if (rb0) {
						m_physicsManifoldList[rb0].emplace_back(manifold);
					}
					if (rb1) {
						m_physicsManifoldList[rb1].emplace_back(manifold);
					}
				}
			}
		} else {
			// If the game is not running update the bounding boxes tree,
			// as the collision geometry might be needed by some tools (like the PlantingTool).
			physicsWorld.dynamicsWorld->updateAabbs();
		}

		// Update the game objects.
		for (auto& itrActorByType : playingObjects) {
			// TODO: skip object that have no update.
			// This might help:
			// https://stackoverflow.com/questions/4741271/ways-to-detect-whether-a-c-virtual-function-has-been-redefined-in-a-derived-cl
			for (int t = 0; t < itrActorByType.second.size(); ++t) {
				GameObject* const object = itrActorByType.second[t];
				sgeAssert(object != nullptr);
				object->update(updateSets);
			}
		}

		// Post-Update the game objects.
		for (auto& itrActorByType : playingObjects) {
			// TODO: skip object that have no update.
			for (int t = 0; t < itrActorByType.second.size(); ++t) {
				GameObject* const object = itrActorByType.second[t];
				sgeAssert(object != nullptr);
				object->postUpdate(updateSets);
			}
		}

		// Call post update for scripts.
		for (ObjectId scriptObj : m_scriptObjects) {
			if (IWorldScript* script = dynamic_cast<IWorldScript*>(getObjectById(scriptObj))) {
				script->onPostUpdate(updateSets);
			}
		}

		if (updateSets.isGamePaused() == false) {
			gameTime += updateSets.dt;
			totalStepsTaken++;
		}
	}
}

// Used for giving object unique names (However the GameWorld still supports objects with same name).
int GameWorld::getNextNameIndex() {
	int retval = m_nextNameIndex;
	m_nextNameIndex++;
	return retval;
}

bool GameWorld::setParentOf(ObjectId const childId, ObjectId const newParentId, bool doNotAssert) {
	// TODO: Circular hierarchy checks.
	if (childId == newParentId) {
		return false;
	}

	// The new parent must not be anywhere below the child in the hierarchy,
	// as this would introduce circular references.
	{
		vector_set<ObjectId> allHeirarchyUnderChild;
		getAllChildren(allHeirarchyUnderChild, childId);
		const bool isNewPerentInTheHierarchyOfTheChild = allHeirarchyUnderChild.count(newParentId) != 0;

		if (isNewPerentInTheHierarchyOfTheChild) {
			return false;
		}
	}

	Actor* const child = getActorById(childId);
	if (child == nullptr) {
		if (doNotAssert == false) {
			sgeAssert(false);
		}
		return false;
	}

	// Unparent from exsiting parent
	{
		auto itr = m_parentOf.find(childId);
		if (itr != m_parentOf.end()) {
			// Remove from the parent't list of child nodes.
			const ObjectId oldParent = itr->second;
			m_childernOf[oldParent].eraseKey(childId);
			if (m_childernOf[oldParent].size() == 0) {
				m_childernOf.erase(oldParent);
			}

			m_parentOf.erase(itr);
		}
	}

	// Attach to the new parent.
	if (newParentId.isNull()) {
		child->m_bindingToParentTransform = transf3d::getIdentity();
	} else {
		const Actor* const newParent = getActorById(newParentId);
		if_checked(newParent) {
			m_parentOf[childId] = newParentId;
			m_childernOf[newParentId].add(childId);

			transf3d const parentWs = newParent->getTransform();
			transf3d const bindingTransform = child->getTransform().computeBindingTransform(parentWs);
			child->m_bindingToParentTransform = bindingTransform;
			return true;
		}
	}

	return false;
}

ObjectId GameWorld::getParentId(ObjectId const child) const {
	const auto itr = m_parentOf.find(child);
	if (itr != m_parentOf.end())
		return itr->second;

	return ObjectId();
}

Actor* GameWorld::getParentActor(ObjectId const child) {
	ObjectId parentId = getParentId(child);
	return getActorById(parentId);
}

/// Returns the root parent (parent of the parent of the parent and so on) of the specified object.
ObjectId GameWorld::getRootParentId(ObjectId child) const {
	ObjectId parentToReturn;
	while (true) {
		ObjectId parent = getParentId(child);
		if (parent.isNull()) {
			return parentToReturn;
		}
		parentToReturn = parent;
		child = parent;
	}
}

const vector_set<ObjectId>* GameWorld::getChildensOf(ObjectId const parent) const {
	const auto itr = m_childernOf.find(parent);
	if (itr == m_childernOf.end())
		return nullptr;

	return &itr->second;
}

void GameWorld::getAllChildren(vector_set<ObjectId>& result, ObjectId const parent) const {
	const auto itr = m_childernOf.find(parent);
	if (itr == m_childernOf.end()) {
		return;
	}

	for (const ObjectId id : itr->second) {
		result.add(id);
		getAllChildren(result, id);
	}
}

void GameWorld::getAllParents(vector_set<ObjectId>& result, ObjectId actorId) const {
	while (true) {
		actorId = getParentId(actorId);
		if (actorId.isNull())
			break;

		result.add(actorId);
	}
}

void GameWorld::getAllRelativesOf(vector_set<ObjectId>& result, ObjectId actorId) const {
	// if the list already had the specified actorId keep it, otherwise, remove it,
	// as it might get added by the other functions.
	const bool hadInitialActorInitially = result.count(actorId) != 0;

	ObjectId topMostParent = actorId;
	while (true) {
		ObjectId potentialParent = getParentId(topMostParent);
		if (potentialParent.isNull())
			break;

		topMostParent = potentialParent;
	}

	if (topMostParent.isNull() == false) {
		getAllChildren(result, topMostParent);
		result.add(topMostParent);
	}

	if (hadInitialActorInitially == false) {
		result.eraseKey(actorId);
	}
}

void sge::GameWorld::instantiatePrefab(const char* prefabPath, bool createHistory, bool shouldGenerateNewObjectIds) {
	GameWorld prefabWorld;
	if (loadGameWorldFromFile(&prefabWorld, prefabPath)) {
		instantiatePrefab(prefabWorld, createHistory, shouldGenerateNewObjectIds, nullptr);
	} else {
		sgeAssert(false);
	}
}

/// Instantients the specified world into the current world.
/// @param [in] prefabPath a path the world file to be instantiated.
/// @param [in] createHistory pass true if the changes should be added to undo/redo history.
void GameWorld::instantiatePrefabFromJsonString(const char* prefabJson, bool createHistory, bool shouldGenerateNewObjectIds) {
	GameWorld prefabWorld;
	bool succeeded = loadGameWorldFromString(&prefabWorld, prefabJson);

	if (succeeded) {
		instantiatePrefab(prefabWorld, createHistory, shouldGenerateNewObjectIds, nullptr);
	} else {
		sgeAssert(false);
	}
}

void sge::GameWorld::instantiatePrefab(const GameWorld& prefabWorld,
                                       bool createHistory,
                                       bool shouldGenerateNewObjectIds,
                                       const vector_set<ObjectId>* const pOblectsToInstantiate,
                                       vector_set<ObjectId>* const newObjectIds) {
	std::vector<GameObject*> createdObjects;
	std::unordered_map<ObjectId, ObjectId> oldToNew;
	std::unordered_map<ObjectId, ObjectId> oldParentOf;

	const auto shouldInstantiateObject = [&](const ObjectId objectId) -> bool {
		if (pOblectsToInstantiate == nullptr) {
			return true;
		}

		return pOblectsToInstantiate->count(objectId) != 0;
	};

	auto processActor = [&](GameObject* prefabObject) -> void {
		if (shouldInstantiateObject(prefabObject->getId()) == false) {
			return;
		}

		// FIXME:
		// Kind of a lazy solution here...
		// To instantiate the entities from a prefab we serialize the entity we've just loaded in the prefabWorld
		// and then deserialize it back in the current world, while creating a new entity id for it.
		std::string serialziedPrefabObject = serializeObject(prefabObject);
		ObjectId originalId;
		GameObject* const newObject =
		    deserializeObjectFromJson(this, serialziedPrefabObject.c_str(), shouldGenerateNewObjectIds, &originalId);

		// Fill the output list of all created object ids
		if (newObjectIds) {
			newObjectIds->add(newObject->getId());
		}

		if_checked(newObject) {
			sgeAssert(originalId == prefabObject->getId());
			oldToNew[originalId] = newObject->getId();

			if (shouldGenerateNewObjectIds == false) {
				sgeAssert(prefabObject->getId() == newObject->getId());
			}

			oldParentOf[newObject->getId()] = prefabWorld.getParentId(originalId);
		}

		createdObjects.push_back(newObject);
	};

	for (auto& playingActorsPerType : prefabWorld.playingObjects) {
		for (GameObject* const prefabObject : playingActorsPerType.second) {
			processActor(prefabObject);
		}
	}

	for (GameObject* const prefabObject : prefabWorld.awaitsCreationObjects) {
		processActor(prefabObject);
	}

	// Fix the object relationships stored as members in the entites themselves and also fix the
	// object hierarchy.
	for (GameObject* newObjectFromPrefab : createdObjects) {
		MemberChain chain;

		const TypeDesc* const typeDesc = typeLib().find(newObjectFromPrefab->getType());
		if (typeDesc == nullptr) {
			sgeAssert(false);
			continue;
		}

		// Fix the relationships between objects in their members.
		for (const MemberDesc& mfd : typeDesc->members) {
			std::function<void(void*, const MemberChain&)> lambda = [&](void* root, const MemberChain& chain) -> void {
				const TypeDesc* const typeDesc = chain.getType();
				if (!typeDesc) {
					return;
				}

				if (typeDesc->typeId == sgeTypeId(ObjectId)) {
					ObjectId& idToReplace = *(ObjectId*)chain.follow(root);

					const auto itr = oldToNew.find(idToReplace);
					if (itr != std::end(oldToNew)) {
						const ObjectId& newValue = itr->second;
						idToReplace = newValue;
					}
				}
			};

			if ((mfd.flags & MFF_PrefabDontCopy) == 0) {
				MemberChain chainMember;
				chainMember.add(&mfd);
				chainMember.forEachMember(newObjectFromPrefab, lambda);
			}
		}

		// Fix the object hieirarchy, as it is not stored in the game objects themselves.
		// Resolve the object hierarchy.
		for (GameObject* const newObject : createdObjects) {
			ObjectId originalParent = oldParentOf[newObject->getId()];
			if (originalParent.isNull() == false) {
				// Find the new actor that represents the parent.
				const auto itrNewParent = oldToNew.find(originalParent);
				if (itrNewParent != oldToNew.end()) {
					setParentOf(newObject->getId(), itrNewParent->second);
				}
			}
		}

		if (inspector != nullptr) {
			inspector->deselectAll();
			for (GameObject* const newObject : createdObjects) {
				inspector->select(newObject->getId());
			}
		}
	}

	if (createHistory && inspector != nullptr && createdObjects.empty() == false) {
		vector_set<ObjectId> createdObjectIds;

		for (const GameObject* const object : createdObjects) {
			sgeAssert(object != nullptr);
			if (object) {
				createdObjectIds.insert(object->getId());
			}
		}

		CmdExistingObjectCreation* const cmdObjsCreated = new CmdExistingObjectCreation;
		cmdObjsCreated->setup(*this, createdObjectIds);
		inspector->appendCommand(cmdObjsCreated, false);
	}
}

void GameWorld::createPrefab(GameWorld& prefabWorld,
                             bool shouldKeepOriginalObjectIds,
                             const vector_set<ObjectId>* const pOblectsToInstantiate) const {
	prefabWorld.clear();
	prefabWorld.create();
	prefabWorld.instantiatePrefab(*this, false, !shouldKeepOriginalObjectIds, pOblectsToInstantiate);
}

void GameWorld::removeRigidBodyManifold(RigidBody* const rb) {
	auto itrFind = m_physicsManifoldList.find(rb);
	if (itrFind == m_physicsManifoldList.end()) {
		return;
	}

	// Remove all manifolds where we participate. Including in other actor manifold list.
	btCollisionObject* coToRemove = rb->getBulletRigidBody();
	for (const btPersistentManifold* manifold : itrFind->second) {
		// Get the other collision object, which rb has contacted, go to its manifolds and remove all manifolds with rb.
		const btCollisionObject* otherCollsionObject = getOtherFromManifold(manifold, coToRemove);
		const RigidBody* otherRigidBody = fromBullet(otherCollsionObject);
		if (otherRigidBody) {
			auto otherRBManifolds = m_physicsManifoldList.find(otherRigidBody);
			if (otherRBManifolds != m_physicsManifoldList.end()) {
				for (int iOtherManifold = 0; iOtherManifold < otherRBManifolds->second.size(); ++iOtherManifold) {
					const btCollisionObject* probablyRBCOToDelete =
					    getOtherFromManifold(otherRBManifolds->second[iOtherManifold], otherCollsionObject);

					if (probablyRBCOToDelete == coToRemove) {
						// Remove the current manifold.
						otherRBManifolds->second.erase(otherRBManifolds->second.begin() + iOtherManifold);
						--iOtherManifold;
					}
				}
			}
		}
	}

	// Finally remove the manifolds for the specified rigid body.
	m_physicsManifoldList.erase(itrFind);
}

void GameWorld::addPostSceneTask(IPostSceneUpdateTask* const task) {
	if (task) {
		m_postSceneUpdateTasks.emplace_back(task);
	} else {
		sgeAssert(false);
	}
}

void GameWorld::addPostSceneTaskLoadWorldFormFile(const char* filename) {
	PostSceneUpdateTaskLoadWorldFormFile* task = new PostSceneUpdateTaskLoadWorldFormFile(filename, true);
	addPostSceneTask(task);
}

void GameWorld::setDefaultGravity(const vec3f& gravity) {
	physicsWorld.dynamicsWorld->setGravity(toBullet(gravity));
	m_defaultGravity = gravity;
}

ICamera* GameWorld::getRenderCamera() {
	if (m_useEditorCamera) {
		return &m_editorCamera;
	}

	TraitCamera* traitCamera = getTrait<TraitCamera>(getActorById(m_cameraPovider));

	// If this object cannot provide us a camera, search for the 1st one that can.
	if (traitCamera == nullptr) {
		iterateOverPlayingObjects(
		    [&](GameObject* object) -> bool {
			    traitCamera = getTrait<TraitCamera>(object);
			    if (traitCamera != nullptr) {
				    m_cameraPovider = object->getId();
				    return false;
			    }

			    return true;
		    },
		    false);
	}

	ICamera* const camera = traitCamera ? traitCamera->getCamera() : nullptr;

	// Fallback to the editor camera.
	if (!camera) {
		return &m_editorCamera;
	}

	return camera;
}

} // namespace sge
