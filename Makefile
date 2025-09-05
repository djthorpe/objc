# Installation prefix and build directory
BUILD_DIR ?= build
PREFIX ?= /opt/picofuse

# Tools
CMAKE ?= $(shell which cmake 2>/dev/null)
DOCKER ?= $(shell which docker 2>/dev/null)
GIT ?= $(shell which git 2>/dev/null)

# Build arguments
JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
MAKEFLAGS += --no-print-directory

# Pico variables - also PICO_BOARD
PICO_COMPILER ?= pico_arm_clang
TOOLCHAIN_PATH ?= /opt/LLVM-ET-Arm-19.1.5-Darwin-universal

# check for RELEASE=1
ifdef RELEASE
	CMAKE_BUILD_TYPE := Release
else
	CMAKE_BUILD_TYPE := Debug
endif

.PHONY: all
all: Foundation Application Network

# Configure
.PHONY: config
config: dep-cmake submodule
	@echo
	@echo configure
	@echo "PICO_BOARD=${PICO_BOARD} PICO_COMPILER=${PICO_COMPILER} TOOLCHAIN_PATH=${TOOLCHAIN_PATH}"
	@${CMAKE} -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		$(if ${PICO_BOARD},-D PICO_BOARD=${PICO_BOARD}) \
		-D PICO_COMPILER=${PICO_COMPILER} \
		-D PICO_TOOLCHAIN_PATH:PATH=${TOOLCHAIN_PATH} \
		-D RUNTIME=gcc
	@echo "Configured. Detected compilers:"
	@grep -E 'CMAKE_(C|CXX)_COMPILER:FILEPATH' ${BUILD_DIR}/CMakeCache.txt || true
	@if [ -n "${PICO_BOARD}" ]; then \
	  C_ACTUAL=$$(grep '^CMAKE_C_COMPILER:FILEPATH=' ${BUILD_DIR}/CMakeCache.txt | cut -d= -f2); \
	  case "$$C_ACTUAL" in \
	    ${TOOLCHAIN_PATH}/*) echo "Pico toolchain OK: $$C_ACTUAL";; \
	    *) echo "WARNING: Pico build expected compiler under ${TOOLCHAIN_PATH} but got $$C_ACTUAL";; \
	  esac; \
	fi

# Install
.PHONY: install
install: Foundation Application picotool
	@echo
	@echo install PREFIX=${PREFIX}
	@${CMAKE} -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		$(if ${PICO_BOARD},-D PICO_BOARD=${PICO_BOARD}) \
		-D PICO_COMPILER=${PICO_COMPILER} \
		-D PICO_TOOLCHAIN_PATH:PATH=${TOOLCHAIN_PATH} \
		-D RUNTIME=gcc \
		-D CMAKE_INSTALL_PREFIX=${PREFIX}
	@${CMAKE} --build ${BUILD_DIR} --target install -j ${JOBS}
	@install -d -m 755 ${PREFIX}/bin && install -m 755 ${BUILD_DIR}/third_party/picotool/picotool ${PREFIX}/bin

# Create the libruntime-sys runtime library
.PHONY: runtime-sys
runtime-sys: config
	@echo
	@echo make runtime-sys
	@${CMAKE} --build ${BUILD_DIR} --target runtime-sys -j ${JOBS}

# Create the libobjc-gcc runtime library
.PHONY: libobjc-gcc
libobjc-gcc: runtime-sys
	@echo
	@echo make libobjc-gcc
	@${CMAKE} --build ${BUILD_DIR} --target objc-gcc -j ${JOBS}

# Create the libruntime-pix runtime library
.PHONY: runtime-pix
runtime-pix: runtime-sys
	@echo
	@echo make runtime-pix
	@${CMAKE} --build ${BUILD_DIR} --target runtime-pix -j ${JOBS}

# Create the libruntime-hw runtime library
.PHONY: runtime-hw
runtime-hw: runtime-sys pioasm
	@echo
	@echo make runtime-hw
	@${CMAKE} --build ${BUILD_DIR} --target runtime-hw -j ${JOBS}


# Create the libruntime-net runtime library
.PHONY: runtime-net
runtime-net: runtime-sys
	@echo
	@echo make runtime-net
	@${CMAKE} --build ${BUILD_DIR} --target runtime-net -j ${JOBS}

# Create the libdrivers runtime library
.PHONY: drivers
drivers: runtime-hw
	@echo
	@echo make drivers
	@${CMAKE} --build ${BUILD_DIR} --target drivers -j ${JOBS}

# Create the Foundation library
.PHONY: Foundation
Foundation: libobjc-gcc runtime-sys
	@echo
	@echo make Foundation
	@${CMAKE} --build ${BUILD_DIR} --target Foundation -j ${JOBS}

# Create the Application library
.PHONY: Application
Application: Foundation runtime-hw drivers
	@echo
	@echo make Application
	@${CMAKE} --build ${BUILD_DIR} --target Application -j ${JOBS}

# Create the Network library
.PHONY: Network
Network: Foundation runtime-hw
	@echo
	@echo make Network
	@${CMAKE} --build ${BUILD_DIR} --target Network -j ${JOBS}

# Run the tests
.PHONY: tests
tests: config runtime-sys runtime-hw libobjc-gcc
	@echo
	@echo make tests
	@${CMAKE} --build ${BUILD_DIR}/src/tests -j ${JOBS}
	@${CMAKE} --build ${BUILD_DIR} --target test -j ${JOBS}

# Make the examples
.PHONY: examples
examples: runtime-sys runtime-hw 
	@echo
	@echo make examples
	@${CMAKE} --build ${BUILD_DIR}/src/examples -j ${JOBS}

# Generate the documentation
.PHONY: docs
docs: dep-docker 
	@echo
	@echo make docs
	@${DOCKER} run -v .:/data greenbone/doxygen doxygen /data/doxygen/Doxyfile

# Create the picotool binary
.PHONY: picotool
picotool: submodule dep-cmake
	@echo
	@echo make picotool
	@PICO_SDK_PATH=../../../third_party/pico-sdk ${CMAKE} -S third_party/picotool -B ${BUILD_DIR}/third_party/picotool -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -Wno-dev 
	@make -C ${BUILD_DIR}/third_party/picotool -j ${JOBS}
	@echo Run:
	@echo   install -s ${BUILD_DIR}/third_party/picotool/picotool ${HOME}/bin
	@echo

# Create the pioasm binary
.PHONY: pioasm
pioasm: submodule dep-cmake
	@echo
	@echo make pioasm
	@PICO_SDK_PATH=../third_party/pico-sdk ${CMAKE} -S third_party/pico-sdk/tools/pioasm -B ${BUILD_DIR}/pioasm -Wno-dev -D PIOASM_VERSION_STRING=0.0.0
	@make -C ${BUILD_DIR}/pioasm -j ${JOBS}
	@echo Built pioasm at ${BUILD_DIR}/pioasm/pioasm
	@echo

.PHONY: submodule
submodule: dep-git
	@echo
	@echo "checking out submodules"
	@${GIT} submodule update --init --recursive

.PHONY: clean
clean:
	@echo "Cleaning build directory"
	@rm -rf ${BUILD_DIR}

.PHONY: dep-cmake
dep-cmake:
	@test -f "${CMAKE}" && test -x "${CMAKE}" || (echo "Missing CMAKE: ${CMAKE}" && exit 1)

.PHONY: dep-docker
dep-docker:
	@test -f "${DOCKER}" && test -x "${DOCKER}" || (echo "Missing DOCKER: ${DOCKER}" && exit 1)

.PHONY: dep-git
dep-git:
	@test -f "${GIT}" && test -x "${GIT}" || (echo "Missing GIT: ${GIT}" && exit 1)
