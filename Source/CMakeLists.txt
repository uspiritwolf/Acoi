cmake_minimum_required (VERSION 3.16)

set(IMGUI_PATH "imgui")

file(GLOB_RECURSE IMGUI_SRC "${IMGUI_PATH}/*.cpp")
file(GLOB_RECURSE IMGUI_HEADER "${IMGUI_PATH}/*.h")

include_directories(${CMAKE_SOURCE_DIR} ${IMGUI_PATH})

add_executable (VoA ${IMGUI_SRC} ${IMGUI_HEADER} "StdAfx.h" "Main.cpp" "App.cpp" "App.h")

target_link_libraries(VoA glfw)

find_package(OpenGL REQUIRED)

target_link_libraries(VoA OpenGL::GL)

IF(WIN32)
	set_target_properties(VoA PROPERTIES WIN32_EXECUTABLE TRUE)

	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

target_precompile_headers(VoA PRIVATE "StdAfx.h")