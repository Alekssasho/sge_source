#pragma once

#include "sge_core/sgecore_api.h"

#include "Sizing.h"
#include <memory>
#include <vector>

namespace sge::gamegui {

struct IWidget;
struct InvisibleWidget;

//----------------------------------------------------
// ILayout
//----------------------------------------------------
struct SGE_CORE_API ILayout {
	ILayout(std::weak_ptr<IWidget> ownerWidget);
	virtual ~ILayout() = default;

	IWidget* getOwner() { return m_owner.lock().get(); }

	virtual void update() {}

  private:
	/// The widget which contains this layout
	std::weak_ptr<IWidget> const m_owner;
};

//----------------------------------------------------
// ColumnLayout
//----------------------------------------------------
struct SGE_CORE_API ColumnLayout : public ILayout {
	ColumnLayout(std::weak_ptr<IWidget> ownerWidget, Pos pos, Size size);

	void addWidget(std::shared_ptr<IWidget> widget);
	void update() override;

  private:
	Pos m_pos;
	Size m_size;
	Unit m_spacing;

	std::vector<std::shared_ptr<IWidget>> m_widgets;
	std::shared_ptr<InvisibleWidget> m_columnContainer;
};

} // namespace sge::gamegui
