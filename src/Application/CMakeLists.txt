set(NAME "Application")

add_library(${NAME} STATIC
    Application.m
    ApplicationMain.m
    GPIO.m
    NXTimer.m
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} PUBLIC
    runtime-hw
    Foundation
)
