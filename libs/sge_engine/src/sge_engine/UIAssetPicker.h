#pragma once

#include <string>
#include <functional>

#include "sge_engine_api.h"
#include "sge_core/AssetLibrary.h"

namespace sge {

struct ObjectId;
struct GameWorld;
struct Actor;

SGE_ENGINE_API bool assetPicker(
    const char* label, char* const currentValue, int currentValueLen, AssetLibrary* const assetLibrary, AssetType const assetType);
SGE_ENGINE_API bool actorPicker(const char* label, GameWorld& world, ObjectId& ioValue, std::function<bool(const GameObject&)> filter = nullptr, bool pickPrimarySelection = false);
SGE_ENGINE_API bool gameObjectTypePicker(const char* label, TypeId& ioValue, const TypeId needsToInherit = TypeId());

} // namespace sge
