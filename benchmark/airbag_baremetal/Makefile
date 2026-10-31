PROJECT := airbag_baremetal_reg_f103
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
CPU := -mcpu=cortex-m3 -mthumb
CFLAGS := $(CPU) -O0 -g3 -ffreestanding -fdata-sections -ffunction-sections -Wall -Wextra
INCLUDES := -I. -IApp -ISwc -IMcal -IEcuAbstraction -IServices -IConfig -IBase -IBare
SRCS := $(wildcard App/*.c) $(wildcard Swc/*.c) $(wildcard Mcal/*.c) EcuAbstraction/BuzzerIf.c EcuAbstraction/Mpu6050If.c $(wildcard Services/*.c) $(wildcard Bare/*.c)
OBJS := $(SRCS:.c=.o) startup_stm32f103.o
LDFLAGS := $(CPU) -T linker.ld -Wl,--gc-sections -Wl,-Map=$(PROJECT).map -nostartfiles -specs=nosys.specs -specs=nano.specs -lm
.PHONY: all clean
all: $(PROJECT).elf $(PROJECT).bin
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
%.o: %.s
	$(CC) $(CPU) -c $< -o $@
$(PROJECT).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	$(SIZE) $@
$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
clean:
	rm -f $(OBJS) $(PROJECT).elf $(PROJECT).bin $(PROJECT).map
