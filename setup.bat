@echo off
set PROJECT_DIR=%~dp0


set BUILD_DIR=%PROJECT_DIR%build

if exist "%BUILD_DIR%" (
    echo Cleaning up build directory...
    rmdir /s /q "%BUILD_DIR%"
) else (
    echo Build directory does not exist. Skipping cleanup.
)

echo Creating build directory...
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

echo Configuring the project with CMake...
cmake -DCMAKE_BUILD_TYPE=Debug  .. || (
    echo CMake configuration failed. Exiting.
    exit /b 1
)

echo Building the project...
cmake --build . || (
    echo Build failed. Exiting.
    exit /b 1
)

echo Build completed successfully!
pause
