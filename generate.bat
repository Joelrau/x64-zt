@echo off
git submodule update --init --recursive
tools\premake5 %* vs2026 --iw8-copy-to="D:\Games\! MW IW8 CLIENTS\1.20\Call of Duty Modern Warfare (2019)"
pause