# Take all .cpp files and put it in the variable neat_sources
FILE(GLOB neat_sources *.cpp)

# Take all .h files and put it in the variable neat_header
FILE(GLOB neat_header *.h)

# Add the shared library neat
# shared libraries are linked dynamically and loaded at runtime

add_library(neat SHARED ${neat_header} ${neat_sources})
