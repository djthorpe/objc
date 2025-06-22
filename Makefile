BUILD_DIR ?= build
LLVM_TOOLCHAIN_PATH ?= /opt/LLVM-ET-Arm-19.1.5-Linux-x86_64
PICO_BOARD ?= pico

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
