import pandas as pd
import matplotlib.pyplot as plt

# 1. Load the dataset you just downloaded
file_path = 'mit_bih_100.csv'
print(f"Loading {file_path}...")

df = pd.read_csv(file_path)

# 2. Extract 10 seconds of data (360 Hz * 10 seconds = 3600 samples)
# Assuming the CSV has an index column, the first ECG lead (MLII) is usually column 1
# We multiply by 1000 to convert the raw mV to a more readable integer scale for later C++ work
ecg_signal = df.iloc[:3600, 1].values * 1000 

# 3. Plot the signal
plt.figure(figsize=(14, 5))
plt.plot(ecg_signal, color='#2c3e50', linewidth=1)
plt.title('MIT-BIH Record 100 - First 10 Seconds (360 Hz)')
plt.xlabel('Sample Number')
plt.ylabel('Amplitude (Scaled)')
plt.grid(True, linestyle='--', alpha=0.7)

plt.tight_layout()
plt.show()

