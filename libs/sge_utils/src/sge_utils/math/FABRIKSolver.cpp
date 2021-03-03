#include "FABRIKSolver.h"
#include "primitives.h"
#include "quat.h"
#include "vec3.h"

namespace sge {

void FABRIKSolver(const int numPoints,
                  vec3f inoutPoints[],
                  const float linkLengths[],
                  const vec3f& endAffector,
                  const vec3f* pPole,
                  int maxIterations,
                  float earlyExitDelta) {
	sgeAssert(numPoints >= 2);
	sgeAssert(linkLengths != nullptr);
	maxIterations = maxOf(1, maxIterations);

	const vec3f startPosWs = inoutPoints[0];

	for (int iIteration = 0; iIteration < maxIterations; ++iIteration) {
		if (distance(startPosWs, endAffector) <= earlyExitDelta) {
			break;
		}

		// backwards step.
		{
			inoutPoints[numPoints - 1] = endAffector;
			for (int iPoint = numPoints - 1; iPoint >= 1; --iPoint) {
				const vec3f staticPoint = inoutPoints[iPoint];
				vec3f& pointToMove = inoutPoints[iPoint - 1];

				const float linkLength = linkLengths[iPoint - 1];
				vec3f diff = pointToMove - staticPoint;
				pointToMove = diff.normalized0() * linkLength + staticPoint;
			}
		}

		// forwards step.
		{
			inoutPoints[0] = startPosWs;
			for (int iPoint = 1; iPoint < numPoints; ++iPoint) {
				const vec3f staticPoint = inoutPoints[iPoint - 1];
				vec3f& pointToMove = inoutPoints[iPoint];

				const float linkLength = linkLengths[iPoint - 1];
				vec3f diff = pointToMove - staticPoint;
				pointToMove = diff.normalized0() * linkLength + staticPoint;
			}
		}
	}

	if (pPole != nullptr) {
		vec3f pole = *pPole;
		for (int t = 1; t < numPoints - 1; ++t) {
			vec3f planeNormal = (inoutPoints[t + 1] - inoutPoints[t - 1]).normalized0();
			if (planeNormal.lengthSqr() < 1e-6f) {
				continue;
			}

			vec3f planePosition = inoutPoints[t - 1];

			const Plane p = Plane::FromPosAndDir(planePosition, planeNormal);

			vec3f poleOnPlane = (p.Project(pole) - planePosition).normalized0();
			vec3f pointOnPlane = (p.Project(inoutPoints[t]) - planePosition).normalized0();

			float dotProd = dot(poleOnPlane, pointOnPlane);
			float angle = acosf(clamp(dotProd, -1.f, 1.f));
			vec3f crossProd = cross(pointOnPlane, poleOnPlane);

			if (crossProd.lengthSqr() < 1e-5f && isEpsEqual(dotProd, 0.f)) {
				continue;
			}

			if (crossProd.dot(planeNormal) < 0) {
				angle = -angle;
			}

			vec3f pt = quat_mul_pos(quatf::getAxisAngle(planeNormal, angle), (inoutPoints[t] - planePosition)) + planePosition;
			inoutPoints[t] = pt;
		}
	}
}

} // namespace sge
