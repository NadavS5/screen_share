import socket
import struct
import av
DEBUG = False

   
   
def send(sock: socket.socket, data: bytes | bytearray):
    size = struct.pack("!I", len(data))
    sock.send(size + data)
    if DEBUG:
         print("SENDING->",(size + data)[:100])

def _recv_by_size(sock: socket.socket, size: int):
        buff = b""
        while(size > 0):
            recv = sock.recv(size)
            if recv == b"":
                return b""
            size-= len(recv)
            buff += recv
        return buff

def recv(sock: socket.socket) -> bytes:
    s = _recv_by_size(sock,4)
    if s == b"":
        return b""
    size = int.from_bytes(struct.unpack("!I",s ))
    data = _recv_by_size(sock, size)
    if DEBUG:
         print("RECIEVED", data)
    return data


# print(recv(client))
# send(client, b"hello world")



import numpy as np
import PyNvVideoCodec as nvc
import mss
import time
import cv2
width = 1920
height = 1080
fps = 100
frame_size = width * height * 1.5  # Size for NV12 format
# Create encoder
encoder = nvc.CreateEncoder(
    width=width,
    height=height,
    fmt = "ABGR",
    format = "NV12",
    usecpuinputbuffer=True,
    
    **{"codec":"h264","fps" : fps,"colorspace": "bt709", "tuning_info" : "ultra_low_latency","lookahead" : 0
    })

sock = socket.socket()

sock.bind(("0.0.0.0", 8999))
sock.listen(5)
print("listening")
client, addr = sock.accept()
print("connected")

# Process input frames
with mss.mss(with_cursor=True) as ms:
    print("loop")
    while True:
        
        # Read raw frame data
        sc = ms.grab(ms.monitors[0])
        frame = np.array(sc)
        
        
        #BGRA TO ABGR
        abgr_frame = frame[:, :, [3, 0, 1, 2]]
        
        #Thats because the decoder thinks we are using ABGR so here we do the inverse of ABGR to RGBA
        abgr_frame = frame[:, :, [2,1,0,3]]
        
        bitstream = encoder.Encode(abgr_frame)
        packet = bytes(bitstream)
        # print("len:", len(packet))
        
        
        # Write encoded data to file
        # print(f"sending frame {i}")
        send(client, bytes(bitstream))
        # print(describe_nal(packet))
        # print("send")
        

    # Flush encoder
    bitstream = encoder.EndEncode()
    send(client, bytes(bitstream))