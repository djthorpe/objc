BUILD_DIR ?= build


.PHONY: test
test: submodule
	@echo make test
	@cmake -B ${BUILD_DIR}
	@cmake --build ${BUILD_DIR}
	@echo "\nRun:\n  picotool load ${BUILD_DIR}/src/test2/test2.uf\n"

.PHONY: picotool
picotool: submodule
	@echo make picotool
	@PICO_SDK_PATH=../../../third_party/pico-sdk cmake -S third_party/picotool -B ${BUILD_DIR}/third_party/picotool -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -Wno-dev 
	@make -C ${BUILD_DIR}/third_party/picotool
	@echo "\nRun:\n  install -s ${BUILD_DIR}/third_party/picotool/picotool ${HOME}/bin\n"

# Submodule checkout
.PHONY: submodule
submodule:
	@echo "Checking out submodules"
	@git submodule update --init --recursive 
