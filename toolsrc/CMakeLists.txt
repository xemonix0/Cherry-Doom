include(CherryDoomSettings)

add_executable(bin2c EXCLUDE_FROM_ALL bin2c.c)
add_executable(bmp2c EXCLUDE_FROM_ALL bmp2c.c)
add_executable(swantbls EXCLUDE_FROM_ALL swantbls.c)

target_include_directories(bmp2c PRIVATE "../src/" "${CMAKE_CURRENT_BINARY_DIR}/../")

target_cherrydoom_settings(bin2c bmp2c swantbls)
