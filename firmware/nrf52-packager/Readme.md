# Packager for Bruxense

This is a convenient Makefile to help generating compatible bootloader settings with firmware, flashing the device and generating DFU packages for OTA updates.

The Nordic Secure Bootloader ensure that a valid application is installed on the device, or it won't boot.
To do so, at each boot a CRC is computed by the bootloader upon firmware and compared to a pre-calculated CRC recorded in the bootloader settings.
This tools generates these bootloader settings and merge hex files to flash together the firmware, the bootloader and the bootloader settings.
This ensures that the application will boot after each flash.

example :

```bash
make clean
make VERSION_STRING=0.2.7
make flash VERSION_STRING=0.2.7
```

## Configuration
- Configure firmware and bootloader project pathes in the Makefile (`APP_DIR` and `BTL_DIR`)
- Configure firmware, bootloader and bootloader settings versions (should be incremented each time)

## List of available Make targets
### main targets
- `bin` : Build all, generate settings and merge in .hex file
- `app_package_generate` : generate an OTA package for BLE app
### build targets
- `build_firmware`, `rebuild_firmware` : Build the firmware
- `build_bootlader` : Build the bootloader
- `build_all` : Build both firmware and bootloader
- `clean_all` : Clean firmware and bootloader build folders
- `rebuild_all` : Clean and build firmware and bootloader
### bin helpers
- `generate_settings` : Generate bootlader settings
- `merge_all` : Merge firmware, bootloader and bootloader settings in a single .hex file
### flash helpers
- `flash_merged` : Flash merged .hex file to the device
- `flash_softdevice` : Flash only the softdevice
