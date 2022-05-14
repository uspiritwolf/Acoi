cmake_minimum_required (VERSION 3.16)

set(IMGUI_PATH "imgui")

set(OPENCV_HEADERS 
 "${OpenCV_SOURCE_DIR}/include"
 "${OpenCV_SOURCE_DIR}/modules/core/include"
 "${OpenCV_SOURCE_DIR}/modules/imgcodecs/include"
 "${OpenCV_SOURCE_DIR}/modules/gapi/include"
 "${OpenCV_SOURCE_DIR}/modules/imgproc/include"
 "${OpenCV_SOURCE_DIR}/modules/objdetect/include"
 "${OpenCV_SOURCE_DIR}/modules/calib3d/include"
 "${OpenCV_SOURCE_DIR}/modules/features2d/include"
 "${OpenCV_SOURCE_DIR}/modules/dnn/include"
 "${OpenCV_SOURCE_DIR}/modules/flann/include")

include_directories("${CMAKE_SOURCE_DIR}/Source" ${IMGUI_PATH} ${OPENCV_HEADERS} ${VoA_BINARY_DIR})

file(GLOB_RECURSE IMGUI_SRC "${IMGUI_PATH}/*.cpp")

file(GLOB_RECURSE IMGUI_HEADER "${IMGUI_PATH}/*.h")

set(GRAPHICS_SOURCES
 "Graphics/Texture.h" "Graphics/Texture.cpp")

set(UTILS_SOURCES
 "Utils/FileDialog.h" "Utils/FileDialog.cpp")

set(EDITOR_SOURCES
 "Editor/Editor.h" "Editor/Editor.cpp"
 "Editor/EditorInterface.h"
 "Editor/OpenCV/TextDetection.h" "Editor/OpenCV/TextDetection.cpp")

add_executable (VoA
 "Core.h"
 "Main.cpp"
 "App.h" "App.cpp"
 ${IMGUI_SRC}
 ${IMGUI_HEADER} 
 ${UTILS_SOURCES}
 ${EDITOR_SOURCES})

target_link_libraries(VoA glfw)

target_link_libraries(VoA 
 opencv_core
 opencv_gapi
 opencv_imgcodecs
 opencv_imgproc
 opencv_objdetect
 opencv_calib3d
 opencv_features2d
 opencv_dnn
 opencv_flann
)

find_package(OpenGL REQUIRED)

target_link_libraries(VoA OpenGL::GL)


IF(WIN32)
	set_target_properties(VoA PROPERTIES WIN32_EXECUTABLE TRUE)
	add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
ENDIF()

target_precompile_headers(VoA PRIVATE "Core.h")