################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Bootloader.c \
../Core/Src/Crc.c \
../Core/Src/FLASH_MANAGER.c \
../Core/Src/FLASH_module.c \
../Core/Src/Rollback.c \
../Core/Src/UDS.c \
../Core/Src/can.c \
../Core/Src/cantp.c \
../Core/Src/clock.c \
../Core/Src/gpio.c \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/Bootloader.o \
./Core/Src/Crc.o \
./Core/Src/FLASH_MANAGER.o \
./Core/Src/FLASH_module.o \
./Core/Src/Rollback.o \
./Core/Src/UDS.o \
./Core/Src/can.o \
./Core/Src/cantp.o \
./Core/Src/clock.o \
./Core/Src/gpio.o \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/Bootloader.d \
./Core/Src/Crc.d \
./Core/Src/FLASH_MANAGER.d \
./Core/Src/FLASH_module.d \
./Core/Src/Rollback.d \
./Core/Src/UDS.d \
./Core/Src/can.d \
./Core/Src/cantp.d \
./Core/Src/clock.d \
./Core/Src/gpio.d \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Bootloader.cyclo ./Core/Src/Bootloader.d ./Core/Src/Bootloader.o ./Core/Src/Bootloader.su ./Core/Src/Crc.cyclo ./Core/Src/Crc.d ./Core/Src/Crc.o ./Core/Src/Crc.su ./Core/Src/FLASH_MANAGER.cyclo ./Core/Src/FLASH_MANAGER.d ./Core/Src/FLASH_MANAGER.o ./Core/Src/FLASH_MANAGER.su ./Core/Src/FLASH_module.cyclo ./Core/Src/FLASH_module.d ./Core/Src/FLASH_module.o ./Core/Src/FLASH_module.su ./Core/Src/Rollback.cyclo ./Core/Src/Rollback.d ./Core/Src/Rollback.o ./Core/Src/Rollback.su ./Core/Src/UDS.cyclo ./Core/Src/UDS.d ./Core/Src/UDS.o ./Core/Src/UDS.su ./Core/Src/can.cyclo ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/can.su ./Core/Src/cantp.cyclo ./Core/Src/cantp.d ./Core/Src/cantp.o ./Core/Src/cantp.su ./Core/Src/clock.cyclo ./Core/Src/clock.d ./Core/Src/clock.o ./Core/Src/clock.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

