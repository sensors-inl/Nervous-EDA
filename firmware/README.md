<p align="center">
    <h1 align="center">Nervous EDA Firmware</h1>
</p>

<p align="center">
    <img alt="C Language" src="https://img.shields.io/badge/C-00599C?logo=C&logoSize=auto" />
    <a href="https://www.nordicsemi.com/Products/Development-software/nRF5-SDK">
        <img alt="nRF5 SDK v17.1.0" src="https://img.shields.io/badge/v17.1.0-grey?label=nRF5%20SDK&labelColor=%23213D96" />
    </a>
    <a href="https://www.infineon.com/cms/en/design-support/tools/sdk/psoc-software/psoc-creator/">
        <img alt="PSoC Creator v4.4" src="https://img.shields.io/badge/v4.4-grey?label=PSoC%20Creator&labelColor=%23EE1A42" />
    </a>
    <a href="https://opensource.org/licenses/MIT">
        <img alt="License: MIT" src="https://img.shields.io/badge/License-MIT-yellow.svg" />
    </a>
</p>

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Overview](#overview)
- [Instructions to Build Firmware for the CY8C4A45PVI-481](#instructions-to-build-firmware-for-the-cy8c4a45pvi-481)
- [Instructions to Build Firmware for the nRF52840](#instructions-to-build-firmware-for-the-nrf52840)
  - [Dependencies](#dependencies)
  - [Build](#build)
- [Release](#release)

---

## Overview

As described in [Massot et al.](https://dx.doi.org/10.1109/JSEN.2024.3485187), the embedded electronic circuit employs two microcontrollers:

1. **CY8C4A45PVI-481**: A PSoC Analog Co-Processor (Infineon) used for generating multi-sinusoidal waveforms and conditioning the resulting voltage and current signals before sampling. Firmware development for this microcontroller was done using [PSoC Creator](https://www.infineon.com/cms/en/design-support/tools/sdk/psoc-software/psoc-creator/).  
   - Typically, PSoC firmware must be built and programmed using PSoC Creator, which runs on Microsoft Windows. However, the tool provides a Makefile export used in CI pipelines.  
   - The firmware can be programmed by building the project in PSoC Creator or directly using the pre-built firmware with [PSoC Programmer](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.psocprogrammer).

2. **nRF52840**: Found in the ISP1807 (Insight SiP) module, this microcontroller handles voltage and current sampling, impedance calculation, and Bluetooth Low Energy communication.  
   - Due to its unique peripheral-specific features (e.g., PPI, hardware-timer-based clock generation, dual-channel ADC with synchronous acquisition), the firmware was developed using the nRF5 SDK (now deprecated).  
   - Like the PSoC, the firmware for the nRF52840 is built in CI, and pre-built firmware binaries are available for download with each release.

---

## Instructions to Build Firmware for the CY8C4A45PVI-481

1. Download and install [PSoC Creator v4.4](https://www.infineon.com/cms/en/design-support/tools/sdk/psoc-software/psoc-creator/).
2. Use PSoC Creator to build and program the firmware onto the microcontroller.

---

## Instructions to Build Firmware for the nRF52840

These steps outline how to build the firmware using a Linux distribution. Adjustments may be required for Windows.

### Dependencies

Ensure the following tools are installed:

- [GNU ARM GCC Toolchain v5-2016-q2-update](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2)

```bash
cd ~
wget "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2"
mkdir ~/gcc-arm-none-eabi
tar xjf gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2 -C ~/gcc-arm-none-eabi --strip-components 1
```

- [nRF5 SDK v17.1.0](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v17.x.x/nRF5_SDK_17.1.0_ddde560.zip)

```bash
cd ~
wget "https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v17.x.x/nRF5_SDK_17.1.0_ddde560.zip"
unzip nRF5_SDK_17.1.0_ddde560.zip
```

- [nRF Util](https://www.nordicsemi.com/Products/Development-tools/nRF-Util)

```bash
cd ~
wget -q https://developer.nordicsemi.com/.pc-tools/nrfutil/x64-linux/nrfutil
mv nrfutil /usr/local/bin
chmod +x /usr/local/bin/nrfutil
nrfutil install nrf5sdk-tools
```

- [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nRF-Command-Line-Tools/Download)

```bash
cd ~
mkdir nrf-command-line-tools
wget -qO - "https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/desktop-software/nrf-command-line-tools/sw/versions-10-x-x/10-24-2/nrf-command-line-tools-10.24.2_linux-amd64.tar.gz" | tar --no-same-owner -xz -C ./nrf-command-line-tools --strip-components=2
export PATH="~/nrf-command-line-tools/bin:$PATH"
```

### Build

Clone this repository and build the nRF52 firmware from the `nrf52-packager` folder using the following command line :

```bash
make SDK_ROOT=~/nRF5_SDK_17.1.0_ddde560 \
     GNU_INSTALL_ROOT="~/gcc-arm-none-eabi/bin/" \
     GNU_VERSION="$(~/gcc-arm-none-eabi/bin/arm-none-eabi-gcc -dumpversion)" \
     VERSION_STRING=v1.0.0 \
     NAME_PREFIX=nervous-eda-firmware-nrf52
```

The full firmware and OTA binaries will be made available in the `nrf52-packager` folder.

---

## Release

Both firmwares are built continuously in CI at each [release](https://github.com/sensors-inl/Nervous-ECG/releases/latest).

---
