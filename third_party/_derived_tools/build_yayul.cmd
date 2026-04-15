@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -host_arch=x64 -arch=x64 >nul
if errorlevel 1 exit /b 1
cd /d C:\Users\p00121261\AndroidStudioProjects\MyApplication\third_party\apollo\upstream\virtualagc\yaYUL
if errorlevel 1 exit /b 1
"C:\Users\p00121261\AppData\Local\Android\Sdk\ndk\28.2.13676358\toolchains\llvm\prebuilt\windows-x86_64\bin\clang.exe" --target=x86_64-pc-windows-msvc -O2 -DMSC_VS -D_CRT_SECURE_NO_WARNINGS -DNVER=\"local\" *.c -o C:\Users\p00121261\AndroidStudioProjects\MyApplication\third_party\_derived_tools\yaYUL.exe
