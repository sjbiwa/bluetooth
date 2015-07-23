#
# Makefile for Bluetooth LE sample program for nRF51822
#

PROJECT_NAME := ble_sample

TARGET := $(PROJECT_NAME)
TARGET_ELF := $(TARGET).elf
TARGET_HEX := $(TARGET).hex
TARGET_BIN := $(TARGET).bin


GNU_INSTALL_ROOT := /usr/local/linaro32n
NRK51_SDK_ROOT   := ../../../../nRF51_SDK_8.0.0_5fc2c3a
#NRK51_SDK_ROOT   := ../../../../nRF51_SDK_9.0.0_2e23562
BSP_ROOT		 := $(NRK51_SDK_ROOT)/examples/bsp

TEMPLATE_PATH	 := $(NRK51_SDK_ROOT)/components/toolchain/gcc

OS_ROOT			 := ../../../../../bwos
OS_KERN_SRC	 	 := $(OS_ROOT)/kernel
OS_KERN_INC	 	 := $(OS_ROOT)/include
OS_BSP_SRC		 := $(OS_ROOT)/arch/arm_cm
OS_BSP_INC		 := $(OS_BSP_SRC)
OS_BOARD_SRC	 := $(OS_ROOT)/arch/arm_cm/board/nrf51822
OS_BOARD_INC	 := $(OS_BOARD_SRC)

export OUTPUT_FILENAME
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 

MK := mkdir
RM := rm -rf

# Toolchain commands
GNU_PREFIX		:= arm-none-eabi
CC       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc"
AS       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as"
AR       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar" -r
LD       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld"
NM       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm"
OBJDUMP  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump"
OBJCOPY  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy"
SIZE    		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size"

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#source common to all targets
C_SOURCE_FILES += \
			$(NRK51_SDK_ROOT)/components/libraries/button/app_button.c \
			$(NRK51_SDK_ROOT)/components/libraries/util/app_error.c \
			$(NRK51_SDK_ROOT)/components/libraries/fifo/app_fifo.c \
			$(NRK51_SDK_ROOT)/components/libraries/gpiote/app_gpiote.c \
			$(NRK51_SDK_ROOT)/components/libraries/timer/app_timer.c \
			$(NRK51_SDK_ROOT)/components/drivers_nrf/hal/nrf_delay.c \
			$(NRK51_SDK_ROOT)/components/ble/common/ble_advdata.c \
			$(NRK51_SDK_ROOT)/components/ble/common/ble_conn_params.c \
			$(NRK51_SDK_ROOT)/components/ble/ble_services/ble_nus/ble_nus.c \
			$(NRK51_SDK_ROOT)/components/ble/common/ble_srv_common.c \
			$(NRK51_SDK_ROOT)/components/toolchain/system_nrf51.c \
			$(NRK51_SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
			$(BSP_ROOT)/bsp.c \
			../bl_main.c \
			../application.c \
			../board_timer.c \
			../idle.c \
			$(OS_KERN_SRC)/fixmb.c \
			$(OS_KERN_SRC)/flag.c \
			$(OS_KERN_SRC)/init_task.c \
			$(OS_KERN_SRC)/malloc.c \
			$(OS_KERN_SRC)/msgq.c \
			$(OS_KERN_SRC)/mutex.c \
			$(OS_KERN_SRC)/sem.c \
			$(OS_KERN_SRC)/smp.c \
			$(OS_KERN_SRC)/st_malloc.c \
			$(OS_KERN_SRC)/startup.c \
			$(OS_KERN_SRC)/task.c \
			$(OS_KERN_SRC)/timer.c \
			$(OS_BSP_SRC)/arch.c \
			$(OS_BSP_SRC)/irq.c \
			$(OS_BOARD_SRC)/boards.c \

#assembly files common to all targets
ASM_SOURCE_FILES  += $(NRK51_SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.s
ASM2_SOURCE_FILES  += $(OS_BSP_SRC)/dispatch.S ../handler.S

#linker script
LINKER_SCRIPT = ble_sample.ld

#includes common to all targets
INC_PATHS  = -I$(NRK51_SDK_ROOT)/components/softdevice/common/softdevice_handler
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/toolchain
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/device
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/drivers_nrf/hal
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/libraries/button
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/libraries/gpiote
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/libraries/util
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/drivers_nrf/uart
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/ble/common
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/libraries/timer
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/ble/ble_services/ble_nus
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/libraries/fifo
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/softdevice/s110/headers
INC_PATHS += -I$(NRK51_SDK_ROOT)/components/toolchain/gcc
INC_PATHS += -I$(BSP_ROOT)
INC_PATHS += -I..
INC_PATHS += -I$(OS_KERN_INC)
INC_PATHS += -I$(OS_BSP_INC)
INC_PATHS += -I$(OS_BOARD_INC)

#flags common to all targets
CFLAGS  = -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF51
CFLAGS += -DS110
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs --std=gnu99 -mno-unaligned-access
CFLAGS += -Wall -Os -g
CFLAGS += -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -flto -fno-builtin

CFLAGS += -DOS_VER="0.01"
CFLAGS += -DTASK_MAX_NUM=4
CFLAGS += -DFLAG_MAX_NUM=2
CFLAGS += -DSEM_MAX_NUM=2
CFLAGS += -DMUTEX_MAX_NUM=2
CFLAGS += -DMSGQ_MAX_NUM=2
CFLAGS += -DFIXMB_MAX_NUM=0
CFLAGS += -DTIMER_MAX_NUM=0
CFLAGS += -DUSE_SMP=0 -DCPU_NUM=1 -DNO_USE_SVC_CALL -DNO_USE_SVC_STACK -DMASTER_CPU_ID=0
CFLAGS += -DCORE_M0
CFLAGS += -DMAX_CLOCK_FREQ=16000000
#CFLAGS += -DSVCALL_PRIORITY=0xff
CFLAGS += -DPENDSV_PRIORITY=0xff
CFLAGS += -DUSE_TICKLESS
CFLAGS += -DMAIN_TASK_USR_STACK_SIZE=256
CFLAGS += -DINITIAL_TASK_USR_STACK_SIZE=256


# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(TARGET).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys -g

# Assembler flags
ASMFLAGS += -x assembler-with-cpp -g
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF51
ASMFLAGS += -DS110
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD

C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS				= $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS			= $(C_SOURCE_FILE_NAMES:.c=.o)

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS			  = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS			  = $(ASM_SOURCE_FILE_NAMES:.s=.o)

ASM2_SOURCE_FILE_NAMES = $(notdir $(ASM2_SOURCE_FILES))
ASM2_PATHS			  = $(call remduplicates, $(dir $(ASM2_SOURCE_FILES) ))
ASM2_OBJECTS			  = $(ASM2_SOURCE_FILE_NAMES:.S=.o)

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)
vpath %.S $(ASM2_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS) $(ASM2_OBJECTS)


#building all targets
all: $(TARGET_BIN) $(TARGET_HEX)

# Create objects from C SRC files
.c.o:
	$(CC) $(CFLAGS) $(INC_PATHS) -c $<

# Assemble files
.s.o:
	$(CC) $(ASMFLAGS) $(INC_PATHS) -c $<

.S.o:
	$(CC) $(CFLAGS) $(INC_PATHS) -c $<

# Link
$(TARGET_ELF): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@


## Create binary .bin file from the .elf file
$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

## Create binary .hex file from the .out file
$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex $< $@

clean:
	$(RM) -f $(OBJECTS)
	$(RM) -f $(TARGET_ELF)
	$(RM) -f $(TARGET_HEX)
	$(RM) -f $(TARGET_BIN)
	$(RM) -f $(TARGET).map

flash: $(MAKECMDGOALS)
	nrfjprog --reset --program $(TARGET_HEX)
