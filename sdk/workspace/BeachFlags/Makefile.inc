USE_RASPIKE_ART=1

mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_CXXOBJS += act.o run.o stay.o trace.o trace_blue.o turn.o loopFrontAndBack.o pipod.o readfile.o pipodforpic.o turnByLocalizer.o\
	hsvMeasure.o localizer.o calculateAngle.o \
	detect.o detectBrightness.o  detectDistance.o detectStart.o detectAngle.o detectAngleForCurrentTargetValue.o detectCount.o  detectHsv.o detectMotorAngle.o detectSonar.o detectSonarOrDistance.o  detectgCheck.o detectAngleforpic.o detectTime.o\
	scenario.o robot.o module_common.o

SRCLANG := c++
APPL_CFLAGS += -std=c++11

# EV3RT C++ API
#ifdef CONFIG_EV3RT_APPLICATION
#	# Include libraries
#	include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile
#endif

# Application directories
APPL_DIRS += \
	$(mkfile_path)action \
	$(mkfile_path)analysis \
	$(mkfile_path)detection \
	$(mkfile_path)scenario

# Include directories
INCLUDES += \
	-I$(mkfile_path)action \
	-I$(mkfile_path)analysis \
	-I$(mkfile_path)detection \
	-I$(mkfile_path)scenario
