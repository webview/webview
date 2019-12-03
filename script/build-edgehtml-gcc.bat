@echo off
rem clang-cl -std=c++17 -DWEBVIEW_EDGEHTML -I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\cppwinrt" ../main.cc -o main.exe
clang-cl -std=c++17 -DWEBVIEW_EDGEHTML ../main.cc -o main.exe
pause