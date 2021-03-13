#include "sge_engine/Actor.h"
#include "sge_engine/traits/TraitRigidBody.h"
#include "sge_engine/GameWorld.h"
#include "sge_engine/traits/TraitModel.h"
#include "sge_engine/typelibHelper.h"
#include "sge_utils/math/EulerAngles.h"

namespace sge {

struct Player : public Actor
{
	TraitRigidBody ttRigidbody;
	TraitModel ttModel;

	ObjectId cameraObject;
	vec3f cameraOffset = vec3f(0.f, 2.f, 3.f);

	virtual AABox3f getBBoxOS() const {
		return AABox3f();
	}

	void create() 
	{
		registerTrait(ttRigidbody);
		registerTrait(ttModel);

		ttModel.setModel("assets/models/playerBall/playerBall.mdl", true);
		ttRigidbody.getRigidBody()->create(this, CollsionShapeDesc::createSphere(1.f), 1.f, false);
		ttRigidbody.getRigidBody()->setRollingFriction(0.1f);
		ttRigidbody.getRigidBody()->setSpinningFriction(0.1f);
		ttRigidbody.getRigidBody()->setFriction(0.3f);  
		ttRigidbody.getRigidBody()->setBounciness(0.8f); 

		ttRigidbody.getRigidBody()->setDamping(0.05f, 0.005f);
	}

	void update(const GameUpdateSets& u) {

		if (u.isGamePaused()) { 
			return;  
		}

		Actor* const cameraActor = getWorld()->getActorById(cameraObject); 
		if(cameraActor) {
			transf3d cameraTransform;
			cameraTransform.p = getTransform().p + cameraOffset;
			cameraTransform.r =  quatf::getAxisAngle(vec3f::axis_x(), -deg2rad(30.f)) * quatf::getAxisAngle(vec3f::axis_y(), deg2rad(90.f));

			cameraActor->setTransform(cameraTransform); 
		}

		const vec2f inputDir = u.is.GetArrowKeysDir(true, false);
		const vec3f inputDirWs = vec3f(inputDir.x, 0.f, -inputDir.y);

		ttRigidbody.getRigidBody()->applyForce(inputDirWs * 700.f * u.dt, vec3f(0.f, 0.75f, 0.f));
	}

	
};

DefineTypeId(Player, 30'02'22'0001);
ReflBlock()
{
	ReflAddActor(Player)
		ReflMember(Player, cameraObject)
		ReflMember(Player, cameraOffset)
	;
}

}
