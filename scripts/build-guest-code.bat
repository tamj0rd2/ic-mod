:: This script is meant to be run on the guest machine, not the host
:: it's purpose is to build the solution
echo "Trying to build the project..."
@echo off
call "C:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat"
:: set this to Release to build an actual production version
devenv %1 /build HalfRelease
