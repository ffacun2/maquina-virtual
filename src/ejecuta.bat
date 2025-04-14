@echo off
gcc main.c mv.c disassembler.c operaciones.c splitter.c -o main.exe

if %errorlevel% neq 0 (
    echo Error en la compilacion.
    pause
    exit /b
)

echo Compilacion exitosa.
set /p archivo=Ingrese el nombre del archivo junto con la extension .vmx: 
main.exe %archivo% -d

pause