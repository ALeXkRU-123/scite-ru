@ECHO OFF
SET PATH=C:\MinGW\bin;%ProgramFiles%\CodeBlocks\bin;C:\MinGW\upx;%PATH%

mingw32-make all
if errorlevel 1 exit /b 1
MOVE lpeg.dll ..\..\Pack\tools\LuaLib\