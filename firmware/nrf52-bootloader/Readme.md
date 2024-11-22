# Bootloader for RENFORCE EDA

## Prerequisites to do once

### Generate the keys if not present in the project directory

- Install nrfutil
  - From official page <https://www.nordicsemi.com/Products/Development-tools/nRF-Util/Download#infotabs>
- Install sdk tools
  - `nrfutil install nrf5sdk-tools`
- Generate public and private keys in the project directory
  - `nrfutil keys generate ./dfu_private_key.pem`
  - `nrfutil keys display --key pk --format code ./dfu_private_key.pem --out_file ./dfu_public_key.c`

### Install full micro_ecc library in the SDK
- Add micro_ecc source code to the SDK in `$SDK_DIR$/external/micro-ecc`
  - `git clone https://github.com/kmackay/micro-ecc.git`
- Build micro_ecc library in  `$SDK_DIR$/external/micro-ecc/nrf52hf_armgcc/armgcc`
  - `make`

## Configure VSCode to compile and debug

A few preliminary steps are necessary in order to make and debug from VSCode :
-  Configure the paths in `nrf_vscode.sh` to match those of your system.
-  Install the Cortex-Debug and the C/C++ Intellisens VSCode extensions.
-  Always run VScode by launching the batch script (`./nrf_vscode.sh`). It will configure the environment variables so they can then be used in the configuration files.

## Using tasks

In order to build, flash or clean the project, use the relevant tasks from Terminal > Run Task

## Debugging

To debug, use Run > Start Debugging. It will automatically show the debug perspective.

