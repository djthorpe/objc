# Centralized installation logic for installation
include(GNUInstallDirs)

# Library directory selection
if(DEFINED PICO_BOARD)
    # For Pico builds, segregate by board name
    set(OBJC_LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/pico/${PICO_BOARD}")
else()
    # OS + architecture-specific library directory
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _LIB_DIR)
    if(NOT _LIB_DIR)
        set(_LIB_DIR "unknown")
    endif()
    set(OBJC_LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/${SYSTEM_NAME_LOWER}/${_LIB_DIR}")
endif()

# pkg-config generation (uses PROJECT_VERSION from top-level project)
set(OBJC_PKGCONFIG_NAME "picofuse")
set(OBJC_PKGCONFIG_DESCRIPTION "Objective-C runtime libraries (Foundation, Application, Network, Runtime components)")
set(OBJC_PKGCONFIG_LIBS "-L${CMAKE_INSTALL_PREFIX}/${OBJC_LIB_INSTALL_DIR} -lobjc-gcc -lFoundation -lApplication -lNetwork -lruntime-sys -lruntime-hw -lruntime-net -ldrivers")
set(OBJC_PKGCONFIG_CFLAGS "-I${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR} -I${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/runtime-${RUNTIME}")

# Determine pkg-config filename variant
if(DEFINED PICO_BOARD)
    set(_PKG_OS "pico")
    set(_PKG_ARCH "${PICO_BOARD}")
else()
    set(_PKG_OS "${SYSTEM_NAME_LOWER}")
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _PKG_ARCH)
    if(NOT _PKG_ARCH)
        set(_PKG_ARCH unknown)
    endif()
endif()
set(_PKGCONFIG_BASENAME "picofuse-${_PKG_OS}-${_PKG_ARCH}.pc")

configure_file(${CMAKE_SOURCE_DIR}/cmake/picofuse.pc.in
                             ${CMAKE_BINARY_DIR}/${_PKGCONFIG_BASENAME} @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${_PKGCONFIG_BASENAME}
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Header installation
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
          PATTERN "tests" EXCLUDE
          PATTERN "tests/*" EXCLUDE
          PATTERN "boards" EXCLUDE
          PATTERN "boards/*" EXCLUDE
          PATTERN "*.h")

# Public library targets (must exist prior to inclusion)
set(OBJC_INSTALL_TARGETS
    objc-gcc
    runtime-sys
    runtime-hw
    runtime-net
    drivers
    Foundation
    Application
    Network)

install(TARGETS ${OBJC_INSTALL_TARGETS}
        ARCHIVE DESTINATION ${OBJC_LIB_INSTALL_DIR}
        LIBRARY DESTINATION ${OBJC_LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
