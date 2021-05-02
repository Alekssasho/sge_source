#include "DummyPlugin.h"
#include "sge_engine/DefaultGameDrawer.h"

namespace sge {
IGameDrawer* DummyPlugin::allocateGameDrawer() {
	return new DefaultGameDrawer();
}

} // namespace sge