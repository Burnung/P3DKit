#opts
FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fpch-deps -MMD -fPIC")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -fpch-deps -MMD -fPIC")
set(OpenGL_GL_PREFERENCE LEGACY)
#inc
include_directories("${PROJECT_SOURCE_DIR}/libs/platforms/linux")
#libs
#find_package(GLEW REQUIRED)
#lib assimp
find_package(ASSIMP)
set(ASSIMP_INCLUDE ${ASSIMP_INCLUDE_DIRS})
set(assimp ${ASSIMP_LIBRARIES})
set(OpenCV_DIR /usr/local/share/OpenCV)
# free image
#find_package(FREEIMAGE)
#message(freeimage ${FREEIMAGE_INCLUDE_DIRS})
#predefined macros
add_definitions(-DIS_LINUX=1)
add_definitions(-DIS_DESKTOP=1)

function(getGL_libs libs_name)
    find_package(GLEW)
    find_package(OpenGL)# REQUIRED)
    #set(${libs_name} EGL ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} PARENT_SCOPE)
    find_package(glfw3 REQUIRED)
    #find_Package(OPenGL)
    set(${libs_name} glfw ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} GLEW PARENT_SCOPE)
    message(getgllibs ${libs_name})
endfunction()
