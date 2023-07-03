@echo off

devenv TalesCraft2.sln /Build "Release|x64"

xcopy .\build\x64\release\* ..\TalesCraft2\Supports\bin /syd /exclude:.\TalesCraft2.export-exclude-list.txt
