set(NAME "runtime-sys")

# Find OpenSSL
find_package(OpenSSL REQUIRED)

add_library(${NAME} STATIC
    abort.c
    exception.c
    mutex.c
    sys.c
    timer.c
    ../openssl/hash.c
    ../posix/memory.c
    ../posix/puts.c
    ../posix/random.c
    ../posix/sleep.c
    ../posix/thread.c
    ../posix/time.c
    ../posix/timeget.c
    ../all/panicf.c
    ../all/printf.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../../include/runtime-${RUNTIME}
    ${TOOLCHAIN_PATH}/include
)
target_link_libraries(${NAME} PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)
