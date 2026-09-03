@echo off
set /p ver="Version?: "
echo %ver%
::cmd /C itchio-butler push Apollo-GMS1.gmez yellowafterlife/gamemaker-lua:gms1 --userversion=%ver%
::cmd /C itchio-butler push Apollo-GMS2.yymp yellowafterlife/gamemaker-lua:gms2 --userversion=%ver%
cmd /C itchio-butler push Apollo-GMS2.3.yymps yellowafterlife/gamemaker-lua:gms23 --userversion=%ver%
pause