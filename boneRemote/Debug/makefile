################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ti/ccsv5/tools/compiler/tms470_4.9.5

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./src/setupWatchdog.obj" \
"./src/setupTimer.obj" \
"./src/setupGpio.obj" \
"./src/reset.obj" \
"./src/main.obj" \
"./src/lwiplib.obj" \
"./src/httpd_io.obj" \
"./src/cgifuncs.obj" \
"./src/Web.obj" \
"./src/Uart_l.obj" \
"./src/Uart.obj" \
-l"libc.a" \
"../uartEcho.cmd" \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: boneRemote.out

# Tool invocations
boneRemote.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"/opt/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7A8 --code_state=32 --abi=eabi -me -g --gcc --define=am335x --define=beaglebone --diag_warning=225 --display_error_number -z --stack_size=0x800 -m"boneRemote.map" --heap_size=0x800 -i"/opt/ti/ccsv5/tools/compiler/tms470_4.9.5/lib" -i"/opt/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --reread_libs --warn_sections --display_error_number --rom_model -o "boneRemote.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(TMS470_EXECUTABLE_OUTPUTS__QUOTED) "boneRemote.out"
	-$(RM) "src/Uart.pp" "src/Uart_l.pp" "src/Web.pp" "src/cgifuncs.pp" "src/httpd_io.pp" "src/lwiplib.pp" "src/main.pp" "src/reset.pp" "src/setupGpio.pp" "src/setupTimer.pp" "src/setupWatchdog.pp" 
	-$(RM) "src/Uart.obj" "src/Uart_l.obj" "src/Web.obj" "src/cgifuncs.obj" "src/httpd_io.obj" "src/lwiplib.obj" "src/main.obj" "src/reset.obj" "src/setupGpio.obj" "src/setupTimer.obj" "src/setupWatchdog.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
