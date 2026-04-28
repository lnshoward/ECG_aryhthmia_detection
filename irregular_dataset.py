import wfdb
import pandas as pd

print("Downloading MIT-BIH Record 106 (The 'Arrhythmia' Record)...")
try:
    record = wfdb.rdrecord('106', pn_dir='mitdb')
    df = pd.DataFrame(record.p_signal, columns=record.sig_name)
    df.to_csv('mit_bih_106.csv', index_label='Sample_Number')
    print("Success! 'mit_bih_106.csv' is ready for the stress test.")
except Exception as e:
    print(f"Error: {e}")
