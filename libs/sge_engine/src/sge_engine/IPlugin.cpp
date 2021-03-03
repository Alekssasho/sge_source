#include "IPlugin.h"
#include "DefaultGameDrawer.h"

namespace sge {
IGameDrawer* IPlugin::allocateGameDrawer() {
	return new DefaultGameDrawer();
}
} // namespace sge
