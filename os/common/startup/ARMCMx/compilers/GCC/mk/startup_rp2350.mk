# List of the ChibiOS generic RP2350 startup and CMSIS files.
STARTUPSRC = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/crt1.c

STARTUPASM = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/crt0_v8m-ml.S \
             $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/vectors.S \
             $(CHIBIOS)/os/common/startup/ARMCMx/devices/RP2350/rp2350_imagedef.S

STARTUPINC = $(CHIBIOS)/os/common/portability/GCC \
             $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC \
             $(CHIBIOS)/os/common/startup/ARMCMx/devices/RP2350 \
             $(CHIBIOS)/os/common/ext/ARM/CMSIS/Core/Include \
             $(CHIBIOS)/os/common/ext/RP/RP2350

STARTUPLD  = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/ld

# RP2350-specific defines required for generic ARMv8-M-ML port compatibility.
# - CORTEX_SIMPLIFIED_PRIORITY: Use PendSV instead of SVC for context switch
#   (RP2350 bootrom doesn't implement an SVC handler chain).
# - CH_DBG_ENABLE_STACK_CHECK: Always save/restore PSPLIM during context switch
#   (RP2350 startup code sets PSPLIM which must be preserved across switches).
# - PICO_RP2350/PICO_RP2040: Chip identification for Pico SDK headers.
DDEFS  += -DCORTEX_SIMPLIFIED_PRIORITY=TRUE -DCH_DBG_ENABLE_STACK_CHECK=TRUE \
          -DPICO_RISCV=0 -DPICO_RP2350=1 -DPICO_RP2040=0
DADEFS += -DCORTEX_SIMPLIFIED_PRIORITY=TRUE -DCH_DBG_ENABLE_STACK_CHECK=TRUE

# Shared variables
ALLXASMSRC += $(STARTUPASM)
ALLCSRC    += $(STARTUPSRC)
ALLINC     += $(STARTUPINC)
