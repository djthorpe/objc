BUILD_DIR ?= build
BUILD_TYPE ?= Debug
LLVM_TOOLCHAIN_PATH ?= /opt/LLVM-ET-Arm-19.1.5-Linux-x86_64
PICO_BOARD ?= pico
PICOTOOL_BUILD_DIR ?= ${BUILD_DIR}/third_party/picotool

.PHONY: tests
tests: submodule picotool
	@echo make test
	@cmake -B ${BUILD_DIR} -DPICO_BOARD=${PICO_BOARD} -DLLVM_TOOLCHAIN_PATH=${LLVM_TOOLCHAIN_PATH} -Dpicotool_DIR=${PICOTOOL_BUILD_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
	@cmake --build ${BUILD_DIR}

.PHONY: picotool
picotool: ${PICOTOOL_BUILD_DIR}/picotool
	@echo "\nRun:\n  install -s ${PICOTOOL_BUILD_DIR}/picotool ${HOME}/bin\n"

${PICOTOOL_BUILD_DIR}/picotool: submodule
	@echo make picotool
	@PICO_SDK_PATH=../../../third_party/pico-sdk cmake -S third_party/picotool -B ${PICOTOOL_BUILD_DIR} -Wno-dev 
	@cmake --build ${PICOTOOL_BUILD_DIR}

.PHONY: submodule
submodule:
	@echo "Checking out submodules"
	@git submodule update --init --recursive 

.PHONY: clean
clean:
	@echo "Cleaning build directory"
	@rm -rf ${BUILD_DIR}
