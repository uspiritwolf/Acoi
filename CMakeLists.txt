cmake_minimum_required (VERSION 3.8)

project ("Acoi")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
SET(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)
SET(BUILD_LIST "core,imgcodecs,gapi,imgproc,objdetect,calib3d,features2d,dnn,flann" CACHE BOOL "" FORCE)
SET(BUILD_WITH_STATIC_CRT OFF CACHE BOOL "" FORCE)

add_subdirectory ("ThirdParty/glfw")

add_subdirectory ("ThirdParty/opencv")

add_subdirectory ("Source")

file(COPY "${OpenCV_SOURCE_DIR}/data/haarcascades/haarcascade_frontalface_alt.xml" DESTINATION "${CMAKE_BINARY_DIR}/bin")
file(COPY "${OpenCV_SOURCE_DIR}/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml" DESTINATION "${CMAKE_BINARY_DIR}/bin")