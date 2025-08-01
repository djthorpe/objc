BUILD_DIR ?= build
TOOLCHAIN_PATH ?= /usr
ARCH ?= $(shell arch | tr A-Z a-z | sed 's/amd64/x86_64/' | sed 's/armv7l/arm/' | sed 's/arm64/aarch64/')
OS ?= $(shell uname -s | tr A-Z a-z)
PLATFORM ?= $(shell uname | tr A-Z a-z | sed 's/linux/gnu/' | sed 's/darwin/apple/')
TARGET ?= ${ARCH}-${OS}-${PLATFORM}
CMAKE ?= $(shell which cmake 2>/dev/null)
DOCKER ?= $(shell which docker 2>/dev/null)
GIT ?= $(shell which git 2>/dev/null)
MAKEFLAGS += --no-print-directory

# check for RELEASE=1
ifdef RELEASE
	CMAKE_BUILD_TYPE := Release
else
	CMAKE_BUILD_TYPE := Debug
endif

.PHONY: all
all: NXFoundation

# Create the libobjc-gcc runtime library
.PHONY: libobjc-gcc
libobjc-gcc: dep-cc dep-cmake
	@echo
	@echo make libobjc-gcc
	@${CMAKE} -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		-D RUNTIME=gcc \
		-D TARGET=${TARGET}
	@${CMAKE} --build ${BUILD_DIR} --target objc-gcc


# Create the libobjc-gcc runtime library
.PHONY: runtime-pix
runtime-pix: dep-cc dep-cmake
	@echo
	@echo make runtime-pix
	@${CMAKE} -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		-D RUNTIME=gcc \
		-D TARGET=runtime-pix
	@${CMAKE} --build ${BUILD_DIR} --target runtime-pix

# Create the libobjc-gcc runtime library
.PHONY: NXFoundation
NXFoundation: libobjc-gcc
	@echo
	@echo make NXFoundation
	@${CMAKE} --build ${BUILD_DIR} --target NXFoundation

# Run the tests
.PHONY: tests
tests: NXFoundation
	@echo
	@echo make tests
	@${CMAKE} --build ${BUILD_DIR}/src/tests
	@${CMAKE} --build ${BUILD_DIR} --target test

# Make the examples
.PHONY: examples
examples: NXFoundation
	@echo
	@echo make examples
	@${CMAKE} --build ${BUILD_DIR}/src/examples

# Generate the documentation
.PHONY: docs
docs: dep-docker 
	@echo
	@echo make docs
	@${DOCKER} run -v .:/data greenbone/doxygen doxygen /data/doxygen/Doxyfile

# Cross-compile libraries for Raspberry Pi Pico
.PHONY: pico
pico: submodule dep-cmake
	@echo
	@echo make pico cross-compilation
	@${CMAKE} -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		-D RUNTIME=gcc \
		-D TARGET=armv6m-none-eabi
	@${CMAKE} --build ${BUILD_DIR} --target NXFoundation

# Create the picotool binary
.PHONY: picotool
picotool: submodule dep-cmake
	@echo
	@echo make picotool
	@PICO_SDK_PATH=../../../third_party/pico-sdk ${CMAKE} -S third_party/picotool -B ${BUILD_DIR}/third_party/picotool -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -Wno-dev 
	@make -C ${BUILD_DIR}/third_party/picotool
	@echo Run:
	@echo   install -s ${BUILD_DIR}/third_party/picotool/picotool ${HOME}/bin
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
