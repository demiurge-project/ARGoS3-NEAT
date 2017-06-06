#Genome parser

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Message to print in the terminal
message(STATUS "Building genome parser...")

add_executable(genome_to_graph parserGenome.cpp)
set_property(TARGET genome_to_graph PROPERTY CXX_STANDARD 11)
set_property(TARGET genome_to_graph PROPERTY CXX_STANDARD_REQUIRED ON)

#if you want the version suited for the 23 input network uncoment the following line :
#add_executable(genome_to_graph23 parserGenome.cpp)
