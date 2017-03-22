@echo off
rem F: \\WYSE\C
rem I: \\WYSE\D
rem J: \\WYSE\E
rem L: \\WYSE\F
rem P: \\WYSE\G

if "%1"=="" goto end

echo .>usage.txt

net use %1 \\wyse\c
echo --- \\wyse\c ----------------------------------------------------- >>usage.txt
usage %1 >>usage.txt
net use %1 /d

net use %1 \\wyse\d
echo --- \\wyse\d ----------------------------------------------------- >>usage.txt
usage %1 >>usage.txt
net use %1 /d

net use %1 \\wyse\e
echo --- \\wyse\e ----------------------------------------------------- >>usage.txt
usage %1 >>usage.txt
net use %1 /d

net use %1 \\wyse\f
echo --- \\wyse\f ----------------------------------------------------- >>usage.txt
usage %1 >>usage.txt
net use %1 /d

net use %1 \\wyse\g
echo --- \\wyse\g ----------------------------------------------------- >>usage.txt
usage %1 >>usage.txt
net use %1 /d

:end
