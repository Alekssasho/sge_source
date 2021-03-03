#####################################################
# HLSLParser
#####################################################
project(HLSLParser)

set(HLSLParser_SOURCES 
	./src/CodeWriter.cpp
	./src/Engine.cpp
	./src/GLSLGenerator.cpp
	./src/HLSLGenerator.cpp
	./src/HLSLParser.cpp
	./src/HLSLTokenizer.cpp
	./src/HLSLTree.cpp
	./src/MSLGenerator.cpp
)

add_library(HLSLParser STATIC ${HLSLParser_SOURCES})
target_include_directories(HLSLParser PUBLIC ./src)
target_link_libraries(HLSLParser PUBLIC mcpp)