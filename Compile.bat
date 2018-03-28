@echo off

set PATH=%PATH%;C:\Qt\Tools\mingw530_32\bin;

echo Enter SFML location (this folder needs to have a lib and a include subfolder) :
set /P SFMLdir=""

set lib_static=-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lopengl32 -lgdi32 -lfreetype
set lib_dynamic=-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system
set param_compile=-I%CD% -I %SFMLdir%\include -std=c++11
set param_link=-L%SFMLdir%\lib -L%CD%\deps %lib_dynamic% -o shooter.exe

echo g++ -c *.cpp %param_compile% 
g++ -c *.cpp %param_compile% 

echo g++ *.o %param_link% -o Rush.exe
g++ *.o %param_link% -o RobotsWar.exe

del *.o

echo Executable created !
