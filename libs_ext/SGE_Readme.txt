
A List of modifications on 3rd party libraries.

MCPP - See the directory for more info.
IMGUI - Te library code isn't changed however we've added a few additional libraries/extensions that are made for it.
SDL2 - a CMake modification, used to disable the install code. We have our own install and we do not want the one provided by SDL2.
HLSLParser - Based on https://github.com/Thekla/hlslparser we have some minor modifications for acessing texture sizes in HLSL. The function name is tex2Dsize()