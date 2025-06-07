import serial
import time
import numpy as np # as means that instead of numpy , we can type np

Streamer_CMD_Set = {
#   'Name of CMD': (CMD_ID,Dlen)
    'Get_Set_Sampling_Freq':(0x1,2),
    'Get_Set_Frame_Length':(0x2,1),
    'Get_Set_Stream_ON':(0x3,1),
    'Get_Frames':(0x4,1),
   # 'Data_Frames':(0x5,1+ SbW_Streamer.FrameLength),
    'Get_FrameBufferDepth':(0x6,2),
}

class SbW_Streamer:
    #communication params (specific to the com port)
    COM_Name:str
    BaudRate:int
    Timeout:float
    port:serial.Serial

    #class params (specific to the implementation)
    SamplingFreq=np.uint16(0)
    FrameLength=np.uint8(0)
    FrameBufferDepth=np.uint16(0)
    StreamON=np.uint8(0)

    def __init__(self,COM_port:str,baudRate:int,Timeout:float):
        self.COM_Name = COM_port
        self.BaudRate = baudRate
        self.Timeout =Timeout
    
    def Open(self)->bool:
        try:
            #initialize and open port
            self.port = serial.Serial(self.COM_Name,  baudrate=self.BaudRate,timeout=self.Timeout)
            return True

        except Exception as e:
            return False
    
    #deals with data of frame only without CRC Registers
    def CRC (self,data:list[np.uint8], len:np.uint16)->np.uint16: 
        CRC_Result:int = 0
        for i in range(0,len) :
            CRC_Result += int(data[i])
        return np.uint16(CRC_Result)
    
#user functions
    def Get_Set_SamplingFreq(self, R_W:bool):
        CMD = Streamer_CMD_Set['Get_Set_Sampling_Freq'][0]
        Data_Length = Streamer_CMD_Set['Get_Set_Sampling_Freq'][1]

        Request_Len = 5 if R_W == True else 5+Data_Length
        Reply_Len = 5 + Data_Length if R_W == True else 5

        Frame = np.zeros(Request_Len , dtype=np.uint8)
        Frame[0] = 0x55 #SOF
        Frame[1] = CMD | (0b10000000 if R_W == True else 0)
        Frame[2] = 0 if R_W == True else Data_Length #data length
        # in case of write mode, the data field needs to be filled
        if R_W == False:
            Frame[3] = np.uint8(self.SamplingFreq & 0xff)
            Frame[4] = np.uint8(self.SamplingFreq >> 8)
        Checksum = self.CRC(Frame,Request_Len-2)
        Frame[Request_Len-2] = np.uint8(Checksum & 0xff)
        Frame[Request_Len-1] = np.uint8(Checksum>>8)

        try:
            self.port.write(Frame)
            Data_in = list(self.port.read(Reply_Len)) 

            #check if the requested num of bytes was recieved
            if(len(Data_in)==Reply_Len):
                #compute the frame checksum
                Checksum_Calc = self.CRC(Data_in,len(Data_in)-2)
                #construct the frame internal checksum
                Checksum = Data_in[Reply_Len-1] | (Data_in[Reply_Len-2]<<8)
                if Checksum == Checksum_Calc:
                    if R_W == False:

                        self.SamplingFreq = np.uint16(Data_in[4] | (Data_in[3]<<8))
                        ##########          Least significant bit,  Most significant bit           ########
                        return Frame
                    else:
                        return Data_in

                else: return False
            else :return False
        except Exception as e:
            print(e)
            return False
    
    def Get_Set_FrameLength(self, R_W:bool):
        R_W = True #read only command
        CMD = Streamer_CMD_Set['Get_Set_Frame_Length'][0]
        Data_Length = Streamer_CMD_Set['Get_Set_Frame_Length'][1]

        Request_Len = 5 if R_W == True else 5+Data_Length
        Reply_Len = 5 + Data_Length if R_W == True else 5

        Frame = np.zeros(Request_Len , dtype=np.uint8)
        Frame[0] = 0x55 #SOF
        Frame[1] = CMD | (0b10000000 if R_W == True else 0)
        Frame[2] = 0 if R_W == True else Data_Length #data length

        Checksum = self.CRC(Frame,Request_Len-2)
        Frame[Request_Len-2] = np.uint8(Checksum&0xff) #0xff= 0b1111 1111
        Frame[Request_Len-1] = np.uint8(Checksum>>8) 

        try:
            self.port.write(Frame)
            Data_Frame = list(self.port.read(Reply_Len)) 
            #check if the requested num of bytes was recieved
            if(len(Data_Frame)==Reply_Len):
                #compute the frame checksum
                Checksum_Calc = self.CRC(Data_Frame,len(Data_Frame)-2)
                #construct the frame internal checksum
                Checksum = Data_Frame[Reply_Len-1] | (Data_Frame[Reply_Len-2]<<8)
                if Checksum == Checksum_Calc:
                    self.FrameLength = np.uint8(Data_Frame[3])
                    return Data_Frame         
                else: return False
            else :return False
        except Exception as e:
            print(e)
            return False
        
    def Get_FrameBufferDepth(self, R_W:bool):
            R_W = True #read only command
            CMD = Streamer_CMD_Set['Get_FrameBufferDepth'][0]
            Data_Length = Streamer_CMD_Set['Get_FrameBufferDepth'][1]

            Request_Len = 5 if R_W == True else 5+Data_Length
            Reply_Len = 5 + Data_Length if R_W == True else 5

            Frame = np.zeros(Request_Len , dtype=np.uint8)
            Frame[0] = 0x55 #SOF
            Frame[1] = CMD | (0b10000000 if R_W == True else 0)
            Frame[2] = 0 if R_W == True else Data_Length #data length
            # in case of write mode, the data field needs to be filled
            if R_W == True:
                Frame[3] = self.FrameLength
            Checksum = self.CRC(Frame,Request_Len-2)
            Frame[Request_Len-2] = np.uint8(Checksum&0xff) #0xff= 0b1111 1111
            Frame[Request_Len-1] = np.uint8(Checksum>>8) 

            try:
                self.port.write(Frame)
                Data_Frame = list(self.port.read(Reply_Len)) 
                #check if the requested num of bytes was recieved
                if(len(Data_Frame)==Reply_Len):
                    #compute the frame checksum
                    Checksum_Calc = self.CRC(Data_Frame,len(Data_Frame)-2)
                    #construct the frame internal checksum
                    Checksum = Data_Frame[Reply_Len-1] | (Data_Frame[Reply_Len-2]<<8)
                    if Checksum == Checksum_Calc:
                        self.Get_FrameBufferDepth = np.uint16(Data_Frame[3]<<8 | Data_Frame[4])
                        return Data_Frame         
                    else: return False
                else :return False
            except Exception as e:
                print(e)
                return False
            
    def GetFrames(self,R_W:bool,NumOfFrames):
        R_W = False #write only command
        CMD = Streamer_CMD_Set['Get_Frames'][0]
        Data_Length = Streamer_CMD_Set['Get_Frames'][1]

        Request_Len = 5 if R_W == True else 5+Data_Length
        Reply_Len = 5 + Data_Length if R_W == True else 5

        Frame = np.zeros(Request_Len , dtype=np.uint8)
        Frame[0] = 0x55 #SOF
        Frame[1] = CMD | (0b10000000 if R_W == True else 0)
        Frame[2] = 0 if R_W == True else Data_Length #data length
        Frame[3] = np.uint8(NumOfFrames)
        Checksum = self.CRC(Frame,Request_Len-2)
        Frame[Request_Len-2] = np.uint8(Checksum&0xff) #0xff= 0b1111 1111
        Frame[Request_Len-1] = np.uint8(Checksum>>8) 

        try:
            self.port.write(Frame)

            BytesToRead = int(self.FrameLength) * NumOfFrames
            Data_Frame = self.port.read(BytesToRead)

            return (True,Data_Frame)
        
        except Exception as e:
            print(e)
            return (False,[])
