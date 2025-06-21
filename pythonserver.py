import socket
import struct
import av
DEBUG = False
import psutil, os
p = psutil.Process(os.getpid())
p.nice(psutil.HIGH_PRIORITY_CLASS)

   
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



# import cupy as cp
import numpy as np
import PyNvVideoCodec as nvc
import mss
import time
import cv2
import dxcam
width = 1920
height = 1080
fps = 60
# Create encoder
encoder = nvc.CreateEncoder(
    width=width,
    height=height,
    fmt = "ABGR",
    format = "NV12",
    usecpuinputbuffer=True,
    
    **{"codec":"h264","fps" : fps,"colorspace": "bt709", "tuning_info" : "ultra_low_latency","lookahead" : 0, "bitrate" : 30_000_000,"preset" : "P1","gop" : 10
    })
camera = dxcam.create(output_color="RGBA",device_idx=0, output_idx=0)
sock = socket.socket()

sock.bind(("0.0.0.0", 8999))
sock.listen(5)
print("listening")
client, addr = sock.accept()
print("connected")

# Process input frames
camera.start(target_fps=fps,video_mode=True)
with mss.mss(with_cursor=True) as ms:
    print("loop")
    while True:
        start = time.perf_counter()
        # Read raw frame data
        # start = time.perf_counter()
        # sc = ms.grab(ms.monitors[0])
        frame = camera.get_latest_frame()
        # grab_time = time.perf_counter()

        # frame = np.array(sc)
          #BGRA TO ABGR
        # abgr_frame = frame[:, :, [3, 0, 1, 2]]
        
        #Thats because the decoder thinks we are using ABGR so here we do the inverse of ABGR to RGBA
        # abgr_frame = frame[:, :, [2,1,0,3]]
        # convert_time = time.perf_counter()
        
        bitstream = encoder.Encode(frame)
        # encode_time = time.perf_counter()
        # print("send")
        send(client, bytes(bitstream))
        # send_time = time.perf_counter()

        # print(f"grab: {grab_time - start:.4f}s, convert: {convert_time - grab_time:.4f}s, encode: {encode_time - convert_time:.4f}s, send: {send_time - encode_time:.4f}s")

        end = time.perf_counter()
        # print(f"fps should be: {1/ (end-start) }")
        time.sleep(max(0, (1 / fps) - (end - start)))
        
    # Flush encoder
    bitstream = encoder.EndEncode()
    send(client, bytes(bitstream))