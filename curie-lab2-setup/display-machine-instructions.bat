rem ======================================================================
rem display-machine-instructions.bat
rem ======================================================================
rem This script will first find the GNU objdump program suitable for
rem cross-compiled Atmel AVR programs, then it will use objdump to
rem disassemble the most recently compiled Arduino sketch. It saves the
rem output to the desktop as a text file.

@echo on
setlocal enableextensions

rem ----------------------------------------------------------------------
rem Find objdump
rem ----------------------------------------------------------------------

cd C:\CORNELL

for /f "delims=" %%f in ('dir/b/s/a-d avr-objdump.exe') do (
  set ARDUINO_OBJDUMP_PATH=%%f
  goto endloop1
)

: endloop1

rem ----------------------------------------------------------------------
rem Find most recently compiled Arduino sketch
rem ----------------------------------------------------------------------

cd C:\Users\LAB_USER\AppData\Local\Temp\

for /f "delims=" %%f in ('dir build*tmp /o:-d /b') do (
  set MOST_RECENT_DIR=%%f
  goto endloop2
)

: endloop2

rem ----------------------------------------------------------------------
rem Run objdump on the most recently compiled Arduino sketch
rem ----------------------------------------------------------------------

%ARDUINO_OBJDUMP_PATH% -d %MOST_RECENT_DIR%\*elf > C:\Users\LAB_USER\Desktop\machine-instructions.txt

