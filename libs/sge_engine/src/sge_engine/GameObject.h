#pragma once

#include "TypeRegister.h"
#include "sge_engine/typelibHelper.h"
#include "sge_engine_api.h"
#include "sge_utils/utils/basetypes.h"
#include "sge_utils/utils/vector_map.h"
#include <string>

namespace sge {

struct GameWorld;
struct GameInspector;
struct GameUpdateSets;

struct GameObject;
struct Trait;

enum EditMode : signed char {
	editMode_actors = 0,
	editMode_points,

	editMode_count
};

using ObjectAEMemberFilterFn = bool (*)(GameObject* actor, const MemberDesc& mdf, void* pValueToFIlter);

//--------------------------------------------------------------------
// ObjectId
//--------------------------------------------------------------------

// clang-format on
struct ObjectId {
	// Caution:
	// a std::hash implmentation below! Find it if you add any members.
	int id;

	explicit ObjectId(int const id = 0)
	    : id(id) {}
	bool isNull() const { return id == 0; }

	bool operator<(const ObjectId& r) const { return id < r.id; }
	bool operator>(const ObjectId& r) const { return id > r.id; }
	bool operator==(const ObjectId& r) const { return id == r.id; }
	bool operator!=(const ObjectId& r) const { return id != r.id; }
};

DefineTypeIdInline(ObjectId, 20'03'06'0005);

// struct ObjectIdHint {
//	ObjectId id;
//	TypeId type = nullptr;
//	int index = -1;
//
//	ObjectIdHint() = default;
//	ObjectIdHint(ObjectId id, TypeId type, int index)
//	    : id(id)
//	    , type(type)
//	    , index(index) {
//	}
//
//	bool isNull() const {
//		return id.isNull();
//	}
//
//	bool hasValidHint() {
//		return type != nullptr && index >= 0;
//	}
//
//	bool operator<(const ObjectIdHint& r) const {
//		return id.id < r.id.id;
//	}
//	bool operator>(const ObjectIdHint& r) const {
//		return id.id > r.id.id;
//	}
//	bool operator==(const ObjectIdHint& r) const {
//		return id.id == r.id.id;
//	}
//	bool operator!=(const ObjectIdHint& r) const {
//		return id.id != r.id.id;
//	}
//};

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
struct Actor;

struct SGE_ENGINE_API GameObject : public NoncopyableMovable {
	GameObject() = default;
	virtual ~GameObject() = default;

	void worldInitializeMe(GameWorld* const world, const ObjectId id, const TypeId typeId, std::string displayName) {
		m_id = id;
		m_type = typeId;
		m_world = world;
		m_displayName = std::move(displayName);
	}

	bool isActor() const;
	Actor* getActor();
	const Actor* getActor() const;

	ObjectId getId() const { return m_id; }
	TypeId getType() const { return m_type; }
	const std::string& getDisplayName() const { return m_displayName; }
	const char* getDisplayNameCStr() const { return m_displayName.c_str(); }
	void setDisplayName(std::string name) { m_displayName = std::move(name); }
	GameWorld* getWorld() { return m_world; }
	const GameWorld* getWorld() const { return m_world; }
	GameWorld* getWorldMutable() const { return m_world; }

	// Composes a debug display name that contains the display name + the id of the object (as the names aren't unique).
	void composeDebugDisplayName(std::string& result) const;

	// Called when the object has to be created.
	virtual void create() = 0;

	// Regular update, the player may only read and partially modify itself.
	// However this shouldn't break other game object that rely on this one.
	// For example during update() the object shouldn't delete it's rigid bodies
	// as other object may rely on the contact manifolds with that object.
	// use the postUpdate() for such manipulations.
	virtual void update(const GameUpdateSets& UNUSED(updateSets)) {}

	// This is the place where the object can freely modify themselves,
	// you cannot rely on other objects, as they can modify themselves here.
	virtual void postUpdate(const GameUpdateSets& UNUSED(updateSets)) {}

	/// Called when the object enters or leaves the game.
	/// If you override this, make sure you've called this method as well.
	virtual void onPlayStateChanged(bool const isStartingToPlay);

	// Called when THIS object was create by duplicating another.
	// Called at the end of the duplication process.
	virtual void onDuplocationComplete() {}

	// Called when a member of the object has been changed.
	virtual void onMemberChanged() {}

	// This method just registers the pointer to the trait, it is only a book keeping.
	void registerTrait(Trait& trait);

	Trait* findTrait(const TypeId family) {
		Trait** ppTrait = m_traits.find_element(family);
		return ppTrait ? *ppTrait : nullptr;
	}

	const Trait* findTrait(const TypeId family) const {
		Trait* const* ppTrait = m_traits.find_element(family);
		return ppTrait ? *ppTrait : nullptr;
	}

	int getDirtyIndex() const { return m_dirtyIndex; }

	void makeDirtyExternal() { makeDirty(); }

	virtual ObjectAEMemberFilterFn getAEMemberFIlterFn(const MemberDesc& UNUSED(mfd)) { return nullptr; }

  protected:
	void makeDirty() { m_dirtyIndex++; }

  public: // TODO: private
	ObjectId m_id;
	TypeId m_type; // the QuickTypeId of the inherited class.
	int m_dirtyIndex = 0;

	std::string m_displayName;
	GameWorld* m_world = nullptr;

	vector_map<TypeId, Trait*, false> m_traits;
};

//--------------------------------------------------------------------
// TODO: Find a better place for these.
//--------------------------------------------------------------------
struct SelectedItem {
	SelectedItem() = default;

	SelectedItem(EditMode const editMode, const ObjectId& objectId, int const index)
	    : editMode(editMode)
	    , objectId(objectId)
	    , index(index) {}

	explicit SelectedItem(const ObjectId& objectId)
	    : editMode(editMode_actors)
	    , objectId(objectId) {}

	bool operator==(const SelectedItem& ref) const { return objectId == ref.objectId && index == ref.index && editMode == ref.editMode; }

	bool operator<(const SelectedItem& ref) const { return ref.objectId > objectId || ref.index > index || ref.editMode > editMode; }

	EditMode editMode = editMode_actors; // The mode the item was selected.
	ObjectId objectId;
	int index = 0; // The index of the item. Depends on the edit mode.
};


//--------------------------------------------------------------------
// Trait
//--------------------------------------------------------------------
struct SGE_ENGINE_API Trait {
	Trait() = default;

	virtual ~Trait() { m_owner = nullptr; }

	Trait(const Trait&) = delete;
	const Trait& operator==(const Trait&) = delete;

	GameObject* getObject() {
		sgeAssert(m_owner != nullptr);
		return m_owner;
	}
	const GameObject* getObject() const {
		sgeAssert(m_owner != nullptr);
		return m_owner;
	}

	GameObject* object() {
		sgeAssert(m_owner != nullptr);
		return m_owner;
	}
	const GameObject* object() const {
		sgeAssert(m_owner != nullptr);
		return m_owner;
	}

	Actor* getActor();
	const Actor* getActor() const;

	template <typename T>
	T* getObjectT() {
		GameObject* const object = getObject();

		if (!object || object->getType() != sgeTypeId(T))
			return nullptr;

		return static_cast<T*>(object);
	}

	GameWorld* getWorldFromObject() { return m_owner ? m_owner->getWorld() : nullptr; }
	const GameWorld* getWorldFromObject() const { return m_owner ? m_owner->getWorld() : nullptr; }

	// virtual TypeId getExactType() const = 0;
	virtual TypeId getFamily() const = 0;

	virtual void onRegister(GameObject* const owner) {
		sgeAssert(owner);
		m_owner = owner;
	}
	virtual void onPlayStateChanged(bool const UNUSED(isStartingToPlay)) {}

	Trait& operator=(const Trait&) { return *this; }

  public:
	GameObject* m_owner = nullptr;
};

#define SGE_TraitDecl_Base(BaseTrait) \
	typedef BaseTrait TraitFamily;    \
	TypeId getFamily() const final { return sgeTypeId(BaseTrait); }

#define SGE_TraitDecl_Final(TraitSelf) \
	typedef TraitSelf TraitType;       \
	//TypeId getExactType() const final { return sgeTypeId(TraitSelf); } \

#define SGE_TraitDecl_Full(TraitSelf)                               \
	typedef TraitSelf TraitFamily;                                  \
	typedef TraitSelf TraitType;                                    \
	TypeId getFamily() const final { return sgeTypeId(TraitSelf); } \
	//TypeId getExactType() const final { return sgeTypeId(TraitSelf); } \

template <typename TTrait>
TTrait* getTrait(GameObject* const object) {
	if (!object) {
		return nullptr;
	}

	Trait* const trait = object->findTrait(sgeTypeId(typename TTrait::TraitFamily));
	if (!trait) {
		return nullptr;
	}

	return dynamic_cast<TTrait*>(trait);
}

template <typename TTrait>
const TTrait* getTrait(const GameObject* const object) {
	if (!object) {
		return nullptr;
	}

	const Trait* const trait = object->findTrait(sgeTypeId(typename TTrait::TraitFamily));
	if (!trait) {
		return nullptr;
	}

	return dynamic_cast<const TTrait*>(trait);
}

} // namespace sge

namespace std {
template <>
struct hash<sge::ObjectId> {
	size_t operator()(const sge::ObjectId& x) const { return std::hash<decltype(x.id)>{}(x.id); }
};
} // namespace std
