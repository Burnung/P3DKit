#opts
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /Zc:__cplusplus /D_CRT_SECURE_NO_WARNINGS /DNOMINMAX /D_USE_MATH_DEFINES /DWIN32_LEAN_AND_MEAN /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS /D_CRT_SECURE_NO_WARNINGS")
#inc
include_directories("${PROJECT_SOURCE_DIR}/libs/platforms/windows")
include(${ROOT_DIR}/libs/embree/embree.cmake)
#if(CMAKE_BUILD_TYPE STREQUAL Debug)
#	add_definitions(_DDebug)
#endif()
#predefined macros
add_definitions(-DIS_WINDOWS=1)
add_definitions(-DIS_DESKTOP=1)
add_definitions(-D_USE_MATH_DEFINES -DFREEIMAGE_LIB)
#file(GLOB assimp_t $ENV{ASSIMP_LIBRARY_PATH}/*.lib)
#set(FREEIMG_INCLUDE $ENV{FREEIMAGE_PATH})
#add_subdirectory(libs/FreeImage)
add_subdirectory(libs/Assimp)
set(glfw3_DIR ${ROOT_DIR}/libs/glfw/lib/cmake/glfw3)
set(GLEW_DIR ${ROOT_DIR}/libs/glew/lib/cmake/glew)
set(eigen_DIR ${ROOT_DIRd}/libs/eigen/share/cmake)
set(OpenCV_DIR ${ROOT_DIR}/libs/opencv/win/x64/vc16/staticlib)
add_subdirectory(libs/ycnn_net_cmake)
#add_subdirectory(${ROOT_DIR}/libs/glew)
#include(${ROOT_DIR}/libs/Assimp/assimp.cmake)
#find_package(Assimp REQUIRED)


function(getGL_libs libs_name)
    find_package(glew REQUIRED STATIC)
    message(root dir is ${ROOT_DIR})
    find_package(glfw3)
    find_Package(OPenGL)
    set(${libs_name} glfw ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} GLEW::glew_s PARENT_SCOPE)
    message(getgllibs ${libs_name})
endfunction()