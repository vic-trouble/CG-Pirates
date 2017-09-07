#pragma once

#include <iostream>

#ifndef SUBMISSION
#define LOG(x) std::cerr << x << std::endl;
#else
#define LOG(x)
#endif
