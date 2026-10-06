@echo off
setlocal
set root=%~dp0..
cd /d "%root%"

call "%root%\generate.bat" --games=%1
if errorlevel 1 exit /b 1

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set msbuild=%%i
"%msbuild%" "%root%\build\x64-zt.sln" -p:Configuration=Release -p:Platform=x64 -m -v:minimal -nologo
