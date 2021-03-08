
A List of modifications on 3rd party libraries.

MCPP - See the directory for more info.
IMGUI - Te library code isn't changed however we've added a few additional libraries/extensions that are made for it.
SDL2:
 - a CMake modification, used to disable the install code. We have our own install and we do not want the one provided by SDL2.
 - A small change in original line:
	list(APPEND EXTRA_LIBS user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32)
	We've added vcruntime as an argument as in Release the code uses memset



HLSLParser - Based on https://github.com/Thekla/hlslparser we have some minor modifications for acessing texture sizes in HLSL. The function name is tex2Dsize()