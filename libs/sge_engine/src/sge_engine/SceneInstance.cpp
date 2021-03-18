#include "SceneInstance.h"
#include "sge_utils/utils/FileStream.h"
#include "sge_utils/utils/json.h"

namespace sge {

void SceneInstance::newScene(bool forceKeepSameInspector) {
	m_world.clear();
	if (forceKeepSameInspector == false) {
		m_inspector.clear();
	}
	m_world.inspector = &m_inspector;
	m_inspector.m_world = &m_world;
	m_world.create();
}

void SceneInstance::loadWorldFromJson(const char* const json,
                                      bool disableAutoSepping,
                                      const char* const workingFileName,
                                      bool forceKeepSameInspector) {
	newScene(forceKeepSameInspector);
	[[maybe_unused]] bool success = loadGameWorldFromString(&m_world, json, workingFileName);
	sgeAssert(success);

	getInspector().m_disableAutoStepping = disableAutoSepping;
}

void SceneInstance::loadWorldFromFile(const char* const filename, bool disableAutoSepping, bool forceKeepSameInspector) {
	std::vector<char> fileContents;
	if (FileReadStream::readFile(filename, fileContents)) {
		fileContents.push_back('\0');
		loadWorldFromJson(fileContents.data(), disableAutoSepping, filename, forceKeepSameInspector);
		return;
	}

	sgeAssert(false);
}

bool SceneInstance::saveWorldToFile(const char* const filename) {
	if (!filename) {
		return false;
	}

	JsonValueBuffer jvb;
	JsonValue* const jWorld = serializeGameWorld(&m_world, jvb);
	if_checked(jWorld) {
		JsonWriter jw;
		bool succeeded = jw.WriteInFile(filename, jWorld, true);
		return succeeded;
	}

	return false;
}

void SceneInstance::update(float dt, const InputState& is) {
	dt = clamp(minOf(dt, 1.f), 0.f, 1.f / 15.f);
	const GameUpdateSets updateSets(dt, !m_inspector.isSteppingAllowed(), is);
	m_world.update(updateSets);

	for (int t = 0; t < m_world.m_postSceneUpdateTasks.size(); ++t) {
		IPostSceneUpdateTask* const task = m_world.m_postSceneUpdateTasks[t].get();

		PostSceneUpdateTaskSetWorldState* const taskChangeWorldJson = dynamic_cast<PostSceneUpdateTaskSetWorldState*>(task);
		if (taskChangeWorldJson != nullptr) {
			taskChangeWorldJson->newWorldStateJson;
			loadWorldFromJson(taskChangeWorldJson->newWorldStateJson.c_str(), false, m_world.m_workingFilePath.c_str());
			m_inspector.m_disableAutoStepping = !taskChangeWorldJson->noPauseNoEditorCamera;
			m_inspector.m_useEditorCamera = !taskChangeWorldJson->noPauseNoEditorCamera;
		}

		PostSceneUpdateTaskLoadWorldFormFile* const taskChangeWorld = dynamic_cast<PostSceneUpdateTaskLoadWorldFormFile*>(task);
		if (taskChangeWorld != nullptr) {
			loadWorldFromFile(taskChangeWorld->filename.c_str(), m_inspector.m_disableAutoStepping);
			m_inspector.m_disableAutoStepping = !taskChangeWorld->noPauseNoEditorCamera;
			m_inspector.m_useEditorCamera = !taskChangeWorld->noPauseNoEditorCamera;
		}
	}

	m_world.m_postSceneUpdateTasks.clear();
}

} // namespace sge
