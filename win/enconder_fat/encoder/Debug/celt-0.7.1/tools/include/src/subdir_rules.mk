################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
celt-0.7.1/tools/include/src/bitwise.obj: ../celt-0.7.1/tools/include/src/bitwise.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="/opt/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="/home/carrasin/workspace/encoder/celt-0.7.1" --include_path="/home/carrasin/workspace/encoder/celt-0.7.1/libcelt" --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="celt-0.7.1/tools/include/src/bitwise.pp" --obj_directory="celt-0.7.1/tools/include/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

celt-0.7.1/tools/include/src/framing.obj: ../celt-0.7.1/tools/include/src/framing.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="/opt/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="/home/carrasin/workspace/encoder/celt-0.7.1" --include_path="/home/carrasin/workspace/encoder/celt-0.7.1/libcelt" --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="celt-0.7.1/tools/include/src/framing.pp" --obj_directory="celt-0.7.1/tools/include/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


