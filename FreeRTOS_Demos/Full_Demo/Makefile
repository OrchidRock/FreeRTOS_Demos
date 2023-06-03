#/*
# * FreeRTOS V202112.00
# * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of
# * this software and associated documentation files (the "Software"), to deal in
# * the Software without restriction, including without limitation the rights to
# * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# * the Software, and to permit persons to whom the Software is furnished to do so,
# * subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all
# * copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# *
# * https://www.FreeRTOS.org
# * https://aws.amazon.com/freertos
# *
# * 1 tab == 4 spaces!
# */

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
MCU = TM4C123GH6PM
BIN = FreeRTOS_Full_Demo.bin

BUILD_DIR := build

FREERTOS_DIR_REL := ../
FREERTOS_DIR := $(abspath $(FREERTOS_DIR_REL))
KERNEL_PATH := $(FREERTOS_DIR)/Source
PORT_PATH := $(KERNEL_PATH)/portable/GCC/ARM_CM4F
#DEMO_PATH = ../Common/Minimal
#FULL_DEMO = ./Full_Demo

DRIVERLIB_PARENT_PATH := ../../
DRIVERLIB_PATH := $(DRIVERLIB_PARENT_PATH)/driverlib

UTILS_PATH := ../../utils

DEBUG = #-DDEBUG #-g 
OPT = -Os
WARNINGS = -Wall -Wunused -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -Wbad-function-cast -Wcast-align -Wsign-compare -Wshadow -Wpointer-arith
INCLUDE = -I. -I./Full_Template -I./Minimal -I$(DRIVERLIB_PARENT_PATH) -I$(DRIVERLIB_PATH) -I$(KERNEL_PATH)/include -I$(PORT_PATH) -I$(UTILS_PATH)
CFLAGS = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp $(OPT) $(DEBUG) -ffunction-sections -fdata-sections -MD -std=c99 $(WARNINGS) -pedantic $(INCLUDE)
CFLAGS += -DPART_$(MCU) -Dgcc -DTARGET_IS_BLIZZARD_RA1

LDFLAGS = -T $(MCU).ld --entry ResetISR --gc-sections 
LDFLAGS += -Map $(BUILD_DIR)/$(BIN)_gcc.Map 

LIBC = ${shell ${CC} -mthumb -march=armv6t2 -print-file-name=libc.a}
LIBGCC = ${shell ${CC} -mthumb -march=armv6t2 -print-file-name=libgcc.a}

# driverlib source
DRIVERLIB_FILES :=  \
$(DRIVERLIB_PATH)/sysctl.c \
$(DRIVERLIB_PATH)/gpio.c \
$(DRIVERLIB_PATH)/uart.c \
$(DRIVERLIB_PATH)/timer.c \
$(DRIVERLIB_PATH)/interrupt.c \
$(DRIVERLIB_PATH)/cpu.c \

# FreeRTOS source
FREERTOS_FILES := \
$(KERNEL_PATH)/tasks.c \
$(KERNEL_PATH)/queue.c \
$(KERNEL_PATH)/list.c \
$(KERNEL_PATH)/timers.c \
$(KERNEL_PATH)/portable/MemMang/heap_4.c \
$(PORT_PATH)/port.c


DEMO_SOURCE_FILES := \
printf-stdarg.c \
uartstdio.c \
main.c \
startup_gcc.c \
Full_Template/TestRunner.c \
Full_Template/IntQueueTimer.c \
Full_Template/RegTests.c \
Minimal/semtest.c \
Minimal/GenQTest.c \
Minimal/QueueOverwrite.c \
Minimal/QPeek.c \
Minimal/TaskNotify.c \
Minimal/BlockQ.c \
Minimal/PollQ.c \
Minimal/QueueSet.c \
Minimal/IntQueue.c \
Minimal/TimerDemo.c \
Minimal/integer.c \
Minimal/flop.c \
Minimal/recmutex.c \
Minimal/countsem.c \
Minimal/IntSemTest.c \
Minimal/blocktim.c \
Minimal/EventGroupsDemo.c \
Minimal/TaskNotify.c \
Minimal/TaskNotifyArray.c \

VPATH := $(DRIVERLIB_PATH) $(KERNEL_PATH) $(KERNEL_PATH)/portable/MemMang/ $(PORT_PATH) ./Full_Template/ ./Minimal/ $(UTILS_PATH)

# driverlib objs
DRIVERLIB_OBJS := $(patsubst %.c,%.o,$(notdir $(DRIVERLIB_FILES)))

# freertos objs
FREERTOS_OBJS := $(patsubst %.c,%.o,$(notdir $(FREERTOS_FILES)))

# demo objs
DEMO_OBJS := $(DEMO_SOURCE_FILES:%.c=%.o)

OBJS := \
$(addprefix $(BUILD_DIR)/driverlib/, $(DRIVERLIB_OBJS)) \
$(addprefix $(BUILD_DIR)/FreeRTOS/, $(FREERTOS_OBJS)) \
$(addprefix $(BUILD_DIR)/, $(DEMO_OBJS))

$(info $(OBJS))

.PHONY: clean

#
# Rules for all object files.
#

$(BUILD_DIR)/$(BIN) : $(BUILD_DIR)/a.out
	$(OBJCOPY) -O binary $< $@
	@echo ""
	@echo "-------------------------------------------------"
	$(SIZE) $(BUILD_DIR)/a.out
	@echo "-------------------------------------------------"


$(BUILD_DIR)/a.out : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBC) $(LIBGCC)

${BUILD_DIR}/driverlib/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/FreeRTOS/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	-rm -rf $(BUILD_DIR)
