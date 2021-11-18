#predefined macros
add_definitions(-DIS_IOS=1)
add_definitions(-DIS_APPLE=1)

function(getGL_libs libs_name)
    #find_Package(OPenGL REQUIRED)
    set(${libs_name} "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo" PARENT_SCOPE)
    message(getgllibs ${libs_name})
endfunction()

