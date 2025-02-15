# Example :
# make VERSION_STRING=0.2.7
# SDK location and GNU toolchain directories can be specified :
# make SDK_ROOT="the/sdk/root/directory" GNU_INSTALL_ROOT="the/arm-none-eabi-gcc/directory" GNU_VERSION="the version" VERSION_STRING=0.2.7

# App Version should be incremented on each release
VERSION_STRING ?= 0.0.0
# Remove existing 'v' in version string for file naming
APP_VERSION_STRING := $(subst v,,$(VERSION_STRING))
# Remove existing '.' in version string to create bootloader application version
TMP_APP_VERSION = $(subst .,,$(APP_VERSION_STRING))
# Remove existing '0' for major version number
APP_VERSION ?= $(TMP_APP_VERSION:0%=%)
# Output nanme prefix
NAME_PREFIX ?= nervous-eda-firmware-nrf52

# Merged hex
MRG_HEX := $(NAME_PREFIX)-full-$(VERSION_STRING).hex

# App package
APP_PKG = $(NAME_PREFIX)-ota-$(VERSION_STRING).zip

DEVICE := nrf52840

SDK_ROOT ?= C:/ncs/nRF5_SDK_17.1.0
APP_DIR := ../nrf52-firmware
BTL_DIR := ../nrf52-bootloader

# To read softdevice version, use command 'nrfjprog --memrd 0x300C', last 4 four nibbles are SOFTDEVICE_ID
ifeq ($(DEVICE), nrf52832)
	FAMILY := NRF52
	BOARD_TYPE := pca10040
	SOFT_DEVICE := s132
	# 0x00B7 is s132_nrf52_6.1.1
	# 0x00CB is s132_nrf52_7.0.1
	# 0x0101 is s132_nrf52_7.2.0
	SOFTDEVICE_ID := 0x0101
else ifeq ($(DEVICE), nrf52840)
	FAMILY := NRF52840
	BOARD_TYPE := pca10056
	SOFT_DEVICE := s140
	# 0x00CA is s140_nrf52_7.0.1
	# 0x0100 is s140_nrf52_7.2.0
	SOFTDEVICE_ID := 0x0100
endif

COMPILER_TYPE := armgcc
DEVICE_ARCH := nrf52hf

# SoftDevice location
SDV_HEX = $(SDK_ROOT)/components/softdevice/$(SOFT_DEVICE)/hex/$(SOFT_DEVICE)_nrf52_7.2.0_softdevice.hex

# App location
APP_HEX := $(APP_DIR)/$(BOARD_TYPE)/$(SOFT_DEVICE)/$(COMPILER_TYPE)/_build/$(DEVICE)_xxaa.hex

# Bootloader version
BTL_VERSION := 1
# Bootloader location
BTL_HEX := $(BTL_DIR)/$(BOARD_TYPE)_$(SOFT_DEVICE)_ble/$(COMPILER_TYPE)/_build/$(DEVICE)_xxaa_$(SOFT_DEVICE).hex

# Bootlader settings
BTL_SETTINGS_VERSION := 1
BTL_SETTINGS_HEX := bootloader_settings.hex

NRF_DFU_HW_VERSION := 52

all: bin ota

build_firmware:
	@echo .
	@echo ----------------------------------------------
	@echo ---------- Build firmware --------------------
	@echo ----------------------------------------------
	make APP_VERSION_STRING=$(APP_VERSION_STRING) -C $(APP_DIR)/$(BOARD_TYPE)/$(SOFT_DEVICE)/$(COMPILER_TYPE)

build_bootloader:
	@echo .
	@echo ----------------------------------------------
	@echo ---------- Build bootloader ------------------
	@echo ----------------------------------------------
	make -C $(BTL_DIR)/$(BOARD_TYPE)_$(SOFT_DEVICE)_ble/$(COMPILER_TYPE)

build_all: build_firmware build_bootloader

clean:
	@echo .
	@echo ----------------------------------------------
	@echo ---------- Clean all -------------------------
	@echo ----------------------------------------------
	make -C $(APP_DIR)/$(BOARD_TYPE)/$(SOFT_DEVICE)/$(COMPILER_TYPE) clean
	make -C $(BTL_DIR)/$(BOARD_TYPE)_$(SOFT_DEVICE)_ble/$(COMPILER_TYPE) clean

generate_settings:
	@echo .
	@echo ----------------------------------------------
	@echo ---------- Generate bootloader settings ------
	@echo ----------------------------------------------
	nrfutil settings generate --family $(FAMILY) --application $(APP_HEX) --application-version $(APP_VERSION) --bootloader-version $(BTL_VERSION) --bl-settings-version $(BTL_SETTINGS_VERSION) $(BTL_SETTINGS_HEX)

merge:
	@echo .
	@echo ----------------------------------------------
	@echo ---------- Merge hex files -------------------
	@echo ----------------------------------------------
	mergehex -m $(APP_HEX) $(BTL_HEX) $(BTL_SETTINGS_HEX) -o $(MRG_HEX)
	mergehex -m $(SDV_HEX) $(MRG_HEX) -o $(MRG_HEX)

bin: build_firmware build_bootloader generate_settings merge

ota: build_firmware
	@echo .
	@echo ----------------------------------------------
	@echo ---------- Generate application DFU package --
	@echo ----------------------------------------------
	nrfutil pkg generate --hw-version $(NRF_DFU_HW_VERSION) --application-version $(APP_VERSION) --application $(APP_HEX) --sd-req $(SOFTDEVICE_ID) --key-file $(BTL_DIR)/dfu_private_key.pem $(APP_PKG)

flash:
	@echo .
	@echo ----------------------------------------------
	@echo ---------- Flash merged hex ------------------
	@echo ----------------------------------------------
	nrfjprog --recover
	nrfjprog -f nrf52 --program $(MRG_HEX) --chiperase --verify
	nrfjprog -f nrf52 --reset