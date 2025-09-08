set(NAME "runtime-fs")

add_library(${NAME} STATIC
    ../littlefs/dir.c
    ../littlefs/volume.c
    ../littlefs/vol_file.c
    ../stub/vol_flash.c
    ../littlefs/vol_mem.c
    ${CMAKE_SOURCE_DIR}/third_party/littlefs/lfs.c
    ${CMAKE_SOURCE_DIR}/third_party/littlefs/lfs_util.c
)
target_include_directories(${NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/littlefs
)
target_link_libraries(${NAME} PUBLIC
    runtime-sys
)
