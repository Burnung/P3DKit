find_library(accelerate Accelerate REQUIRED)
find_library(foundation Foundation REQUIRED)
add_definitions(-DGLEW_STATIC)
find_package(GLEW REQUIRED STATIC)

add_compile_options (
	#-DEIGEN_USE_BLAS
	-DGL_SILENCE_DEPRECATION
	#-fembed-bitcode
	-ffast-math
	-Wno-deprecated-declarations
	-fvisibility=hidden
)


#predefined macros
add_definitions(-DIS_MACOS=1)
add_definitions(-DIS_APPLE=1)
add_definitions(-DIS_DESKTOP=1)

link_directories(/usr/local/lib)

function(getGL_libs libs_name)
    find_package(GLEW REQUIRED STATIC)
    find_package(glfw3)
    find_Package(OPenGL REQUIRED)
    set(tmp_libs glfw ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} GLEW::glew_s)
    set(${libs_name} ${tmp_libs} "-framework CoreFoundation -framework OpenGL" PARENT_SCOPE)
    message(getgllibs ${libs_name})
endfunction()

