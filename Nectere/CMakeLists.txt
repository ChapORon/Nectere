# CMakeList.txt : CMake project for Nectere, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Nectere "Nectere.cpp" "Nectere.h" "Discord/Discord.cpp" "Discord/Discord.h" "Twitch/Twitch.cpp" "Twitch/Twitch.h")

# TODO: Add tests and install targets if needed.
