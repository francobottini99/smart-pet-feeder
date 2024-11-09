# Pet Food Charging Station

This project involves the development of a pet food charging station, designed for home use. The device incorporates a load cell to measure the weight of the food container and refill it when the level falls below 10 grams, a water level sensor to monitor hydration, and the ability to be controlled via a mobile application with Bluetooth connectivity.

### Authors:
- **Bottini, Franco Nicolas**
- **Robledo, Valentin**
- **Lencina, Aquiles Benjamín**

## Presentation Video
[Project Presentation](https://drive.google.com/file/d/11WiiX82fky_-kCaf6UjTl7HnfXC9PH7j/view?usp=drive_link)

## Project Details

### Prototype Components
- LPC1769 Cortex M3 development board.
- HC-05 Bluetooth module.
- HW-038 water level sensor.
- 20 Kg bar-type load cell with HX-711 analog-to-digital converter module.
- 4x4 matrix keyboard.
- 28BYJ-48 stepper motor with SBT0811 driver module.

### Used Peripherals
- 2 DMA channels.
- 1 ADC channel.
- 1 UART module.
- 4 timers in match mode.
- External interrupts via GPIO.

## Bluetooth Communication (HC-05)
Communication between the Bluetooth module and the development board is done via the UART protocol. DMA peripheral is used for both transmitting and receiving data. The data packets have a fixed size of 32 bytes, consisting of a package type identifier and the corresponding information, separated by ";".

## Water Level Sensor (HW-038)
This sensor provides an analog output that is digitized using the microcontroller's internal ADC. The ADC is configured with a maximum sampling frequency of 200 KHz, and a timer with a match set to 100 ms is used to obtain 10 measurements per second.

## Load Cell + HX-711
The load cell is read using the HX-711 analog-to-digital converter module. A timer is used to send pulses to the module's SCK pin, and the reading process is triggered with another timer, thus obtaining 10 measurements per second.

## 4x4 Matrix Keyboard
To read the matrix keyboard, an anti-bounce routine is implemented using a timer with a match set to 2 ms.

## Stepper Motor (28BYJ-48)
The stepper motor's rotation is controlled via a timer with a match set to 1 ms, allowing a 90° rotation in approximately 1 second.

## Water Level Sensor Simulation
Due to technical issues with the HW-038 water level sensor, its functionality was simulated using a potentiometer.
