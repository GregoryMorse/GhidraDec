CD /D D:\source\repos\GhidraDec
rmdir /s /q build
mkdir build
cd build
SET SAVEPTH=%PATH%
SET PATH=%PATH%;%ProgramFiles%\cmake\bin;%UserProfile%\Desktop\Apps\win_flex_bison

del CMakeCache.txt
cmake .. -DIDA_SDK_DIR=D:\libraries\idasdk76
cmake --build . --config Release -- -m
copy Release\*.dll "%UserProfile%\Desktop\Apps\IDA Pro 7.6\plugins"
cd release
mkdir ida76
copy /Y *.dll ida76
REM "%ProgramFiles%\7-zip\7z.exe" a -pexetools ghidradec.zip *.dll
cd ..

del CMakeCache.txt
cmake .. -DIDA_SDK_DIR=D:\libraries\idasdk75
cmake --build . --config Release -- -m
copy Release\*.dll "%UserProfile%\Desktop\Apps\IDA Pro 7.5 SP3\plugins"
cd release
mkdir ida75
copy /Y *.dll ida75
REM "%ProgramFiles%\7-zip\7z.exe" a -pexetools ghidradec.zip *.dll
cd ..

del CMakeCache.txt
cmake .. -DIDA_SDK_DIR=D:\libraries\idasdk72
cmake --build . --config Release -- -m
copy Release\*.dll "%ProgramFiles%\Ida 7.2\plugins"
cd release
mkdir ida72
copy /Y *.dll ida72
REM "%ProgramFiles%\7-zip\7z.exe" a -pexetools ghidradec.zip *.dll
cd ..

del CMakeCache.txt
cmake .. -DIDA_SDK_DIR=D:\libraries\idasdk70
cmake --build . --config Release -- -m
copy Release\*.dll "%ProgramFiles%\Ida 7.0\plugins"
cd release
mkdir ida70
copy /Y *.dll ida70
REM "%ProgramFiles%\7-zip\7z.exe" a -pexetools ghidradec-ida70.zip *.dll
cd ..

REM default already seems to be: -G "Visual Studio 16 2019 Win64" -A x64
del CMakeCache.txt
del deps\jsoncpp\jsoncpp-project-prefix\src\jsoncpp-project-build\CMakeCache.txt
rmdir /s /q deps\jsoncpp\CMakeFiles
cmake .. -G "Visual Studio 16 2019" -A Win32 -DIDA_SDK_DIR32=D:\libraries\idasdk68
cmake --build . --config Release -- -m
move /Y Release\ghidradec64.p64 Release\ghidradec.p64
copy Release\*.plw "%ProgramFiles (x86)%\Ida 6.8\plugins"
copy Release\*.p64 "%ProgramFiles (x86)%\Ida 6.8\plugins"
cd release
mkdir ida68
copy /Y *.p64 ida68
copy /Y *.plw ida68
REM "%ProgramFiles%\7-zip\7z.exe" a -pexetools ghidradec-ida68.zip *.plw
cd..

cd release
del ghidradec.zip
"%ProgramFiles%\7-zip\7z.exe" a -pexetools ghidradec.zip ida*
cd ..

cd ..
SET PATH=%SAVEPTH%
SET SAVEPTH=