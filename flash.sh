#!/bin/sh

openocd -f stm32f1x_stlinkv2_dongle.cfg -c 'program build/stm32-usb-password-manager.elf verify reset exit'
