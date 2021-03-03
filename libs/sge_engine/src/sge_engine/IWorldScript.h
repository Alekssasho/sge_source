#pragma once

#include "ScriptObject.h"

namespace sge {

struct GameUpdateSets;
struct GameDrawSets;

/// @brief An interface for getting getting callbacks on world events.
/// The object needs to be attached to the world in order to work.
struct IWorldScript : public Script {
	IWorldScript() = default;
	~IWorldScript() = default;

	/// @brief Called before the world starts update all game objects.
	virtual void onPreUpdate(const GameUpdateSets& u) = 0;
	/// @brief Called after all game object have been updated.
	virtual void onPostUpdate(const GameUpdateSets& u) = 0;
	/// @brief Called when rendering the world has finished.
	virtual void onPostDraw(const GameDrawSets& drawSets) = 0;
};

} // namespace sge
