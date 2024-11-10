# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Adrian "asie" Siekierka, 2024

export WONDERFUL_TOOLCHAIN ?= /opt/wonderful
export BLOCKSDS ?= /opt/blocksds/core

DRIVERS := \
	r4tf

# Tools
# -----

CP		:= cp
MAKE		:= make
MKDIR		:= mkdir
RM		:= rm -rf

# Verbose flag
# ------------

ifeq ($(V),1)
_V		:=
else
_V		:= @
endif

.PHONY: all clean $(DRIVERS)

all: $(DRIVERS)

clean:
	@echo "  CLEAN"
	$(_V)$(RM) build dist

$(DRIVERS):
	@echo "  MAKE    $@"
	$(_V)$(MAKE) -f Makefile.driver DRIVER=$@
