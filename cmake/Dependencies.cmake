include(FetchContent)

include(cmake/dependencies/OpenGL.cmake)    #defines target cs250_opengl
include(cmake/dependencies/GLEW.cmake)      # defines target cs250_glew
include(cmake/dependencies/SDL2.cmake)      # defines target cs250_sdl2
include(cmake/dependencies/DearImGUI.cmake) # defines target imgui   ;  note DearImGUI.cmake depends on SDL2.cmake
include(cmake/dependencies/STB.cmake)       # defines target stb
include(cmake/dependencies/GSL.cmake)       # defines target gsl
include(cmake/dependencies/GLM.cmake)       # defines target cs250_glm

add_library(dependencies INTERFACE)

target_link_libraries(dependencies INTERFACE 
    cs250_opengl
    cs250_glew
    cs250_sdl2
    imgui
    stb
    gsl
    cs250_glm
)
