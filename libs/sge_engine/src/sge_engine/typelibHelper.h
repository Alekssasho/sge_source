#pragma once

#include "sge_engine/TypeRegister.h"

#define ReflAddActor(T) ReflAddType(T) ReflInherits(T, sge::Actor)
#define ReflAddObject(T) ReflAddType(T) ReflInherits(T, sge::GameObject)
#define ReflAddScript(T) ReflAddType(T) ReflInherits(T, sge::Script)
