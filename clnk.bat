@echo off
setlocal

REM Replace this path by your prefer (folder is required to be ib PATH)
set "INSTALLATION_FOLDER=D:\path\"

if "%~1"=="clean" (
  REM Clear temporary folder
  if exist %TEMP%\.clnk rmdir /s /q %TEMP%\.clnk
  echo Temporaries deleted
  goto :quit
)

set "APPLICATION_PATH=%~1"
set "LINK_NAME=%~2"
set "APPLICATION_WORKING_DIRECTORY=%~3"

if "%APPLICATION_PATH%"=="" (
  echo No application path specified
  goto :usage
)

if "%LINK_NAME%"=="" (
  echo No link name specified
  goto :usage
)

echo Including vcvars...
call vcvars64.bat

REM Check template file
if not exist "%~dp0\.clnk\template.cpp" (
  echo No template file found in "%~dp0\.clnk"
  echo Put template.cpp in the same folder with clnk.bat
  goto :error_quit
)

REM Go to the temporary folder
pushd "%~dp0\.clnk"

echo Now in "%cd%". Generate sources...

set "PREFIXES=path.cpp"

REM Create temporary file with application path
echo #define APPLICATION_PATH R"p(%APPLICATION_PATH%)p" > path.cpp

REM Create temporary file with working directory (if specified)
if not "%APPLICATION_WORKING_DIRECTORY%"=="" (
  echo #define APPLICATION_WORKING_DIRECTORY R"p(%APPLICATION_WORKING_DIRECTORY%)p" > wd.cpp
  set "PREFIXES=%PREFIXES% + wd.cpp"
)

echo Prefixes generated. Merging to final source...

REM Merge prefix files with template
copy %PREFIXES% + template.cpp final.cpp

echo Compiling...

REM Compile final source
cl /c /nologo /EHsc final.cpp /Fo:final.o
if errorlevel 1 popd & goto :error_quit

echo Compiled! Linking...

REM Link object to the final executable
link /nologo final.o User32.lib Shell32.lib /SUBSYSTEM:WINDOWS "/OUT:%INSTALLATION_FOLDER%%LINK_NAME%.exe"
if errorlevel 1 popd & goto :error_quit

echo Linked!

pause

echo Cleaning up...

del path.cpp
if not "%APPLICATION_WORKING_DIRECTORY%"=="" del wd.cpp
del final.cpp
del final.o

echo Everything done.

REM Return to original startup folder
popd

REM Finally quit

:quit
pause
endlocal
exit /b 0

:error_quit
pause
endlocal
exit /b 1

:usage
echo Usage: %~0 (application_path) (shortcut_name) [startup_directory]
goto :error_quit