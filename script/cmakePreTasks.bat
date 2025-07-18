@echo off
set "GENERATED_DIR=build\generatedCodeFile"

REM 检查目录是否存在
if not exist "%GENERATED_DIR%" (
    echo CREATE_DERECTORY: %GENERATED_DIR%
    mkdir "%GENERATED_DIR%"
) else (
    echo DERECTORY_EXIST: %GENERATED_DIR%
) 
python script\python\generateCppFile.py
