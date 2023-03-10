all: build

VERSION := $(shell if yt --version 2>/dev/null; then yt --version 2>&1; else echo; fi)
ifeq ($(MEDIA_DIR),)
	MEDIA_DIR := $(shell if [ -d /media ]; then echo "/media"; elif [ -d /run/media ]; then echo "/run/media"; else echo ""; fi)
endif

check-deps:
ifeq ($(MEDIA_DIR),)
	@echo media dir not found, please set MEDIA_DIR to the parent of the MICROBIT mounted directory
	@false
endif
ifeq ($(VERSION),)
	@echo you should install yotta before trying anything
	@echo "(with nix you can use github.com/joxcat/yotta)"
	@false
endif

INCLUDES := include/asenum/include/asenum \
		include/hana/include \
		include/PEGTL/include \
		include/curve25519/include \
		yotta_modules/microbit/inc \
		yotta_modules/microbit-dal/inc/core \
		yotta_modules/microbit-dal/inc/platform \
		yotta_modules/microbit-dal/inc/drivers \
		yotta_modules/microbit-dal/inc/types \
		yotta_modules/microbit-dal/inc/bluetooth \
		yotta_modules/ble \
		yotta_modules/mbed-classic/api \
		yotta_modules/mbed-classic/hal \
		yotta_modules/mbed-classic/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822 \
		yotta_modules/mbed-classic/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/TARGET_NRF51_MICROBIT \
		yotta_modules/mbed-classic/targets/cmsis \
		yotta_modules/mbed-classic/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822

check: check-deps
	clang-tidy src/main.cpp -- $(shell for folder in $(INCLUDES); do echo "-I$$folder"; done)

build: check-deps
	@yt build

install: check-deps
	@cp build/bbc-microbit-classic-gcc/src/pfp-combined.hex /run/media/$(USER)/MICROBIT/
	@echo "Install done"

autoinstall: check-deps
ifeq ($(ID),)
	sudo mount --mkdir -o "gid=$(shell id -u),uid=$(shell id -g)" /dev/disk/by-label/MICROBIT /run/media/$(USER)/MICROBIT
else
	sudo mount --mkdir -o "gid=$(shell id -u),uid=$(shell id -g)" /dev/disk/by-id/$(ID) /run/media/$(USER)/MICROBIT
endif
	@cp build/bbc-microbit-classic-gcc/src/pfp-combined.hex /run/media/$(USER)/MICROBIT/
	@echo "Install done"
	sudo umount /run/media/$(USER)/MICROBIT
	sudo rm -rf /run/media/$(USER)/MICROBIT

autodeploy: check-deps
	@echo "WARNING: Deploy to predefined microbits won't work with different devices (id identified)"
	make autoinstall ID="usb-MBED_VFS_9901000051774e4500747019000000280000000097969901-0:0"
	make autoinstall ID="usb-MBED_VFS_9901000051114e45004780170000004e0000000097969901-0:0"

count-lines:
	tokei -e include/asenum -e include/doctest.h src include

clean: check-deps
	@yt clean
	@echo "Cleaning done"
