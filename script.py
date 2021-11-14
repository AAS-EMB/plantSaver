#!/usr/bin/python
import serial
import struct
ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, parity=serial.PARITY_NONE,\
                    stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=0)
print("connected to: " + ser.portstr)
while True:
    line = ser.readline()
    if line:
        tmp = struct.unpack('f', line)
        print("Temperature board =", tmp[0])