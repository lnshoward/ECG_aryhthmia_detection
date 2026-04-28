import wfdb
import pandas as pd

print("Connecting to PhysioNet...")
print("Downloading MIT-BIH Record 100 (this might take a few seconds)...")

try:
    # rdrecord downloads the signal data directly from the 'mitdb' database online
    record = wfdb.rdrecord('100', pn_dir='mitdb')

    # Convert the raw signal data into a structured Pandas DataFrame
    # record.sig_name usually contains the lead names (e.g., 'MLII', 'V5')
    df = pd.DataFrame(record.p_signal, columns=record.sig_name)

    # Save it to a CSV file in your current directory
    csv_filename = 'mit_bih_100.csv'
    df.to_csv(csv_filename, index_label='Sample_Number')

    print(f"Success! Saved exactly {len(df)} samples (30 minutes of ECG data) to {csv_filename}")
    print("Ready for processing.")

except Exception as e:
    print(f"An error occurred: {e}")
