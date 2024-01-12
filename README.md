# Estación de Carga de Alimento para Mascotas

Este proyecto consiste en el desarrollo de una estación de carga de alimento para mascotas, diseñada para su uso en el hogar. El dispositivo incorpora una celda de carga para medir el peso del recipiente de alimento y rellenarlo cuando el nivel caiga por debajo de 10 gramos, un sensor de nivel de agua para controlar la hidratación, y la capacidad de ser controlado a través de una aplicación móvil con conectividad Bluetooth.

### Autores:
- **Bottini, Franco Nicolas**
- **Robledo, Valentin**
- **Lencina, Aquiles Benjamín**

## Video de Presentación
[Presentación del Proyecto](https://drive.google.com/file/d/11WiiX82fky_-kCaf6UjTl7HnfXC9PH7j/view?usp=drive_link)

## Detalles del Proyecto

### Elementos del Prototipo
- Placa de desarrollo LPC1769 Cortex M3.
- Módulo Bluetooth HC-05.
- Sensor de nivel de agua HW-038.
- Celda de carga tipo barra de 20 Kg con módulo conversor analógico-digital HX-711.
- Teclado matricial 4x4.
- Motor paso a paso 28BYJ-48 con módulo driver SBT0811.

### Periféricos Utilizados
- 2 canales de DMA.
- 1 canal de ADC.
- 1 módulo UART.
- 4 timers en modo match.
- Interrupciones externas por GPIO.

## Comunicación Bluetooth (HC-05)
La comunicación entre el módulo Bluetooth y la placa de desarrollo se realiza mediante el protocolo UART. Se implementa el uso del periférico DMA tanto para la transmisión como para la recepción de información. Los paquetes de datos tienen un tamaño fijo de 32 bytes, compuestos por un identificador de tipo de paquete y la información correspondiente, separados por ";".

## Sensor de Nivel de Agua (HW-038)
Este sensor proporciona una salida analógica que se digitaliza utilizando el ADC interno del microcontrolador. Se configura el ADC con una frecuencia de muestreo máxima de 200 KHz y se utiliza un timer con un match configurado a 100 ms para obtener 10 mediciones por segundo.

## Celda de Carga + HX-711
La lectura de la celda de carga se realiza utilizando el módulo conversor analógico-digital HX-711. Se emplea un timer para enviar impulsos al pin SCK del módulo, y se lanza el proceso de lectura con otro timer, obteniendo así 10 mediciones por segundo.

## Teclado Matricial 4x4
Para la lectura del teclado matricial se implementa una rutina de anti-rebote utilizando un timer con un match configurado en 2 ms.

## Motor Paso a Paso (28BYJ-48)
El giro del motor paso a paso se controla mediante un timer con un match configurado en 1 ms, permitiendo un giro aproximado de 90° en 1 segundo.

## Simulación del Sensor de Nivel de Agua
Debido a problemas técnicos con el sensor de nivel de agua HW-038, se optó por simular su funcionamiento mediante un potenciómetro.