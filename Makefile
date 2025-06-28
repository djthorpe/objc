BUILD_DIR ?= build
LLVM_TOOLCHAIN_PATH ?= /opt/LLVM-ET-Arm-19.1.5-Linux-x86_64
PICO_BOARD ?= pico
PICOTOOL_BUILD_DIR ?= ${BUILD_DIR}/third_party/picotool

.PHONY: test
test: submodule ${PICOTOOL_BUILD_DIR}/picotool
	@echo make test
	@cmake -B ${BUILD_DIR} -D PICO_BOARD=${PICO_BOARD} -D LLVM_TOOLCHAIN_PATH=${LLVM_TOOLCHAIN_PATH} -Dpicotool_DIR=${PICOTOOL_BUILD_DIR}
	@cmake --build ${BUILD_DIR} -v
	@echo "\nRun:\n  picotool load -x ${BUILD_DIR}/src/test/test.uf2\n"

.PHONY: picotool
picotool: ${PICOTOOL_BUILD_DIR}/picotool
	@echo "\nRun:\n  install -s ${PICOTOOL_BUILD_DIR}/picotool ${HOME}/bin\n"

${PICOTOOL_BUILD_DIR}/picotool: submodule
	@echo make picotool
	@PICO_SDK_PATH=../../../third_party/pico-sdk cmake -S third_party/picotool -B ${PICOTOOL_BUILD_DIR} -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -Wno-dev 
	@make -C ${BUILD_DIR}/third_party/picotool

.PHONY: submodule
submodule:
	@echo "Checking out submodules"
	@git submodule update --init --recursive 

.PHONY: clean
clean:
	@echo "Cleaning build directory"
	@rm -rf ${BUILD_DIR}
