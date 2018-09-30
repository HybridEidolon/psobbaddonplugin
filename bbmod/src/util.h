#pragma once

#include <string>
#include <fstream>
#include <iostream>

std::string string_format(const std::string fmt_str, ...);

std::string bbmod_get_config_property(std::ifstream &   infile, const std::string propname);

