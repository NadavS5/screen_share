import socket
import struct
DEBUG = False
def send(sock: socket.socket, data: bytes | bytearray):
    size = struct.pack("!I", len(data))
    sock.send(size + data)
    if DEBUG:
         print("SENDING->",(size + data))

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

sock = socket.socket()

sock.bind(("0.0.0.0", 8999))
sock.listen(5)
client, addr = sock.accept()
print("connected")
# print(recv(client))
# send(client, b"hello world")

import av
import mss
import numpy as np
import time

# Screen capture size
width, height = 1920, 1080

# Setup encoder context (no file, just get packets)
codec = av.codec.CodecContext.create('libx264', 'w')
codec.width = width
codec.height = height
# codec.time_base = (1, 30)  # 30 FPS
codec.framerate = 60
codec.pix_fmt = 'yuv420p'
codec.bit_rate = 4_000_000
# Configure encoder options for better quality/performance
codec.options = {
    'preset': 'ultrafast',  # Fast encoding
    'tune': 'zerolatency',  # Low latency
    # 'crf': '23',  # Quality level (lower = better quality)
    'maxrate': '1500k',  # Maximum bitrate
    'bufsize': '2000k',  # Buffer size
    'x264-params': 'repeat-headers=1'
}   
# stream.codec_context.options = {}

# Open the codec
codec.open()
cnt = 0
# Start screen capture
with mss.mss(with_cursor=True) as sct:
    monitor = {"top": 0, "left": 0, "width": width, "height": height}

    while True:  # Just 10 frames for demo
        sct_img = sct.grab(monitor)
        img = np.array(sct_img)

        # Convert BGRA to RGB (mss returns BGRA format)
        img_rgb = img[..., [2, 1, 0]]  # BGR to RGB conversion

        # Create VideoFrame
        frame = av.VideoFrame.from_ndarray(img_rgb, format='rgb24')
        frame.pts = cnt  # Set presentation timestamp
        
        # Reformat to yuv420p (this is done automatically by encoder if needed)
        # frame = frame.reformat(format='yuv420p')

        # Encode and collect packets
        packets = codec.encode(frame)
        for packet in packets:
            h264_bytes = bytes(packet)
            print("sending", len(h264_bytes))
            try: 
                send(client, h264_bytes)
            except:
                print("user quit")
                break
        
        # Add small delay to maintain frame rate
        # time.sleep(1/15)
        cnt +=1
    # Flush encoder
    packets = codec.encode(None)
    for packet in packets:
        h264_bytes = packet.to_bytes()
        # print(f"Flushed packet ({len(h264_bytes)} bytes)")

# Close the codec
# codec.close()