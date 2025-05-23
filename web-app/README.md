<p align="center">
    <h1  align="center">Nervous EDA Application</h1>
</p>

<p align="center">
    <img alt="HTML5" src="https://img.shields.io/badge/HTML5-%23E34F26?logo=HTML5&logoColor=white">
    <img alt="JavaScript" src="https://img.shields.io/badge/JAVASCRIPT-%23323330?logo=javascript&logoColor=%23F7DF1E">
    <a href="https://opensource.org/licenses/MIT">
        <img alt="License: MIT" src="https://img.shields.io/badge/License-MIT-yellow.svg" />
    </a>
</p>

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Overview](#overview)
- [Usage](#usage)
- [Building Executable Binaries for Windows and Linux](#building-executable-binaries-for-windows-and-linux)
  - [Prerequisites](#prerequisites)
  - [Build Instructions](#build-instructions)
- [Release](#release)

---

## Overview

![Web application of the Nervous EDA sensor](../assets/nervous-eda-web-app.png "Web application of the Nervous ECG sensor")

This repository contains an HTML/JavaScript application that connects to Nervous EDA sensors. The application provides real-time Nyquist plot of the impedance spectroscopy
with simple fit of the data using a circular model. The application also plots impedance modulus for each of the 16 frequencies acquired. It can saves data to a `.csv` file. It consists of a single static HTML page, `index.html`, along with two JavaScript files: `app.js` (the main application script) and `proto.js` (the JavaScript compilation of the Protocol Buffer files).

This application uses several libraries. For offline usage, local copies of these libraries are included. The following versions of the libraries are currently used:

- [**Chart.js**](https://www.chartjs.org/) (v4.4.0)
- [**google-protobuf**](https://www.npmjs.com/package/google-protobuf) (v3.19.0)
- [**hammerjs**](https://hammerjs.github.io/) (v2.0.8)
- [**jsxgraph**](https://jsxgraph.uni-bayreuth.de/wp/index.html) (v1.3.2)
- [**material-components-web**](https://www.npmjs.com/package/material-components-web) (v14.0.0)
- [**cobs**](https://www.npmjs.com/package/cobs) (v0.2.1)

Additional configuration files, such as `forge.config.js`, `jsconfig.json`, `main.js`, and `package.js`, are provided to facilitate building the application into an executable using [**Electron**](https://www.electronjs.org/) and [**Electron Forge**](https://www.electronforge.io/). This allows you to build executables for Windows and Linux by following the instructions below.

---

## Usage

To use the application, simply open `index.html` in a Chromium-based web browser (e.g., Chrome, Microsoft Edge). 

1. Press the **CONNECT** button to scan for available ECG sensors.
2. Once connected, press the **START** button to begin collecting and plotting data in real-time.
3. Press the **STOP** button to stop the data collection and save the data to a `.csv` file.

Since the application runs in the web browser, the `.csv` file will be automatically downloaded, but the entire application runs locally in the browser.

> **Note:** When using the built executable (see below), the application will automatically connect to the first available sensor rather than displaying a list of available sensors.

---

## Building Executable Binaries for Windows and Linux

This application can be built into executables for Windows and Linux using Electron.

### Prerequisites

Before building the application, the following dependencies must be installed:

- [Node.js and npm](https://docs.npmjs.com/downloading-and-installing-node-js-and-npm) (required to install and use Electron and Electron Forge)
- Install Electron: `npm install --save-dev electron`
- Install Electron Forge: `npm install --save-dev @electron-forge/cli`

### Build Instructions

1. Prepare the build environment:

    ```console
    npx electron-forge import
    ```

2. Package the build, specifying the architecture and OS. For example, for Windows, use:

    ```console
    npx electron-forge package --arch=x64 --platform=win32
    ```

    For Linux, use:

    ```console
    npx electron-forge package --arch=x64 --platform=linux
    ```

3. The packaged binaries will be located in the `out/` directory.

---

## Release

Executable binaries for Windows and Linux are automatically built in CI and are available for download as `.zip` archives from each [release](https://github.com/sensors-inl/Nervous-EDA/releases/latest).

---
