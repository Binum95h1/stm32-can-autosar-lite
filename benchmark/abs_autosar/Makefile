PROJECT := abs_autosar_reg_f103
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

CPU := -mcpu=cortex-m3 -mthumb
CFLAGS := $(CPU) -O0 -g3 -ffreestanding -fdata-sections -ffunction-sections -Wall -Wextra
INCLUDES := \
-I. \
-IApp \
-IBase \
-IBsw \
-IMcal \
-IEcuAbstraction \
-IRte \
-IServices \
-ISwc \
-IConfig

LDFLAGS := $(CPU) -T linker.ld -Wl,--gc-sections -Wl,-Map=$(PROJECT).map -nostartfiles -specs=nosys.specs -specs=nano.specs -lm

SRCS := \
$(wildcard App/*.c) \
$(wildcard Swc/*.c) \
$(wildcard Rte/*.c) \
$(wildcard Mcal/*.c) \
$(wildcard EcuAbstraction/*.c) \
$(wildcard Services/*.c) \
$(wildcard Bsw/*.c)

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

clean:
	rm -f $(OBJS) $(PROJECT).elf $(PROJECT).bin $(PROJECT).map

flash: $(PROJECT).elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(PROJECT).elf verify reset exit"
