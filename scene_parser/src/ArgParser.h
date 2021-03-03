#include "IAssetRelocationPolicy.h"
#include "Common.h"

namespace sge {

enum class ProgramMode
{
	NotSpecified,
	Mesh,
	Content,
};


struct ArgParser
{	
	struct MeshModeArguments
	{
		std::string input;
		std::string output;
		ModelParseSettings parseSettings;
	};

	struct ContentModeArguments
	{
		// Points to the "content" file which contains the information about which files should be parsed.
		std::string contentFilePath;

		// Points to there the input assets are located.
		std::string assetsInputDir;

		// Where the generated files should be.
		std::string outputDir;
	};

	static void PrintHelp();

	ArgParser() = delete;

	ArgParser(const int argc, const char** argv) :
		argc(argc),
		argv(argv),
		m_succeeded(true)
	{
		m_succeeded = parse();
	}

	bool isFailed() const { return !m_succeeded; }
	ProgramMode getMode() const { return m_mode; }

	const MeshModeArguments& getMeshModeArgs() const { return m_meshModeArgs; }
	const ContentModeArguments& getContentModeArgs() const { return m_contentModeArgs; }
private : 

	bool parse();

	// Input.
	int argc = 0;
	const char** argv = nullptr;

	// Output.
	bool m_succeeded = true;
	ProgramMode m_mode = ProgramMode::NotSpecified;
	MeshModeArguments m_meshModeArgs;
	ContentModeArguments m_contentModeArgs;

	ModelParseSettings sceneParseSettings;
};

}
