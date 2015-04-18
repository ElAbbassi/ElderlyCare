import serial
ser = serial.Serial('/dev/ttyUSB0', 9600)
DATA = []
def parsePacket():
	pass

while True :
    try:
        s=ser.read()
        intValue = ord(s)
        RxByte = hex(intValue)
        #state = hex(ord(ser.read()))
        #print(state,'=',s)       
        if RxByte == '0x7e':
        	print('Fin de trama')
        elif (RxByte == '0x0d'):
        	#print(ser.read(40))
        	afx = ord(ser.read(4))
        	afy = ord(ser.read(4))
        	afz = ord(ser.read(4))
        	mfx = ord(ser.read(4))
        	mfy = ord(ser.read(4))
        	mfz = ord(ser.read(4))
        	gfx = ord(ser.read(4))
        	gfy = ord(ser.read(4))
        	gfz = ord(ser.read(4))
			listt = [afx,afy,afz,mfx,mfy,mfz,gfx,gfy,gfz]
			print(listt)
        elif (RxByte == '0xad'):
        	print(ser.read(16))
        elif (RxByte == '0x0f'):
        	pass
        elif (RxByte == '0x0a'):
        	pass
        else:
        	pass
        	             	
    except:
    	print("Error leyendo")
