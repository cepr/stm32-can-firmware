import can
import time

# os.system('sudo ifconfig can0 down')
# os.system('sudo ip link set can0 type can bitrate 1000000')
# os.system("sudo ifconfig can0 txqueuelen 100000")
# os.system('sudo ifconfig can0 up')

CAN_ID_BRAKE_LIGHTS 		=	0x10
CAN_ID_BLINKER 		=			0x11
CAN_ID_BACK_UP_LIGHTS	=		0x80
CAN_ID_RIGHT_TURN_SIGNALS =		0x81
CAN_ID_LEFT_TURN_SIGNALS =		0x82
CAN_ID_TAIL_LIGHTS 	=			0x83
CAN_ID_OIL_PRESSURE 	=		0x84
CAN_ID_HEAD_TEMP 		=		0x85
CAN_ID_REAR_LEFT_LIGHTS_STATUS= 	0x86
CAN_ID_REAR_RIGHT_LIGHTS_STATUS= 0x87
CAN_ID_REAR_LIGHTS_BRIGHTNESS =	0x88
CAN_ID_LAMBDA 			=		0x89
CAN_ID_VBAT 			=		0x8a
CAN_ID_STARTER 			=		0x8b
CAN_ID_REVERSE_SWITCH        =   0x8c
CAN_ID_EMERGENCY_SWITCH      =   0x8d

can0 = can.interface.Bus(channel = 'can0', bustype = 'socketcan')

can0.send(can.Message(arbitration_id=CAN_ID_BLINKER, is_extended_id=False, data=[1]))
time.sleep(0.1)
can0.send(can.Message(arbitration_id=CAN_ID_BRAKE_LIGHTS, is_extended_id=False, data=[0]))
time.sleep(0.1)
can0.send(can.Message(arbitration_id=CAN_ID_REVERSE_SWITCH, is_extended_id=False, data=[0]))
time.sleep(0.1)
can0.send(can.Message(arbitration_id=CAN_ID_LEFT_TURN_SIGNALS, is_extended_id=False, data=[0]))
time.sleep(0.1)
can0.send(can.Message(arbitration_id=CAN_ID_TAIL_LIGHTS, is_extended_id=False, data=[1]))
time.sleep(0.1)
