@echo off
gcc main.c mv.c disassembler.c -o main.exe

if %errorlevel% neq 0 (
    echo Error en la compilacion.
    pause
    exit /b
)

echo Compilacion exitosa.
set /p archivo=Ingrese el nombre del archivo junto con la extension .vmx: 
vmx.exe %archivo% -d

pause