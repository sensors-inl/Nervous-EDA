# RENFORCE EDA firmware

## Description
This project contains the firmware for the EDA Sensors of Franch ANR project RENFORCE.
The firmware targets nRF52 microcontrolers family and is developed upon nRF5 SDK 17.0.2.
Currently the nRF52840 together with s140_nrf52_7.2.0_softdevice.hex SoftDevice are used.

## Objectives

This code controls a custom analog frontend and acquires voltage and current signals from the frontend.
The frontend generate a current by using IDACs controlled with a software waveform.
This waveform contains multiple frequencies, and the absolute frequencies depend on the sampling rate, which is controlled by a clock which is provided in this code.
Current signal is actually converted by the frontend to a voltage signal by a TIA.

The device aims at :
- Measuring skin admittance in real-time over multiple frequencies from voltage/current signals, and send real / imaginary values of admittance (conductance / susceptance).
- Detecting electrodermal responses and physiological events.
- Being low-power!
- Being small ...
