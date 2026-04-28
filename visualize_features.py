import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load data
df = pd.read_csv('mit_bih_100.csv')
signal = df.iloc[:10000, 1].values * 1000 

# Simplified detection logic (same as your C++)
threshold = 50000
refractory = 72
peaks = []
prev = signal[0]

for i in range(1, len(signal)):
    diff = signal[i] - prev
    if (diff**2) > threshold and (not peaks or (i - peaks[-1]) > refractory):
        peaks.append(i)
    prev = signal[i]

# 1. Plot RR-Intervals
rr_intervals = np.diff(peaks) / 360.0
plt.figure(figsize=(12, 4))
plt.subplot(1, 2, 1)
plt.plot(rr_intervals, 'o-', color='#e67e22')
plt.title('RR-Interval Stability (Seconds)')
plt.ylabel('Seconds')
plt.xlabel('Beat Number')
plt.grid(True, alpha=0.3)

# 2. Overlay Segments (The "Stack" Plot)
plt.subplot(1, 2, 2)
for p in peaks[1:-1]: # Skip first/last to ensure we have room
    segment = signal[p-128 : p+128]
    if len(segment) == 256:
        plt.plot(segment, alpha=0.3, color='#3498db')

plt.title('Segment Overlay (All Beats)')
plt.axvline(x=128, color='red', linestyle='--', label='R-Peak Center')
plt.xlabel('Samples (Window)')
plt.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()
