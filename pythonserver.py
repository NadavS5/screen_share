import socket
import struct
import av
DEBUG = False
def RGB_TO_NV12(sct_img):
    """
    Convert BGRA to NV12 format as a flat uint8 buffer.
    """
    frame = av.VideoFrame.from_ndarray(sct_img, format='rgb24')
    nv12_frame = frame.reformat(format='nv12') # PyAV handles YUV conversion, 4:2:0 subsampling, and NV12 interleaving
    
    nv12_data = b''
    for plane in nv12_frame.planes: # NV12 has 2 planes (Y, UV)
        nv12_data += bytes(plane)
    return nv12_data

   
   
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
height = 1920
fps = 30
frame_size = width * height * 1.5  # Size for NV12 format
# Create encoder
encoder = nvc.CreateEncoder(
    width=width,
    height=height,
    fmt = "NV12",
    format = "ABGR",
    usecpuinputbuffer=True,
    **{"codec":"h264","fps" : fps, "bitrate" : 5_000_000 ,"idrperiod": 10,
        "repeatspspps": 1, "tuning_info" : "low_latency","lookahead": 0, "slice::mode": 0
    })

sock = socket.socket()

sock.bind(("0.0.0.0", 8999))
sock.listen(5)
print("listening")
client, addr = sock.accept()
print("connected")
def describe_nal(packet: bytes):
    if len(packet) < 5: return "Too short"
    if packet.startswith(b'\x00\x00\x00\x01'):
        nal = packet[4]
        types = {
            1: "Non-IDR (P-frame)",
            5: "IDR (I-frame)",
            6: "SEI",
            7: "SPS",
            8: "PPS",
            9: "AUD"
        }
        return f"NAL type {nal & 0x1F}: {types.get(nal & 0x1F, 'Unknown')}"
    return "No Annex B start code"
# Process input frames
with mss.mss(with_cursor=True) as ms:
    print("loop")
    for i in range(1000):
        start_time = time.time()
        
        # Read raw frame data
        bounding_box = {'top': 0, 'left': 0, 'width': width, 'height': height}
        sc = ms.grab(bounding_box)
        frame = np.array(sc)
        # frame = frame[:, :, :3]              # Drop alpha channel, now BGR
        # frame = frame[..., ::-1]             # Convert BGR to RGB
        # # print("1")
        argb_frame = frame[:, :, [3, 2, 1, 0]]
        # nv12_frame = RGB_TO_NV12(frame)
        # nv12_frame = np.ascontiguousarray(nv12_frame)
        bitstream = encoder.Encode(argb_frame)
        packet = bytes(bitstream)
        # print("len:", len(packet))
        
        if len(packet)>0:
            
            print(packet[:5])
            
            # Write encoded data to file
            # print(f"sending frame {i}")
            send(client, bytes(bitstream))
            # print(describe_nal(packet))
            # print("send")
            elapsed = time.time() - start_time
            sleep_time = max(0, (1.0 / fps) - elapsed)
            time.sleep(sleep_time)

    # Flush encoder
    bitstream = encoder.EndEncode()
    # send(client, DeprecationWarning)