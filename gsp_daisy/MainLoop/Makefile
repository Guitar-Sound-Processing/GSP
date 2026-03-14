# Project Name
TARGET = gsp.bin

# DaisyBootloader - BOOT_SRAM, BOOT_QSPI, BOOT_NONE
APP_TYPE = BOOT_SRAM

# Optimisations
OPT = -O3

# Sources
CPP_SOURCES = gsp.cpp
CPP_SOURCES += \
chorus.cpp \
compressor.cpp \
delay_fb.cpp \
delay_ff.cpp \
detune.cpp \
equalizer_3b.cpp \
gsp_chain.cpp \
level_detector.cpp \
lfo.cpp \
limiter.cpp \
noise_gate.cpp \
octave.cpp \
overdrive.cpp \
phaser.cpp \
pitch_shifter.cpp \
pots.cpp \
reverber.cpp \
tone_lphp.cpp \
tremolo.cpp \
wahwah.cpp \

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../.../DaisySP

# Float support on printf
LDFLAGS ?=
LDFLAGS += -u _printf_float

# Upload app in QSPI, execute on SRAM

#LDSCRIPT = $(SYSTEM_FILES_DIR)/STM32H750IB_sram.lds
#LDSCRIPT = $(SYSTEM_FILES_DIR)/STM32H750IB_qspi.lds
#C_DEFS += -DBOOT_APP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
DAISYSP_DIR = ../../DaisySP

include $(SYSTEM_FILES_DIR)/Makefile
