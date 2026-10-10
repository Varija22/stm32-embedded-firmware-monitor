# STM32 Embedded Firmware Monitor

An embedded firmware project built using the STM32F401RE microcontroller to monitor analog sensor inputs using ADC, transmit readings through UART, and provide threshold-based alerts using an LED.

## Overview

The STM32 Embedded Firmware Monitor demonstrates basic embedded systems concepts, including analog-to-digital conversion, UART serial communication, GPIO control, and threshold-based monitoring.

The firmware periodically reads an analog input, converts the ADC reading into a voltage value, compares it against a predefined threshold, and reports the results through UART.

## Features

- **ADC Monitoring:** Reads analog sensor input using a 12-bit ADC.
- **Voltage Conversion:** Converts ADC readings into millivolts.
- **UART Telemetry:** Transmits ADC values, calculated voltage, and status messages over UART.
- **Threshold Alerts:** Reports `WARNING` when the ADC value is greater than or equal to 2000; otherwise, reports `NORMAL`.
- **LED Indication:** Turns the LED on when the threshold is reached and off when the reading is below the threshold.
- **Error Handling:** Reports ADC conversion and UART message-formatting errors where applicable.

## Hardware Requirements

- STM32 Nucleo board based on STM32F401RE
- Analog sensor or potentiometer
- USB cable for programming and serial communication
- Jumper wires, if required

## Software Requirements

- STM32CubeIDE
- STM32Cube HAL drivers
- Git
- A serial terminal for UART monitoring

## Configuration

| Parameter | Value |
|---|---|
| Microcontroller | STM32F401RE |
| ADC resolution | 12-bit |
| ADC input | ADC1, Channel 0 (PA0) |
| ADC range | 0–4095 |
| Reference voltage used in calculation | 3300 mV |
| ADC threshold | 2000 |
| UART peripheral | USART2 |
| UART baud rate | 115200 |
| UART format | 8 data bits, no parity, 1 stop bit |
| Reporting interval | 500 ms |
| LED output | GPIOA, Pin 5 (PA5) |

*Note: The ADC voltage calculation assumes a 3.3 V reference. Actual readings depend on the board's analog reference and input configuration.*

## How It Works

1. The microcontroller initializes the HAL, system clock, GPIO, ADC, and UART peripherals.
2. The ADC samples the analog input.
3. The firmware converts the ADC count into an approximate voltage.
4. The reading is compared with the threshold of 2000.
5. The LED and status message are updated according to the threshold.
6. UART transmits the ADC reading, voltage, and status every 500 milliseconds.

### Example UART Output

```text
STM32 Embedded Firmware Monitor
ADC threshold status monitoring started.
Threshold: 2000 ADC counts

ADC: 1200 | Voltage: 0.967 V | Status: NORMAL
ADC: 2500 | Voltage: 2.014 V | Status: WARNING
```

*The readings above are illustrative examples, not recorded test results.*

## Project Structure

```text
stm32-embedded-firmware-monitor/
├── Core/
│   ├── Inc/
│   ├── Src/
│   │   └── main.c
│   └── Startup/
├── Drivers/
│   ├── CMSIS/
│   └── STM32F4xx_HAL_Driver/
├── .settings/
├── STM32F401RETX_FLASH.ld
├── STM32F401RETX_RAM.ld
├── OST.ioc
├── .project
├── .cproject
├── .gitignore
├── LICENSE
└── README.md
```

## Build and Run

1. Clone the repository:

   ```bash
   git clone https://github.com/Varija22/stm32-embedded-firmware-monitor.git
   ```

2. Open STM32CubeIDE.
3. Import the cloned project into the workspace.
4. Confirm the target microcontroller and peripheral configuration.
5. Build the project.
6. Connect the STM32 board and flash the firmware.
7. Open a serial terminal configured for 115200 baud, 8-N-1.
8. Apply a suitable analog input to the configured ADC pin and observe the UART output and LED behavior.

**Important:** The board's ADC input must remain within its permitted voltage range. Verify the peripheral configuration and pin assignments before testing.

## Testing

The project has been built and tested using STM32CubeIDE. Hardware verification confirmed the expected LED response to the ADC threshold and UART reporting of ADC readings, voltage, and status.

## Contribution Workflow

The project uses Git and GitHub for version control and collaboration.

1. Create a feature branch.
2. Implement and commit the changes.
3. Push the feature branch to GitHub.
4. Open a pull request against `main`.
5. Review and test the proposed changes.
6. Merge the pull request after approval.

## Future Improvements

- Add support for additional analog sensors.
- Make the threshold configurable.
- Add ADC averaging or filtering to reduce noise.
- Introduce structured UART error reporting.
- Add automated build checks.

## License

This project is distributed under the MIT License. See the [LICENSE](LICENSE) file for details.
