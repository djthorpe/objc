BUILD_DIR ?= build
CMAKE ?= $(shell which cmake 2>/dev/null)
DOCKER ?= $(shell which docker 2>/dev/null)
GIT ?= $(shell which git 2>/dev/null)
JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
MAKEFLAGS += --no-print-directory

# check for RELEASE=1
ifdef RELEASE
	CMAKE_BUILD_TYPE := Release
else
	CMAKE_BUILD_TYPE := Debug
endif

.PHONY: all
all: config runtime-sys runtime-hw

# Configure
config: dep-cmake submodule
	@echo
	@echo configure
	@${CMAKE} -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		-D PICO_BOARD=${PICO_BOARD} \
		-D RUNTIME=gcc

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

# Create the libdrivers runtime library
.PHONY: drivers
drivers: runtime-hw
	@echo
	@echo make drivers
	@${CMAKE} --build ${BUILD_DIR} --target drivers -j ${JOBS}

# Create the NXFoundation library
.PHONY: NXFoundation
NXFoundation: libobjc-gcc runtime-sys
	@echo
	@echo make NXFoundation
	@${CMAKE} --build ${BUILD_DIR} --target NXFoundation -j ${JOBS}

# Create the NXApplication library
.PHONY: NXApplication
NXApplication: NXFoundation runtime-hw drivers
	@echo
	@echo make NXApplication
	@${CMAKE} --build ${BUILD_DIR} --target NXApplication -j ${JOBS}

# Run the tests
.PHONY: tests
tests: config runtime-sys runtime-hw
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

# Cross-compile libraries for Raspberry Pi Pico
.PHONY: pico
# The Pico cross-compile target now ensures local picotool and pioasm are built first
# to avoid SDK corruption issues. Set NO_LOCAL_PICOTOOL=1 to skip using locally built tools.
pico: submodule dep-cmake $(if $(NO_LOCAL_PICOTOOL),,picotool pioasm)
	@echo
	@echo make pico cross-compilation
	@${CMAKE} -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		-D RUNTIME=gcc \
		$(if $(NO_LOCAL_PICOTOOL),,-D picotool_DIR=${BUILD_DIR}/third_party/picotool) \
		-D TARGET=armv6m-none-eabi \
		-D CMAKE_TOOLCHAIN_FILE=${PWD}/cmake/armv6m-none-eabi.cmake
	@${CMAKE} --build ${BUILD_DIR} --target NXApplication -j ${JOBS}

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
	@git submodule update --init --recursive

.PHONY: clean
clean:
	@echo "Cleaning build directory"
	@rm -rf ${BUILD_DIR}

.PHONY: dep-cc
dep-cc:
	@test -f "${TOOLCHAIN_PATH}/bin/${CC}" && test -x "${TOOLCHAIN_PATH}/bin/${CC}" || (echo "Missing CC: ${TOOLCHAIN_PATH}/bin/${CC}" && exit 1)

.PHONY: dep-cmake
dep-cmake:
	@test -f "${CMAKE}" && test -x "${CMAKE}" || (echo "Missing CMAKE: ${CMAKE}" && exit 1)

.PHONY: dep-docker
dep-docker:
	@test -f "${DOCKER}" && test -x "${DOCKER}" || (echo "Missing DOCKER: ${DOCKER}" && exit 1)

.PHONY: dep-git
dep-git:
	@test -f "${GIT}" && test -x "${GIT}" || (echo "Missing GIT: ${GIT}" && exit 1)
