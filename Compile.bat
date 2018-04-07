@echo off

echo Enter SFML location (this folder needs to have a lib and a include subfolder) :
set /P SFMLdir=""

set lib_dynamic=-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

set param_compile=-I%CD% -I %SFMLdir%\include -I%CD%\ATMSP -std=c++17
set param_link=-L%SFMLdir%\lib -L%CD%\deps -L%CD%\ATMSP %lib_dynamic% -o shooter.exe -std=c++17 -latmsp -lstdc++fs

echo g++ -c *.cpp %param_compile% 
g++ -c *.cpp %param_compile% 

echo g++ *.o %param_link% -o Rush.exe
g++ *.o %param_link% -o bin\RobotsWar.exe

del *.o

echo Executable created !
