#!/usr/bin/python

import serial, time
import argparse


parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("cmd", help="command to send")
parser.add_argument("arguments", help="arguments to the command", nargs='+')
parser.add_argument("-d", "--device", help="tty to use",
                    default="/dev/ttyUSB0")
parser.add_argument("-r", "--repeat", help="nb of send repeats", type=int,
                    default=1)
args = parser.parse_args()

print args

ser = serial.Serial(args.device, 9600,  bytesize=8, parity='N', stopbits=1)
print(ser)
for i in range(args.repeat):
	str=args.cmd+" "+' '.join(args.arguments)+"\n"
	print str
	ser.write(str)     # write a string
	time.sleep(2)
	print ser.readline()
ser.close()
