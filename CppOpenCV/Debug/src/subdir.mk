################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/030_WarpPictures.cpp \
../src/AntiShake.cpp \
../src/ColorHistogram.cpp \
../src/Histogram1D.cpp \
../src/Layer.cpp \
../src/RobinsonBorders.cpp 

OBJS += \
./src/030_WarpPictures.o \
./src/AntiShake.o \
./src/ColorHistogram.o \
./src/Histogram1D.o \
./src/Layer.o \
./src/RobinsonBorders.o 

CPP_DEPS += \
./src/030_WarpPictures.d \
./src/AntiShake.d \
./src/ColorHistogram.d \
./src/Histogram1D.d \
./src/Layer.d \
./src/RobinsonBorders.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include/opencv -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


