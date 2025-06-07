import SbW_Streamer
import numpy as np 
import struct
import matplotlib.pyplot as plt

Id_list:list[float] = [0.0] * 10
Iq_list:list[float] = [0.0] * 10
Vd_list:list[float] = [0.0] * 10
Vq_list:list[float] = [0.0] * 10
T_list:list[float] = [0.0] * 10
K_list:list[np.uint8] = [0] * 10
M_list:list[np.uint16] = [0] * 10
G_list:list[np.uint8] = [[] for _ in range(10)]

iterations = list(range(1, 11))

def main():
    #Import the COM PORT
    streamer=SbW_Streamer.SbW_Streamer('COM13',115200,1)
    #Open the COM PORT
    Status = streamer.Open()
    print (Status)


    ############### Get_Set_SamplingFreq TESTING ################

    # Read one frame
    R_S = streamer.Get_Set_SamplingFreq(1)
    print(R_S)

    # Write one frame
    """
    streamer.SamplingFreq = np.uint16(65535)
    W_S = streamer.Get_Set_SamplingFreq(0)
    print(W_S)
    """

    # Read & Write multiple frames
    """
    for y in range(0,65535):
        
        for j in range(0,2):
            if j == 0 :
                streamer.SamplingFreq = y
                W_S = streamer.Get_Set_SamplingFreq(j)
                print('Sampling Frequency Register write:'+ str(W_S))
            else:
                R_S = streamer.Get_Set_SamplingFreq(j)
                print('Sampling Frequency Register read:',R_S)
    """


    ############### Get_Set_FrameLength TESTING ################

    # Read one frame
    R_F= streamer.Get_Set_FrameLength(True)
    print(R_F)

    # Write one frame
    """
    streamer.FrameLength = np.uint8(255)
    W_F = streamer.Get_Set_FrameLength(0)
    print(W_F)
    """

    # Read & Write multiple frames
    """
    for x in range(0,256):
        for i in range(0,2):
            if i == 0 :
                streamer.FrameLength = x
                W_F = streamer.Get_Set_FrameLength(i)
                print('Frame Length Register write:'+ str(W_F))
            else:
                R_F = streamer.Get_Set_FrameLength(i)
                print('Frame Length Register read:',R_F)
    """
    """
    for x in range(0,256):
        R_F = streamer.Get_Set_FrameLength(1)
        print('Frame Length Register read:',R_F)
    """


    ############### GetFrameBufferDepth TESTING ################
    R_F = streamer.Get_FrameBufferDepth(True)
    print('Frame Buffer Depth Register read:',R_F)


    ############### GetFrames TESTING ################

    R_F,Dataframe = streamer.GetFrames(1,20)
    Decode_Frames(Dataframe,streamer.FrameLength)
    print('Frame',R_F)
    
pass

def Decode_Frames(dat:bytes,framelen:int):

    for i in range (10):
        Id:float=struct.unpack('<f',dat[0:4])[0]
        Id_list [i] = Id 
        #Id_list.append(Id)
        #Id_list=[Id_list,Id]
        Iq:float=struct.unpack('<f',dat[4:8])[0]
        Iq_list [i] = Iq 
        Vd:float=struct.unpack('<f',dat[8:12])[0]
        Vd_list [i]= Vd
        Vq:float=struct.unpack('<f',dat[12:16])[0]
        Vq_list [i]= Vq
        T:float=struct.unpack('<f',dat[16:20])[0]
        T_list [i]=T
        K:np.uint8=struct.unpack('<B',dat[20:21])[0]
        K_list [i]=K
        M:np.uint16=struct.unpack('>H',dat[21:23])[0]
        M_list [i]=M
        G: list[np.uint8] = list(struct.unpack('<8B', dat[24:32]))
        G_list = G
        print(Id, Iq, Vd, Vq, T, K, M, G)

    fig, axes = plt.subplots(1, 2, figsize=(10, 5))
    axes[0].plot( list(range(1, 11)), Id_list, linestyle='--', color='b', label="Line Chart")
    axes[0].set_title("Id List")
    axes[0].set_xlabel("Iteration Number")
    axes[0].set_ylabel("Id Values")
    axes[0].grid(True)

    axes[1].plot(list(range(1, 11)), Iq_list, linestyle='-', color='r')
    axes[1].set_title("Iq List")
    axes[1].set_xlabel("Iteration Number")
    axes[1].set_ylabel("Iq Values")
    axes[1].grid(True)

    plt.legend()
    plt.tight_layout()
    plt.show()



    pass


if __name__ == '__main__':
    main()