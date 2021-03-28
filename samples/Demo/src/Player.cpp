#include "sge_core/ICore.h"
#include "sge_engine/Actor.h"
#include "sge_engine/GameWorld.h"
#include "sge_engine/traits/TraitModel.h"
#include "sge_engine/traits/TraitRigidBody.h"
#include "sge_engine/typelibHelper.h"
#include "sge_utils/math/EulerAngles.h"

namespace sge {

struct Player : public Actor {
	TraitRigidBody ttRigidbody;
	TraitModel ttModel;

	ObjectId cameraObject;
	vec3f cameraOffset = vec3f(0.f, 2.f, 3.f);

	float yPositionRestart = -50.f;
	vec3f respawnPosition = vec3f(0.f);

	float wobbleAmplitude = 1.f;
	float wobbleForce = 0.f;
	float stretch = 0.f;
	vec3f prePos = vec3f(0.f);
	float wobbleTime = 0.f;

	virtual AABox3f getBBoxOS() const { return AABox3f(); }

	void create() {
		registerTrait(ttRigidbody);
		registerTrait(ttModel);

		ttModel.setModel("assets/models/catBall.mdl", true);
		ttRigidbody.getRigidBody()->create(this, CollsionShapeDesc::createSphere(1.f), 1.f, false);
		ttRigidbody.getRigidBody()->setRollingFriction(0.025f);
		ttRigidbody.getRigidBody()->setSpinningFriction(0.025f);
		ttRigidbody.getRigidBody()->setFriction(0.8f);
		ttRigidbody.getRigidBody()->setBounciness(0.9f);

		ttRigidbody.getRigidBody()->setDamping(0.05f, 0.005f);
	}

	void onPlayStateChanged(bool const isStartingToPlay) {
		Actor::onPlayStateChanged(isStartingToPlay);
		if (isStartingToPlay) {
			respawnPosition = this->getPosition();
			prePos = getPosition();
		}
	}

	void update(const GameUpdateSets& u) {
		if (u.isGamePaused()) {
			return;
		}

		getWorld()->needsLockedCursor = true;

		if (this->getPosition().y < yPositionRestart) {
			this->setPosition(respawnPosition);
			prePos = vec3f(0.f);
			wobbleAmplitude = 1.f;
			wobbleForce = 0.f;
		}

		const vec3f vel = getPosition() - prePos;
		prePos = getPosition();

		float timeScale = 2.f;
		wobbleAmplitude -= ttRigidbody.getRigidBody()->getLinearVel().x0z().length() * u.dt * 0.1f * timeScale;
		wobbleAmplitude -= ttRigidbody.getRigidBody()->getLinearVel().y * u.dt * 0.15f * timeScale;
		//wobbleAmplitude = clamp(wobbleAmplitude, 0.4f, 1.2f);

		Actor* const cameraActor = getWorld()->getActorById(cameraObject);

		vec3f wsForward = vec3f(0.f, 0.f, -1.f);
		vec3f wsRight = vec3f(1.f, 0.f, 0.f);

		if (cameraActor) {
			cameraOffset = quat_mul_pos(quatf::getAxisAngle(vec3f::axis_y(), -u.is.GetCursorMotion().x * deg2rad(1.f)), cameraOffset);

			transf3d cameraTransform = cameraActor->getTransform();
			cameraTransform.p = getTransform().p + cameraOffset;
			//cameraTransform.r = quatf::getAxisAngle(vec3f::axis_x(), -deg2rad(30.f)) * quatf::getAxisAngle(vec3f::axis_y(), deg2rad(90.f));
			cameraTransform.r = quatf::getAxisAngle(vec3f::axis_y(), -u.is.GetCursorMotion().x * deg2rad(1.f)) * cameraTransform.r;
			cameraActor->setTransform(cameraTransform);

			wsForward = cameraActor->getTransformMtx().c0.xyz().x0z().normalized0();
			wsRight = cameraActor->getTransformMtx().c2.xyz().x0z().normalized0();
		}

		wobbleForce += (0.85f - wobbleAmplitude) * 1.75f;
		wobbleForce -= wobbleForce * u.dt * timeScale ;
		wobbleAmplitude += wobbleForce * u.dt * 0.5f * timeScale;

		const vec2f inputDir = u.is.GetArrowKeysDir(true, true);
		const vec3f inputDirWs = wsRight * inputDir.x +  inputDir.y * wsForward;
		const vec3f inputDirWsRight = vec3f(-inputDirWs.z, 0.f, inputDirWs.x);

		vec3f additionalForce = vec3f(0.f);
		float dr = inputDirWsRight.dot(ttRigidbody.getRigidBody()->getLinearVel().x0z());
		float df = inputDirWs.dot(ttRigidbody.getRigidBody()->getLinearVel().x0z());
		additionalForce = -dr * inputDirWsRight * 100.f;
		if (df < 0.f) {
			additionalForce += -df * inputDirWs * 50.f;
		}

		ttRigidbody.getRigidBody()->applyForce((inputDirWs * 300.f + additionalForce) * u.dt, vec3f(0.f, 0.65f, 0.f));

		float squash = wobbleAmplitude; 

		ttModel.setAdditionalTransform(mat4f::getRotationQuat(getTransform().r.inverse()) * mat4f::getTranslation(0.f, -0.5f, 0.f) *
		                               mat4f::getSquashyScalingY(squash) * mat4f::getTranslation(0.f, 0.5f, 0.f) *
		                               mat4f::getRotationQuat(getTransform().r));
	}
};

DefineTypeId(Player, 30'02'22'0001);
ReflBlock() {
	ReflAddActor(Player) ReflMember(Player, cameraObject) ReflMember(Player, cameraOffset);
}

} // namespace sge
