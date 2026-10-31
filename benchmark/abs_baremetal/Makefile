PROJECT := abs_baremetal_reg_f103
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

CPU := -mcpu=cortex-m3 -mthumb
CFLAGS := $(CPU) -O0 -g3 -ffreestanding -fdata-sections -ffunction-sections -Wall -Wextra

INCLUDES := -I. -IApp -IBase -IMcal -IEcuAbstraction -IServices -ISwc -IConfig -IBare

LDFLAGS := $(CPU) -T linker.ld -Wl,--gc-sections -Wl,-Map=$(PROJECT).map -nostartfiles -specs=nosys.specs -specs=nano.specs -lm

SRCS := $(wildcard App/*.c) $(wildcard Swc/*.c) $(wildcard Mcal/*.c) EcuAbstraction/MotorIf.c $(wildcard Services/*.c) $(wildcard Bare/*.c)

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
