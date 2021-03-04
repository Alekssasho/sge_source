#pragma once

#include "AssetProperty.h"
#include "Physics.h"

namespace sge {

struct Actor;
struct GameObject;
struct GameInspector;
struct MemberChain;

/// @brief Provides a strcture that can configure the properties of a TraitRigidBody.
/// It is intended to have this as a member in an Actor (however it is not limited to that used) to
/// provide user editable values in the PropertyEditorWindow.
/// Useful when we want to provide the user with the option to change the properties of a rigid body, but not its shape.
struct SGE_ENGINE_API RigidBodyPropertiesConfigurator {
	RigidBodyPropertiesConfigurator() = default;
	~RigidBodyPropertiesConfigurator() = default;

	float mass = 1.f;
	float friction = 1.f; // TODO: Should we add spinning and rolling friction?
	float rollingFriction = 0.5f;
	float spinningFriction = 0.5f;
	float bounciness = 0.f;
	bool noMoveX = false;
	bool noMoveY = false;
	bool noMoveZ = false;
	bool noRotationX = false;
	bool noRotationY = false;
	bool noRotationZ = false;
	float movementDamping = 0.f;
	float rotationDamping = 0.f;

	/// @brief If true the editor will not show propeties that affect dynamics (mass, movement restriction, damping and so on).
	/// The values can still be changed manually via code.
	/// Useful when we know that the actor we've attached this struct to is going to have a static rigid body and we do not want
	/// to enable the user to change that without code.
	bool dontShowDynamicProperties = false;

  public:
	void applyProperties(Actor& actor) const;
	void applyProperties(RigidBody& rb) const;

	void extractPropsFromRigidBody(const RigidBody& rb);
};

/// @brief Provides a structure that can configure a TraitRigidBody - this can configure
/// The properties of the rigid body (mass, friction etc.) and the collision shape as well.
/// It is intended to have this as a member in an Actor (however it is not limited to that used) to
/// provide user editable values in the PropertyEditorWindow.
/// In order to embed this class properly make sure that the @apply method is called (when the Actor play state changes).
struct SGE_ENGINE_API RigidBodyConfigurator : public RigidBodyPropertiesConfigurator {
	RigidBodyConfigurator() = default;
	~RigidBodyConfigurator() = default;

	enum ShapeSource {
		/// @brief The attached model to the specified actor should be used to extract the collision shape.
		shapeSource_fromTraitModel,
		/// @brief The user specifies a list of shapes to be used.
		shapeSource_manuallySpecify,
	};

	/// @brief Updates the rigid body to match the specified object.
	/// @return true if the rigid body has been updated.
	bool apply(Actor& actor, bool addToWorldNow = false) const;


  public:
	ShapeSource shapeSource = shapeSource_fromTraitModel;
	AssetProperty assetPropery = AssetProperty(AssetType::Model);
	std::vector<CollsionShapeDesc> collisionShapes;
};

SGE_ENGINE_API void edit_CollisionShapeDesc(GameInspector& inspector, GameObject* gameObject, MemberChain chain);
SGE_ENGINE_API void edit_RigidBodyPropertiesConfigurator(GameInspector& inspector, GameObject* gameObject, MemberChain chain);
SGE_ENGINE_API void edit_RigidBodyConfigurator(GameInspector& inspector, GameObject* gameObject, MemberChain chain);

} // namespace sge
