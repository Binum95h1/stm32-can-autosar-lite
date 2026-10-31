PROJECT := main_autosar_reg_f103c8
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
CPU := -mcpu=cortex-m3 -mthumb
CFLAGS := $(CPU) -O0 -g3 -ffreestanding -fdata-sections -ffunction-sections -Wall -Wextra
INCLUDES := -IBase -IConfig -IServices -IBsw -IMcal -IEcuAbstraction -IRte -ISwc -IApp
LDFLAGS := $(CPU) -T linker.ld -Wl,--gc-sections -Wl,-Map=$(PROJECT).map -nostartfiles -specs=nosys.specs -specs=nano.specs -lm
SRCS := App/main.c \
        Services/OsIf.c Services/Det.c Services/SchM.c \
        Mcal/Adc.c Mcal/I2c.c Mcal/Mcu.c Mcal/Uart.c Mcal/Dio.c \
        Mcal/Can.c Mcal/Can_PBcfg.c \
        EcuAbstraction/LcdIf.c Swc/MainNodeS.c Swc/MainNodeS_Data.c \
        Bsw/EcuM.c Bsw/BswM.c Bsw/CanIf.c Bsw/CanIf_PBcfg.c \
        Bsw/PduR.c Bsw/PduR_PBcfg.c Bsw/Can_Upper_Autosar.c \
        EcuAbstraction/ComIf.c Rte/Rte_Main.c
OBJS := $(SRCS:.c=.o) startup_stm32f103c8tx.o
all: $(PROJECT).elf $(PROJECT).bin
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
startup_stm32f103c8tx.o: startup_stm32f103c8tx.s
	$(CC) $(CPU) -c $< -o $@
$(PROJECT).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@
$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
