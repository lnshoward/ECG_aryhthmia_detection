# Real-Time ECG Arrhythmia Detection System
**Language:** C++17 / Python 3.10  
**Dataset:** MIT-BIH Arrhythmia Database  

## 🚀 Features
- **Real-Time Circular Buffer:** 3600-sample static buffer for low-SRAM devices.
- **FSM Segmentation:** Finite State Machine for 256-sample window extraction.
- **DTW Classifier:** Dynamic Programming-based shape matching.
- **Baseline Wander Removal:** Zero-mean normalization for drift-resilient detection.

## 🛠️ Usage
1. **Prepare Dataset:** Run `fetch_dataset.py` to download MIT-BIH CSVs.
2. **Compile Engine:** `g++ ecg_final_system.cpp -o ecg_detector`
3. **Execute:** `./ecg_detector`

## 📊 Performance
Validated on Record 106: Successfully differentiates PVCs from Normal Sinus Rhythm using a calibrated DTW threshold of 35,000.
