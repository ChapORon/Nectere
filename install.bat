@net session >nul 2>&1

@IF %errorLevel% EQU 0 (
	@cd %~dp0
	::Pre-clean
	@rmdir nectere-build\ /s /q

	::Working directory
	@mkdir nectere-build
	@cd nectere-build\

	::Create header only folder
	@mkdir lib\
	@mkdir lib\Nectere\
	@xcopy ..\Nectere\ lib\Nectere\ /E
	@del lib\Nectere\*.txt /s /f /q
	@del lib\Nectere\*.cpp /s /f /q
	@del lib\Nectere\*.c /s /f /q

	::Build libraries
	@mkdir nectere-build-tmp
	@cd nectere-build-tmp
	@cmake ..\..
	@cmake --build . --config Release
	@cmake -P cmake_install.cmake
	@cd ..
	@mkdir bin
	@move nectere-build-tmp\Nectere\Release\*.lib bin\
	@move nectere-build-tmp\Nectere\Dp\Release\*.lib bin\
	@move nectere-build-tmp\Nectere\parg\Release\*.lib bin\
	@move nectere-build-tmp\Nectere\Parameters\Release\*.lib bin\
	::rmdir nectere-build-tmp\ /s /q

	::Move build libraries and header files on system
	@cd ..
	::setx NECTERE_PATH "%cd%\nectere-build"
	::xcopy nectere-build\ [NECTERE_PATH]\ /E

	::Clean everything
	::rmdir nectere-build\ /s /f /q
) ELSE (
	@echo Please run the script as an administrator
)

@pause