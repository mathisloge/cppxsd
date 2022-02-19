#pragma once
#include <filesystem>
#include "exceptions.hpp"


#include <iostream>
#define PRINT_TODO_NODE(my_def_curr_node) std::cout << "TODO: [" <<__FILE__ << '@' << __LINE__ << "] " << (my_def_curr_node) << std::endl;

namespace fs = std::filesystem;
