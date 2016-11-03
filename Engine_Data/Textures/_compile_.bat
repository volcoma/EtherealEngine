cls
@echo off

setlocal
set PATH=%PATH%;D:\Software\Cygwin\bin

echo *************************************************
echo This script will compile the assets in this folder
echo *************************************************

pushd %~dp0
call make rebuild
popd

echo All assets compiled.

pause