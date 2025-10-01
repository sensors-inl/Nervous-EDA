# Changelog

## [1.1.2] - 2025-10-01

### Hardware

- JTAG connectors footprint and designator update

### Software - web

- Add device name in exported CSV file
- Add secondary history chart for 724 Hz

## [1.1.1] - 2025-03-20

### Hardware

- Bump KiCad to version 9.0.0
- Adjust decoupling capacitors positioning

## [1.1.0] - 2025-02-15

- Use 32.768kHz clock instead of CPU clock for signal generation and sampling. This is much more precise to obtain the 4096 Hz sampling rate
- Apply a correction factor on phase for each frequency to compensate for DAC/ADC delay. This significantly reduces error on real on imaginary parts

## [1.0.0] - 2024-11-25

🎉 _First release !_
