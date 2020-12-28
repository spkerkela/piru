CPPPATH = [
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt",
    "C:/MinGW/include",
    "C:/mingw_dev_lib/include/SDL2",
]
LIBPATH = ["C:/MinGW/lib", "C:/mingw_dev_lib/lib"]
Program(
    "piru",
    Glob("*.c"),
    LIBS=["sdl2", "sdl2_ttf", "sdl2_image", "sdl2_mixer"],
    LIBPATH=LIBPATH,
    CPPPATH=CPPPATH,
)
