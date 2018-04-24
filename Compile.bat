@echo off

echo Enter g++ path, ex "C:\mingw64\bin" :  
set /P g++path=""

set PATH=%PATH%;%g++path%;

echo Enter SFML location (this folder needs to have a lib and a include subfolder) :
set /P SFMLdir=""

echo Enter fparser location (this folder needs to have a lib and a include subfolder) :
set /P FPARSERdir=""

set lib_dynamic=-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system -lfparser

set param_compile=-I%CD% -I %SFMLdir%\include -I%FPARSERdir%\include -std=c++17
set param_link=-L%SFMLdir%\lib -L%CD%\deps %lib_dynamic% -L%FPARSERdir%\lib -o shooter.exe -std=c++17 -lstdc++fs

echo g++ -c *.cpp %param_compile% 
g++ -c *.cpp %param_compile% 

echo g++ *.o %param_link% -o Rush.exe
g++ *.o %param_link% -o bin\RobotsWar.exe

del *.o

echo Executable created !
pause