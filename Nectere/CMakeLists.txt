# CMakeList.txt : CMake project for Nectere, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../DataParser)

# Add source to this project's executable.
add_executable (Nectere "Nectere.cpp" "Nectere.h" "Discord/Discord.cpp" "Discord/Discord.h" "Twitch/Twitch.cpp" "Twitch/Twitch.h")

# TODO: Add tests and install targets if needed.
if (WIN32)
	target_link_libraries(Nectere DataParser-s)
else()
	target_link_libraries(Nectere DataParser)
endif()