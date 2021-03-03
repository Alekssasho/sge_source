#include "sge_utils/utils/Path.h"

#include <string.h>
#include <stdexcept>

#ifdef __linux__
#define sge_stricmp strcasecmp
#else
#define sge_stricmp _stricmp
#endif

#include "ArgParser.h"

namespace sge {

void ArgParser::PrintHelp()
{
	printf(
		"Usage:\n"
		"-h or -help: print help\n"
		"-m : Specify thmode of the content parser. In not specified the mode is going to be guessed based on the input/output filenames.\n"
		"     \"mesh\" - the target file is a mesh."
		"     \"content\" - the target is a content file description."
		"-i : input filename. [NOTE] If 1st argument type isn't specified the argument is concidered as '-f'.\n"
		"-o : output filename. [NOTE] If last argument type isn't specified the argument is concidered as '-o'. If not specified the inpath is going to be used with *.mdl extension.\n"
		"\n\n\nMesh mode settings:\n"
		"	-no_animations : bypass the animations\n"
		"\n\n\nContent mode settings:\n"
		"	-c : the content descriptor file name.\n"
		"Built on : " __DATE__ 
#if defined DEBUG || defined _DEBUG
	" with Debug "
#endif
		"\n"
	);
}

bool ArgParser::parse()
{
	int nextArgumentIdx = 0;

	const auto getNextArg = [&]() -> const char* {
		if(nextArgumentIdx >= argc) throw std::logic_error("Not enough arguments!\n");
		return argv[++nextArgumentIdx];
	};

	const auto hasMoreArgs = [&]() -> bool {
		return nextArgumentIdx < (argc - 1);
	};
	
	std::string input; // the value behind -i or -f.
	std::string output; // the value behind -o
	std::string contentDescFilename;
	bool no_animation = false; // true if -no_animation is specified.

	try
	{
		while(hasMoreArgs())
		{
			const char* const cmd = getNextArg();

			// Process command line here.

			if(sge_stricmp(cmd, "-h") == 0 || sge_stricmp(cmd, "-help") == 0)
			{
				PrintHelp();
			}
			if(sge_stricmp(cmd, "-i") == 0)
			{
				input = getNextArg();
			}
			else if(sge_stricmp(cmd, "-o") == 0)
			{
				output = getNextArg();
			}
			else if(sge_stricmp(cmd, "-c") == 0)
			{
				contentDescFilename = getNextArg();
			}
			else if(sge_stricmp(cmd, "-no_animations") == 0)
			{
				no_animation = true;
			}
			else if(sge_stricmp(cmd, "-m") == 0) {
				const char* const pModeStr = getNextArg();
				if(sge_stricmp(pModeStr, "mesh") == 0) m_mode = ProgramMode::Mesh;
				if(sge_stricmp(pModeStr, "content") == 0) m_mode = ProgramMode::Content;

			}
			// Keep these at the bottom.
			// If this is the 1st arg and its not know command assume that this is the input filename or
			// ff this is the last arg and its not know command assume that this is the output filename.
			else if(nextArgumentIdx == 1) {
				input = cmd;
			}
			else if(nextArgumentIdx == (argc - 1)) {
				output = cmd;
			}
			else
			{
				throw std::logic_error("FATAL ERROR: Unknown argument!\n");
			}
		}
	}
	catch(const std::exception& e)
	{
		sgeAssert(false);
		printf("Exception caught while parsing arguments : %s\n", e.what());
		return false;
	}
	catch(...)
	{
		sgeAssert(false);
		printf("Unknown exception caught while parsing arguments\n");
		return false;
	}

	if(input.empty()) {
		printf("FATAL ERROR: Missing input isn't specified!");
		return false;
	}

	// If the mode isn't specified yet attempt to deduce it based on the input.
	if(m_mode == ProgramMode::NotSpecified)
	{
		const std::string input_ext = extractFileExtension(input.c_str());
		if(input_ext == "json") m_mode = ProgramMode::Content;
		else if(input_ext == "dae") m_mode = ProgramMode::Mesh;
		else if(input_ext == "fbx") m_mode = ProgramMode::Mesh;
		else if(input_ext == "obj") m_mode = ProgramMode::Mesh;
	}

	// If these aren't determined, assume an error.
	if(input.empty()  || m_mode == ProgramMode::NotSpecified)
	{
		printf("FATAL ERROR: Input/Output/Mode is not specifed!");
		return false;
	}

	// If the output isn't specified and we are in mesh mode - genrate the output based on the input.s
	if(output.empty() && m_mode == ProgramMode::Mesh) {
		output = removeFileExtension(input.c_str()) + ".mdl";
	}
	
	if(m_mode == ProgramMode::Mesh)
	{
		m_meshModeArgs.input = input;

		// If the output isn't specified and we are in mesh mode - genrate the output based on the input.s
		if(output.empty()) {
			output = removeFileExtension(input.c_str()) + ".mdl";
		}

		// The "default renamig policy" which basically does notihng. I really do not like the place of this variable. I guess I'll be dropping it soon.
		static sge::NoneAssetRelocationPolicy noneAssetRelocationPolicy;

		m_meshModeArgs.input = input;
		m_meshModeArgs.output = output;
		m_meshModeArgs.parseSettings.pRelocaionPolicy = &noneAssetRelocationPolicy;
		m_meshModeArgs.parseSettings.shouldExportAnimations = !no_animation;

		return true;
	}
	else if(m_mode == ProgramMode::Content)
	{
		if(output.empty() || contentDescFilename.empty()) {
			printf("FATAL ERROR: Unspecified output directory or content file!");
			return false;
		}

		m_contentModeArgs.assetsInputDir = input;
		m_contentModeArgs.outputDir = output;
		m_contentModeArgs.contentFilePath = contentDescFilename;

		return true;
	}

	return false;
}

}
