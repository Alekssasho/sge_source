#pragma once

#include "sge_engine/sge_engine_api.h"
#include "sge_utils/math/common.h"
#include "sge_utils/math/vec4.h"

namespace sge {

struct Actor;
struct GameUpdateSets;
struct RigidBody;

/// @brief CharacterCtrlCfg describes how the character control should move, its speed, jump height and other parameters.
struct SGE_ENGINE_API CharacterCtrlCfg {
	// Everything below this height is concidered feet. Used to detect if the player is grounded. This is in object space.
	float feetLevel = 0.1f;

	// Walk speed in units per second. This is in object space.
	float walkSpeed = 8.87f;

	// Cosine of the climbable slope angle.
	float minClimbableIncline = cosf(deg2rad(60.f));

	float gravity = 1.f;         // The size of the gravity force.
	float fallingGravity = 1.f;  // The size of gravity when falling.
	float maxJumpVelocity = 0.f; // The velocity to be applied one, in order to reach the high point of the jump.
	float minJumpVelocity = 0.f; // The velocity needed  to be applied if the player released the jump button too early.

	/// @brief By specified jump height and time needed to rach the peak of that jump,
	/// this function computes the jump force and the gravity to be applied to the rigid body
	/// needed to have the desiered jump.
	/// @param jumpHeight is the maximum height of the jump.
	/// @param jumpTimeApex is the time needed to the character to reach @jumpHeight.
	/// @param minJumpHeight The jump usually increases as the player hold the jump button. However if the player
	///                      Release the jump button too early the character might not appear to be jumping. This value forces
	///                      The character to continue to move upwards for the specified (usually small) distance,
	///                      even if the jump button is released.
	/// @param fallingGravityMultiplier If we want the gravity to be stronger when the character is falling (a commonly desiered effect)
	///                                 set this to be above 1, otherwise use 1 to have the same falling speed.
	void computeJumpParams(float jumpHeight, float jumpTimeApex, float minJumpHeight, float fallingGravityMultiplier) {
		gravity = 2.f * jumpHeight / (jumpTimeApex * jumpTimeApex);
		fallingGravity = gravity * fallingGravityMultiplier;
		maxJumpVelocity = gravity * jumpTimeApex;
		minJumpVelocity = sqrtf(2.f * gravity * minJumpHeight);
	}
};

/// @brief Hold the result after a single update of the rigid body.
struct SGE_ENGINE_API CharaterCtrlOutcome {
	bool isLogicallyGrounded = false;
	bool isGroundSlopeClimbable = false;
	bool didJustJumped = false;
	vec3f wallNormal = vec3f(0.f);
	vec3f facingDir = vec3f(1.f, 0.f, 0.f);
};

struct SGE_ENGINE_API CharacterCtrlInput {
	
	/// A shortcut for initializing the strcture for AI input (as the class is used for AI and player character controllers).
	static CharacterCtrlInput aiInput(const vec3f& facingDir, const vec3f& walkDir) {
		CharacterCtrlInput in;
		in.facingDir = facingDir;
		in.walkDir = walkDir;
		return in;
	}

	vec3f facingDir = vec3f(0.f); /// The forward direction of the player in world space.
	vec3f walkDir = vec3f(0.f);   /// Is the input direction of that the player specified this update.
	bool isJumpButtonPressed = false;
	bool isJumpBtnReleased = false;
};

/// @brief A character controller that takes an Actor and its parameter
/// add add forces to its rigid body for the rigid body should behave as a character
/// it is expected that the the angular velocity of the rigid body is already set to 0 (to avoid tippling of the rigid body).
struct SGE_ENGINE_API CharacterCtrl {

	/// @brief Call this function to make the rigid body of the actor ot appear as a character.
	/// Use the return value to change the facing direction of the character).
	CharaterCtrlOutcome update(const GameUpdateSets& updateSets, const CharacterCtrlInput& input);

	bool isJumping() const { return m_jumpCounter > 0; }
	

  public:
	Actor* m_actor = nullptr;
	CharacterCtrlCfg m_cfg;

	// Since this character controller always modifies its velocity, when other objects
	// want the character to move in a certain way, they will modify this value.
	// For example this could be conveyor belts or jump pads.
	vec3f gamplayAppliedLinearVelocity = vec3f(0.f);

	vec3f m_lastNonZeroWalkDir = vec3f(0.f);
	bool m_wasGroundSlopeClimbable = false;
	int m_jumpCounter = 0;
	vec3f m_targetFacingDir = vec3f(1.f, 0.f, 0.f);
	vec3f m_walkDirSmoothAccumulator = vec3f(1.f, 0.f, 0.f);
	float m_timeInAir = 0.f;
	float m_jumpingHorizontalVelocity = 0.f;
};

} // namespace sge
