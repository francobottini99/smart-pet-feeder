################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ADCHandler.c \
../src/Bluetooth.c \
../src/DMAHandler.c \
../src/Keyboard.c \
../src/LoadCell.c \
../src/PetFeeder.c \
../src/StepMotor.c \
../src/TIMHandler.c \
../src/Utils.c \
../src/WaterSensor.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c 

C_DEPS += \
./src/ADCHandler.d \
./src/Bluetooth.d \
./src/DMAHandler.d \
./src/Keyboard.d \
./src/LoadCell.d \
./src/PetFeeder.d \
./src/StepMotor.d \
./src/TIMHandler.d \
./src/Utils.d \
./src/WaterSensor.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d 

OBJS += \
./src/ADCHandler.o \
./src/Bluetooth.o \
./src/DMAHandler.o \
./src/Keyboard.o \
./src/LoadCell.o \
./src/PetFeeder.o \
./src/StepMotor.o \
./src/TIMHandler.o \
./src/Utils.o \
./src/WaterSensor.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__LPC17XX__ -D__REDLIB__ -I"/home/franco/Documentos/Trabajo Final/CMSISv2p00_LPC17xx/inc" -I"/home/franco/Documentos/Trabajo Final/CMSISv2p00_LPC17xx/inc" -I"/home/franco/Documentos/Trabajo Final/CMSISv2p00_LPC17xx/Drivers/inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/ADCHandler.d ./src/ADCHandler.o ./src/Bluetooth.d ./src/Bluetooth.o ./src/DMAHandler.d ./src/DMAHandler.o ./src/Keyboard.d ./src/Keyboard.o ./src/LoadCell.d ./src/LoadCell.o ./src/PetFeeder.d ./src/PetFeeder.o ./src/StepMotor.d ./src/StepMotor.o ./src/TIMHandler.d ./src/TIMHandler.o ./src/Utils.d ./src/Utils.o ./src/WaterSensor.d ./src/WaterSensor.o ./src/cr_startup_lpc175x_6x.d ./src/cr_startup_lpc175x_6x.o ./src/crp.d ./src/crp.o

.PHONY: clean-src

