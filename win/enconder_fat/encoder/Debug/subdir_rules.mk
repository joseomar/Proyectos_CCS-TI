################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
diskio.obj: ../diskio.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/StellarisWare/driverlib" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/driverlib/fatFs_3" --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StellarisWare/boards/Mi_carpetita/enconder_fat/encoder/celt-0.7.1" --include_path="C:/StellarisWare/boards/Mi_carpetita/enconder_fat/encoder/celt-0.7.1/libcelt" --gcc --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="diskio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ff.obj: ../ff.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/StellarisWare/driverlib" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/driverlib/fatFs_3" --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StellarisWare/boards/Mi_carpetita/enconder_fat/encoder/celt-0.7.1" --include_path="C:/StellarisWare/boards/Mi_carpetita/enconder_fat/encoder/celt-0.7.1/libcelt" --gcc --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="ff.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/StellarisWare/driverlib" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/driverlib/fatFs_3" --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StellarisWare/boards/Mi_carpetita/enconder_fat/encoder/celt-0.7.1" --include_path="C:/StellarisWare/boards/Mi_carpetita/enconder_fat/encoder/celt-0.7.1/libcelt" --gcc --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


