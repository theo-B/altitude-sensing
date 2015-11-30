#!/usr/bin/env python

import smbus
import time
import scipy.signal as signal
import numpy as np

# Define addresses
REF_P_XL = 0x08
REF_P_L = 0x09
REF_P_H = 0x0A

WHO_AM_I = 0x0F

RES_CONF = 0x10

CTRL_REG1 = 0x20
CTRL_REG2 = 0x21
CTRL_REG3 = 0x22
INT_CFG_REG = 0x23
INT_SOURCE_REG = 0x24
THS_P_LOW_REG = 0x25
THS_P_HIGH_REG = 0x26
STATUS_REG = 0x27

PRESS_OUT_XL = 0x28
PRESS_OUT_L = 0x29
PRESS_OUT_H = 0x2A

TEMP_OUT_L = 0x2B
TEMP_OUT_H = 0x2C

AMP_CTRL = 0x30

# I2C address of lps331ap
lps331 = 0b1011101
#lps331 = 0b1011100

# Start i2c communication
i2cbus = smbus.SMBus(1); 

# Activate LPS331 @ 12.5Hz
i2cbus.write_byte_data(lps331, CTRL_REG1, 0b11100000)

data = []

for x in xrange(1, 5000):
   pressure_XL = i2cbus.read_byte_data(lps331, PRESS_OUT_XL)
   pressure_L = i2cbus.read_byte_data(lps331, PRESS_OUT_L)
   pressure_H = i2cbus.read_byte_data(lps331, PRESS_OUT_H)
   #print "PH: %i" %pressure_H
   #print "PL: %i" %pressure_L
   #print "PXL: %i" %pressure_XL
   pressure_b = pressure_H << 16 | pressure_L << 8 | pressure_XL
   #print "Pressure is %f" %pressure_b
   pressure = pressure_b/4096.0
   #print "Or %f" %pressure
   altitude = 44308.7 * (1 - ((pressure/1013.25)**0.190284) )
   print "%f" %altitude
   data.append(altitude)
   time.sleep(0.05)
   x+=1
   
data_k = np.convolve(data, np.ones(120)/120) # Box filter - kernel size 120
np.savetxt("data-kd.txt", data_k, delimiter="\t")
np.savetxt("data-raw.txt", data, delimiter="\t")
