import socket
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

HOST = "0.0.0.0"
PORT = 2000
NUM_SAMPLES = 262144
FFT_SIZE = 512
SAMPLING_FREQUENCY = 10000000
FFT_BLOCKS = (NUM_SAMPLES//2) // FFT_SIZE

data = np.zeros(NUM_SAMPLES)
fft_data = np.zeros((FFT_BLOCKS, FFT_SIZE))

def recv_samples():
  global conn
  raw_data = b""
  while len(raw_data) < NUM_SAMPLES:
    packet = conn.recv(NUM_SAMPLES - len(raw_data))
    if not packet:
      return np.zeros(NUM_SAMPLES)
    raw_data += packet
  return raw_data[:NUM_SAMPLES]
  
def update(frame):
  global data
  global fft_data
  data = recv_samples()
  data_array = np.frombuffer(data, dtype=np.int8)
  complex_data = data_array[0::2] + 1j * data_array[1::2]
  complex_data /= 128
  for i in range(FFT_BLOCKS):
    segment = complex_data[i * FFT_SIZE:(i+1) * FFT_SIZE]
    fft_vals = np.fft.fftshift(np.fft.fft(segment))
    fft_mag_chunk = 10 * np.log10(np.abs(fft_vals) ** 2)
    fft_data[i, :] = fft_mag_chunk
  im.set_data(fft_data)
  return [im]

#setup plot
fig, ax = plt.subplots()
im = ax.imshow(fft_data, aspect='auto', vmin=0, vmax=40, origin='lower', cmap='Oranges')
num_ticks = 5  # adjust this value as needed
xticks = np.linspace(1, FFT_SIZE, num_ticks)
xfreqs = ["{:.2f}".format(x) for x in xticks * SAMPLING_FREQUENCY  / (FFT_SIZE*2*1e6) ]
ax.set_xticks(xticks)
ax.set_xticklabels(xfreqs)
fig.colorbar(im, ax=ax)
ax.set_xlabel('Frequency')
ax.set_ylabel('Time Index')
ax.set_title('Real-time FFT Spectrogram')

#setup TCP connection
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((HOST, PORT))
sock.listen(1)
print(f"Server listening on {HOST}:{PORT}")
conn, addr = sock.accept()

#start animation
ani = FuncAnimation(fig, update, frames=np.arange(1000), interval=30, cache_frame_data=False)
plt.show()
