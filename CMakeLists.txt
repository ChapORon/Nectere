cmake_minimum_required (VERSION 3.8)
project (Nectere)

set(CMAKE_CXX_STANDARD 17)

# Include sub-projects.
add_subdirectory(DataParser)
add_subdirectory(Nectere)
add_subdirectory(NectereClientTest)
