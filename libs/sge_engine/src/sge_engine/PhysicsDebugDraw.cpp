#include "PhysicsDebugDraw.h"
#include "sge_core/ICore.h"
#include "sge_core/QuickDraw.h"

namespace sge {

void BulletPhysicsDebugDraw::preDebugDraw(const mat4f& projView, QuickDraw* const debugDraw, const RenderDestination& rdest) {
	m_projView = projView;
	m_debugDraw = debugDraw;

	sgeAssert(m_debugDraw != nullptr);
	if (m_debugDraw) {
		m_debugDraw->drawWired_Clear();
		m_debugDraw->changeRenderDest(rdest.sgecon, rdest.frameTarget, rdest.viewport);
	}
}

void BulletPhysicsDebugDraw::postDebugDraw() {
	if (!m_debugDraw) {
		sgeAssert(m_debugDraw);
		return;
	}

	m_debugDraw->drawWired_Execute(m_projView);
}

void BulletPhysicsDebugDraw::reportErrorWarning([[maybe_unused]] const char* warningString) {
	SGE_DEBUG_WAR("[PS][BT]%s\n", warningString);
}

void BulletPhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	if (m_debugDraw) {
		int rgba = 0;
		rgba |= clamp<int>((int)(color.x() * 255.f), 0, 255);
		rgba |= clamp<int>((int)(color.y() * 255.f), 0, 255) << 8;
		rgba |= clamp<int>((int)(color.z() * 255.f), 0, 255) << 16;
		rgba |= 255 << 24;

		m_debugDraw->drawWiredAdd_Line(fromBullet(from), fromBullet(to), rgba);
	}
}

} // namespace sge
