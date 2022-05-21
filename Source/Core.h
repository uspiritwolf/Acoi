#pragma once

#include <memory>
#include <functional>
#include <thread>
#include <vector>
#include <cstring>
#include <string>
#include <atomic>
#include <mutex>
#include <future>
#include <filesystem>

#if defined(WIN32)
#include <Windows.h>
#endif

#include "imgui.h"
#include "implot.h"
#include "imgui_stdlib.h"
