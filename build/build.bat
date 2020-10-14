call cmake .. 
call cmake --build . --config Release
cd Release
ZincInstaller.exe
cd ..
