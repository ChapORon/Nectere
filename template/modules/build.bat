rmdir bin\ /s /q
mkdir build-tmp\
cd build-tmp
cmake ..
cmake --build . --config Release
cd ..
mkdir bin
move build-tmp\Release\*.dll bin\
rmdir build-tmp\ /s /q
pause