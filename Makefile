# Arudino makefile, for use with: https://github.com/sudar/Arduino-Makefile

ARDMK_DIR = $(HOME)/Arduino-Makefile
ARDUINO_DIR = $(HOME)/arduino-1.0.1
BOARD_TAG    = uno
ARDUINO_PORT = /dev/ttyACM*

include $(HOME)/Arduino-Makefile/arduino-mk/Arduino.mk
