BUILD_DIR ?= build
TOOLCHAIN_PATH ?= /usr
ARCH ?= $(shell arch | tr A-Z a-z | sed 's/x86_64/amd64/' | sed 's/i386/amd64/' | sed 's/armv7l/arm/' | sed 's/aarch64/arm64/')
OS ?= $(shell uname | tr A-Z a-z)
PLATFORM ?= $(shell uname | tr A-Z a-z | sed 's/linux/gnu/' | sed 's/darwin/apple/')
TARGET ?= ${ARCH}-${OS}-${PLATFORM}
CC ?= clang
CMAKE ?= $(shell which cmake 2>/dev/null)

all: tests

.PHONY: tests
tests: dep-cc dep-cmake
	@echo make tests
	@cmake -B ${BUILD_DIR} -Wno-dev
	@cmake --build ${BUILD_DIR}

.PHONY: test
test: submodule
	@echo make test
	@cmake -B ${BUILD_DIR} -D PICO_BOARD=${PICO_BOARD} -D LLVM_TOOLCHAIN_PATH=${LLVM_TOOLCHAIN_PATH}
	@cmake --build ${BUILD_DIR}
	@echo "\nRun:\n  picotool load -x ${BUILD_DIR}/src/test/test.uf2\n"

.PHONY: picotool
picotool: submodule
	@echo make picotool
	@PICO_SDK_PATH=../../../third_party/pico-sdk cmake -S third_party/picotool -B ${BUILD_DIR}/third_party/picotool -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -Wno-dev 
	@make -C ${BUILD_DIR}/third_party/picotool
	@echo "\nRun:\n  install -s ${BUILD_DIR}/third_party/picotool/picotool ${HOME}/bin\n"

.PHONY: submodule
submodule:
	@echo "Checking out submodules"
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
