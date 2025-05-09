@echo off
echo Compilando proyecto...

gcc src/*.c -o vmx.exe

if %errorlevel% equ 0 (
echo Compilacion exitosa.
) else (
echo Error en la compilacion.
)

pause