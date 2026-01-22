@echo off

rem Control switch for harfbuzz copying (true/false)
set "COPY_HARFBUZZ=false"

set "COPY_OUT_DIR=false"

rem Copy harfbuzz directory from source to target location if switch is true
if "%COPY_HARFBUZZ%"=="true" (
    set SOURCE_HARFBUZZ_DIR=E:\src\08_flutter-repo\flutter-1\engine\src\third_party\harfbuzz\
    set DEST_HARFBUZZ_DIR=E:\src\03_unity\com.unity.uiwidgets\engine\src\third_party\harfbuzz\
    
    echo Copying harfbuzz directory from %SOURCE_HARFBUZZ_DIR% to %DEST_HARFBUZZ_DIR%...
    
    rem Clean target directory if it exists
    if exist "%DEST_HARFBUZZ_DIR%" (
        echo Cleaning destination harfbuzz directory...
        rmdir /s /q "%DEST_HARFBUZZ_DIR%"
    )
    
    REM Create destination directories if they don't exist
    mkdir "%DEST_HARFBUZZ_DIR%" 2>nul
    
    rem Copy files from source to target
    xcopy "%SOURCE_HARFBUZZ_DIR%\*" "%DEST_HARFBUZZ_DIR%\" /e /i /h /y
    echo harfbuzz directory copied successfully
) else (
    echo Skipping harfbuzz directory copy as configured
)

rem Control switch for out directory copying (true/false)
if "%COPY_OUT_DIR%"=="true" (
    set SOURCE_OUT_DIR=E:\src\08_flutter-repo\flutter-1\engine\src\out\
    set DEST_OUT_DIR=E:\src\03_unity\com.unity.uiwidgets\engine\src\out\

    REM Clean destination directories
    if exist "%DEST_OUT_DIR%" (
        echo Cleaning destination out directory...
        rmdir /s /q "%DEST_OUT_DIR%"
    )

    REM Create destination directories if they don't exist
    mkdir "%DEST_OUT_DIR%" 2>nul

    REM Copy files
    echo Copying out directory...
    xcopy "%SOURCE_OUT_DIR%" "%DEST_OUT_DIR%" /e /i /h /y
    echo out directory copied successfully
)

REM Execute bee command to start building
cd "E:\src\03_unity\com.unity.uiwidgets\engine"
echo Starting build...
bee

echo Build completed!

xcopy "E:\src\03_unity\com.unity.uiwidgets\engine\build_release" "E:\wkspaces\UIWidgetsSample\Packages\com.unity.uiwidgets\Runtime\Plugins\x86_64" /e /i /h /y
rem pause

call E:\src\03_unity\tjrepo\tj-3\build\WindowsEditor\x64\Debug\Tuanjie.exe --projectPath e:\wkspaces\UIWidgetsSample