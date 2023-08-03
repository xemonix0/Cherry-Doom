include(CherryDoomSettings)

add_library(setup STATIC
            execute.c           execute.h
            mainmenu.c
            multiplayer.c       multiplayer.h)

target_cherrydoom_settings(setup)

target_include_directories(setup
                           INTERFACE "."
                           PRIVATE "../src/" "${CMAKE_CURRENT_BINARY_DIR}/../")

target_link_libraries(setup textscreen SDL2::SDL2)
