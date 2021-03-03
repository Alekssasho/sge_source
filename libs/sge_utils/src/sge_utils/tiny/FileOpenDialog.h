#pragma once

#include <string>
#include "sge_utils/sge_utils.h"

namespace sge {

bool DialogYesNo(const char* caption, const char* message);

std::string FileOpenDialog(const std::string& prompt, bool fileMustExists, const char* fileFilter);
std::string FileSaveDialog(const std::string& prompt, const char* fileFilter, const char* defaultExtension);

}
