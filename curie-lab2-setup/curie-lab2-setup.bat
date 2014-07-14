rem ======================================================================
rem curie-lab2-setup.bat
rem ======================================================================
rem This script will setup a lab computer in Phillips 237 for the
rem Curie Academy Lab 2.

@echo on
setlocal enableextensions

rem ----------------------------------------------------------------------
rem Copy disassemble
rem ----------------------------------------------------------------------

copy disassemble.bat C:\Users\LAB_USER\Desktop

rem ----------------------------------------------------------------------
rem Copy curie_lab2_template.ino
rem ----------------------------------------------------------------------

mkdir C:\Users\LAB_USER\Documents\Arduino\curie_lab2_template
copy  curie_lab2_template.ino C:\Users\LAB_USER\Documents\Arduino\curie_lab2_template

