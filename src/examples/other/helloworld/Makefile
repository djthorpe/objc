BUILD_DIR ?= build
# Default board (override when invoking make if desired)
PICO_BOARD ?= pimoroni_pico_lipo2xl_w
JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Allow overriding build type: set RELEASE=1 for Release
ifdef RELEASE
	CMAKE_BUILD_TYPE := Release
else
	CMAKE_BUILD_TYPE := Debug
endif

.PHONY: all configure build flash clean

all: build

configure:
	@echo "[configure] PICO_BOARD=${PICO_BOARD} (clean configure)"
	@rm -rf ${BUILD_DIR}
	@cmake -B ${BUILD_DIR} -Wno-dev \
		-D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		-D PICO_BOARD=${PICO_BOARD} \
		-D PICO_SDK_PATH=${PWD}/../../../../third_party/pico-sdk \
		-S ${PWD}

build: configure
	@echo "[build] hello_usb"
	@cmake --build ${BUILD_DIR} --target hello_usb -j ${JOBS}
	@echo "UF2: $$(ls ${BUILD_DIR}/hello_usb.uf2 2>/dev/null || echo 'not generated')"

flash: build
	@test -f "${BUILD_DIR}/hello_usb.uf2" || (echo "No UF2 generated" && exit 1)
	@echo "[flash] Using picotool to load hello_usb.uf2"
	@picotool load -f -x ${BUILD_DIR}/hello_usb.uf2

clean:
	@echo "[clean] Removing ${BUILD_DIR}"
	@rm -rf ${BUILD_DIR}
