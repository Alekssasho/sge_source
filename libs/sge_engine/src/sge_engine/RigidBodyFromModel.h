#pragma once

#include "sge_engine_api.h"
#include <vector>

namespace sge {

struct EvaluatedModel;
struct CollsionShapeDesc;

SGE_ENGINE_API bool initializeCollisionShapeBasedOnModel(std::vector<CollsionShapeDesc>& shapeDescs, const EvaluatedModel& evaluatedMode);
SGE_ENGINE_API bool initializeCollisionShapeBasedOnModel(std::vector<CollsionShapeDesc>& shapeDescs, const char* modelAssetPath);

} // namespace sge
