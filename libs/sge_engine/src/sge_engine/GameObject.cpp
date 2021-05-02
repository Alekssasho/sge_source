#include "sge_engine/TypeRegister.h"
#include "sge_utils/utils/strings.h"

#include "Actor.h"

namespace sge {

//--------------------------------------------------------------------
// struct Object
//--------------------------------------------------------------------

// clang-format off
DefineTypeId(GameObject, 20'03'01'0011);
DefineTypeId(std::vector<ObjectId>, 20'03'07'0012);

ReflBlock()
{
	ReflAddType(ObjectId)
		ReflMember(ObjectId, id);

	ReflAddType(std::vector<ObjectId>);

	ReflAddType(GameObject)
		ReflMember(GameObject, m_id)
			.addMemberFlag(MFF_PrefabDontCopy)
			.addMemberFlag(MFF_NonEditable) // CAUTION: the id is actually saveable, but it is handeled diferently!
		ReflMember(GameObject, m_displayName)
			.addMemberFlag(MFF_PrefabDontCopy)
	;
}
// clang-format on

bool GameObject::isActor() const {
	return dynamic_cast<const Actor*>(this) != nullptr;
}

Actor* GameObject::getActor() {
	return dynamic_cast<Actor*>(this);
}

const Actor* GameObject::getActor() const {
	return dynamic_cast<const Actor*>(this);
}

void GameObject::composeDebugDisplayName(std::string& result) const {
	string_format(result, "%s[id=%d]", m_displayName.c_str(), m_id.id);
}

void GameObject::onPlayStateChanged(bool const isStartingToPlay) {
	for (auto itr : m_traits) {
		if_checked(itr.value()) { itr.value()->onPlayStateChanged(isStartingToPlay); }
	}
}

void GameObject::registerTrait(Trait& trait) {
	Trait* const pTrait = &trait;
	TypeId const family = pTrait->getFamily();

	// There shouldn't be a trait with the same family registered.
	if_checked(m_traits.find_element(family) == nullptr) {
		m_traits[family] = pTrait;
		pTrait->onRegister(this);
	}
}

//--------------------------------------------------------------------
// Trait
//--------------------------------------------------------------------
Actor* Trait::getActor() {
	Actor* actor = dynamic_cast<Actor*>(getObject());
	return actor;
}

const Actor* Trait::getActor() const {
	const Actor* actor = dynamic_cast<const Actor*>(getObject());
	return actor;
}

} // namespace sge