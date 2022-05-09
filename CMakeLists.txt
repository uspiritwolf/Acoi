cmake_minimum_required (VERSION 3.8)

project ("VoA")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory ("ThirdParty/glfw")

add_subdirectory ("Source")
