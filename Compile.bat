@echo off

echo Do you want to compile for 32 bit (y or n) ?
set /P mode=""

if %mode% == y (goto SET32MODE)

set g++path=C:\mingw64\bin
set mode=64bit
set FPARSERdir=C:\fparser
set SFMLdir=C:\SFML\SFML-release
goto ENDMODESET

:SET32MODE
set g++path=C:\mingw32\bin
set mode=32bit
set FPARSERdir=C:\fparser\mingw32-7.3-sjljrev0
set SFMLdir=C:\SFML\SFML-release\mingw32-7.3-sjljrev0

:ENDMODESET
set PATH=%PATH%;%g++path%;

set lib_dynamic=-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system -lfparser

set param_compile=-I%CD% -I %SFMLdir%\include -I%FPARSERdir%\include -std=c++17
set param_link=-L%SFMLdir%\lib -L%CD%\deps %lib_dynamic% -L%FPARSERdir%\lib -std=c++17 -lstdc++fs

echo g++ -c *.cpp %param_compile% 
g++ -c *.cpp %param_compile% 

if %mode% == 32bit (
echo g++ *.o %param_link% -o bin\bin32\RobotsWar.exe
g++ *.o %param_link% -o bin\bin32\RobotsWar.exe)
if %mode% == 64bit (
echo g++ *.o %param_link% -o bin\RobotsWar.exe
g++ *.o %param_link% -o bin\RobotsWar.exe)


del *.o

echo Executable created !
pause