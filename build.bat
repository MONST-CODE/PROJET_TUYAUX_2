@echo off
setlocal enabledelayedexpansion

:: --- 1. CONFIGURATION ---
set "MSYS_ROOT=C:\msys64\ucrt64"
set "LOCAL_DIR=backends"
set "INCLUDES=-I"%LOCAL_DIR%" -I"%MSYS_ROOT%\include" -I."
set "LIBS=-L"%MSYS_ROOT%\lib" -lSDL3"
set "FLAGS=-std=c++20 -O2 -Wall -DSDL_MAIN_HANDLED %INCLUDES%"

if not exist "%LOCAL_DIR%" mkdir "%LOCAL_DIR%"
if not exist "obj" mkdir "obj"

:: --- 2. VERIFICATION IMGUI ---
set "BASE_URL=https://raw.githubusercontent.com/ocornut/imgui/master"
set "BACKEND_URL=https://raw.githubusercontent.com/ocornut/imgui/master/backends"
set FILES=imgui.h imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_internal.h imstb_rectpack.h imstb_textedit.h imstb_truetype.h
set BACKEND_FILES=imgui_impl_sdl3.h imgui_impl_sdl3.cpp imgui_impl_sdlrenderer3.h imgui_impl_sdlrenderer3.cpp

for %%f in (%FILES%) do if not exist "%LOCAL_DIR%\%%f" curl -L -s -o "%LOCAL_DIR%\%%f" "%BASE_URL%/%%f"
for %%f in (%BACKEND_FILES%) do if not exist "%LOCAL_DIR%\%%f" curl -L -s -o "%LOCAL_DIR%\%%f" "%BACKEND_URL%/%%f"

:: --- 3. COMPILATION ---
:: Nettoyage des vieux fichiers
del *.pcm *.exe *.o 2>nul
del obj\*.pcm obj\*.o 2>nul

echo.
echo [1/8] Compilation ImGui (vers obj/)...
if not exist "obj\imgui.o" (
    clang++ %FLAGS% -c "%LOCAL_DIR%\imgui.cpp" -o obj/imgui.o
    clang++ %FLAGS% -c "%LOCAL_DIR%\imgui_draw.cpp" -o obj/imgui_draw.o
    clang++ %FLAGS% -c "%LOCAL_DIR%\imgui_tables.cpp" -o obj/imgui_tables.o
    clang++ %FLAGS% -c "%LOCAL_DIR%\imgui_widgets.cpp" -o obj/imgui_widgets.o
    clang++ %FLAGS% -c "%LOCAL_DIR%\imgui_impl_sdl3.cpp" -o obj/imgui_impl_sdl3.o
    clang++ %FLAGS% -c "%LOCAL_DIR%\imgui_impl_sdlrenderer3.cpp" -o obj/imgui_impl_sdlrenderer3.o
) else (
    echo    - Deja compile.
)

echo [2/8] Module Theme...
clang++ %FLAGS% --precompile theme.cppm -o obj/theme.pcm
clang++ %FLAGS% -c obj/theme.pcm -o obj/theme.o

echo [3/8] Module Logic...
clang++ %FLAGS% --precompile logic.cppm -o obj/logic.pcm
clang++ %FLAGS% -c obj/logic.pcm -o obj/logic.o

echo [4/8] Module Particles...
clang++ %FLAGS% --precompile particles.cppm -o obj/particles.pcm
clang++ %FLAGS% -c obj/particles.pcm -o obj/particles.o

echo [5/8] Module Renderer...
clang++ %FLAGS% -fmodule-file=logic=obj/logic.pcm -fmodule-file=particles=obj/particles.pcm -fmodule-file=theme=obj/theme.pcm --precompile renderer.cppm -o obj/renderer.pcm
clang++ %FLAGS% -fmodule-file=logic=obj/logic.pcm -fmodule-file=particles=obj/particles.pcm -fmodule-file=theme=obj/theme.pcm -c obj/renderer.pcm -o obj/renderer.o

echo [6/8] Module Game...
clang++ %FLAGS% -fmodule-file=logic=obj/logic.pcm -fmodule-file=renderer=obj/renderer.pcm -fmodule-file=particles=obj/particles.pcm -fmodule-file=theme=obj/theme.pcm --precompile game.cppm -o obj/game.pcm
clang++ %FLAGS% -fmodule-file=logic=obj/logic.pcm -fmodule-file=renderer=obj/renderer.pcm -fmodule-file=particles=obj/particles.pcm -fmodule-file=theme=obj/theme.pcm -c obj/game.pcm -o obj/game.o

echo [7/8] Compilation Main...
clang++ %FLAGS% -fmodule-file=game=obj/game.pcm -fmodule-file=logic=obj/logic.pcm -fmodule-file=renderer=obj/renderer.pcm -fmodule-file=particles=obj/particles.pcm -fmodule-file=theme=obj/theme.pcm -c main.cpp -o obj/main.o

echo [8/8] LINKAGE FINAL...
set OBJS=obj/main.o obj/logic.o obj/particles.o obj/renderer.o obj/game.o obj/theme.o obj/imgui.o obj/imgui_draw.o obj/imgui_tables.o obj/imgui_widgets.o obj/imgui_impl_sdl3.o obj/imgui_impl_sdlrenderer3.o
clang++ %FLAGS% %OBJS% -o PipeMasterUltra.exe %LIBS%

if !errorlevel! neq 0 (
    echo [ERREUR] Linkage echoue.
    pause
    exit /b 1
)

echo.
echo [SUCCES] Lancement...
.\PipeMasterUltra.exe