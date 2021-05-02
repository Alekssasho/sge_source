#include "sge_engine/Actor.h"
#include "sge_engine/GameWorld.h"
#include "sge_engine/traits/TraitCharacterController.h"
#include "sge_engine/traits/TraitModel.h"
#include "sge_engine/traits/TraitRigidBody.h"
#include "sge_engine/typelibHelper.h"

namespace sge {

struct CharacterActor : public Actor {
	TraitRigidBody ttRigidbody;
	TraitModel ttModel;
	TraitCharacterController ttCharacter;

	virtual AABox3f getBBoxOS() const { return AABox3f(); }

	void create() {
		registerTrait(ttRigidbody);
		registerTrait(ttModel);
		registerTrait(ttCharacter);

		ttModel.setModel("assets/editor/models/cylinder.mdl", true);
		ttRigidbody.getRigidBody()->create(this, CollsionShapeDesc::createCapsule(0.6f, 0.2f, transf3d(vec3f(0.f, 0.5f, 0.f))), 1.f, false);
		ttRigidbody.getRigidBody()->setCanRotate(false, false, false);

		CharacterCtrlCfg cfg;
		cfg.feetLevel = 0.2f;
		cfg.walkSpeed = 8.f;
		cfg.computeJumpParams(2.f, 0.25f, 1.f, 1.5f);

		ttCharacter.getCharCtrl().m_actor = this;
		ttCharacter.getCharCtrl().m_cfg = cfg;
	}

	void update(const GameUpdateSets& u) {
		if (u.isGamePaused()) {
			return;
		}

		// Obtain the input
		const GamepadState* const gamepad = u.is.getHookedGemepad(0);

		vec3f inputDirWS = vec3f(0.f); // The normalized input direction in world space.
		{
			vec3f inputDir = vec3f(0.f);

			// Keyboard input dir.
			if (u.is.wasActiveWhilePolling() && u.is.AnyArrowKeyDown(true)) {
				inputDir = vec3f(u.is.GetArrowKeysDir(true, true), 0);
				inputDir.z = -inputDir.y;
				inputDir.y = 0.f;
			}



			// Use gamepad input dif in no keyboard input was used.
			if (inputDir == vec3f(0.f) && gamepad) {
				inputDir = vec3f(gamepad->getInputDir(true), 0.f);
				inputDir.z = -inputDir.y; // Remap from gamepad space to camera-ish space.
				inputDir.y = 0.f;
			}

			// Correct the input so it is aligned with the camera.
			inputDirWS = inputDir;
		}

		bool const isJumpBtnPressed = (u.is.wasActiveWhilePolling() && u.is.IsKeyPressed(Key::Key_Space));

		bool const isJumpBtnReleased = (u.is.wasActiveWhilePolling() && u.is.IsKeyReleased(Key::Key_Space));

		// Update the character controller.
		CharacterCtrlInput charInput;
		charInput.facingDir = getTransformMtx().c0.xyz().normalized0();
		charInput.walkDir = inputDirWS;
		charInput.isJumpButtonPressed = isJumpBtnPressed;
		charInput.isJumpBtnReleased = isJumpBtnReleased;

		const CharaterCtrlOutcome charOutcome = ttCharacter.getCharCtrl().update(u, charInput);

		// Update the look at direction of the character, remember we aren't rotating it's rigid body, just the visual transform.
		if (charOutcome.facingDir.length() > 1e-3f) {
			vec3f lookDir = normalized(charOutcome.facingDir);

			transf3d newOrientedTForm = getTransform();
			newOrientedTForm.r = quatf::getAxisAngle(vec3f::getAxis(1), atan2(-lookDir.z, lookDir.x));

			setTransform(newOrientedTForm, false);
		}


		// Add your logic here.
	}

	void postUpdate(const GameUpdateSets& u) {
		if (u.isGamePaused()) {
			return;
		}

		// Add your logic here.
	}
};

DefineTypeId(CharacterActor, 20'03'21'0001);
ReflBlock() {
	ReflAddActor(CharacterActor);
}

} // namespace sge
