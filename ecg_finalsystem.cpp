#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iomanip>

// --- Configuration ---
#define SAMPLE_RATE 360
#define BUFFER_SIZE 3600
#define WINDOW_SIZE 256
#define HALF_WINDOW 128

// --- Final Calibrated Thresholds ---
#define QRS_THRESHOLD 50000      
#define REFRACTORY_PERIOD 72     
#define DTW_THRESHOLD 25000.0    // Increased to account for natural variability
#define RR_LOW_THRESHOLD 0.65    
#define RR_HIGH_THRESHOLD 1.2    

enum SystemState { DETECTION_MODE, COLLECTION_MODE };

int16_t ecg_buffer[BUFFER_SIZE];
int16_t normal_template[WINDOW_SIZE];
int16_t current_segment[WINDOW_SIZE];
bool template_ready = false;

float dtw_matrix[WINDOW_SIZE + 1][WINDOW_SIZE + 1];

// --- 1. Normalized DTW Engine ---
float calculate_dtw(int16_t* template_beat, int16_t* live_beat) {
    float mean_temp = 0, mean_live = 0;
    for(int i = 0; i < WINDOW_SIZE; i++) {
        mean_temp += template_beat[i];
        mean_live += live_beat[i];
    }
    mean_temp /= WINDOW_SIZE;
    mean_live /= WINDOW_SIZE;

    for (int i = 0; i <= WINDOW_SIZE; i++) {
        for (int j = 0; j <= WINDOW_SIZE; j++) dtw_matrix[i][j] = 1e9;
    }
    dtw_matrix[0][0] = 0;

    for (int i = 1; i <= WINDOW_SIZE; i++) {
        for (int j = 1; j <= WINDOW_SIZE; j++) {
            float val_t = (float)template_beat[i - 1] - mean_temp;
            float val_l = (float)live_beat[j - 1] - mean_live;
            
            float cost = std::abs(val_t - val_l);
            float min_prev = std::min({dtw_matrix[i - 1][j], 
                                       dtw_matrix[i][j - 1], 
                                       dtw_matrix[i - 1][j - 1]});
            dtw_matrix[i][j] = cost + min_prev;
        }
    }
    return dtw_matrix[WINDOW_SIZE][WINDOW_SIZE];
}

// --- 2. Classification Engine ---
void classify_beat(int total_count, float rr_interval) {
    if (!template_ready) {
        for (int i = 0; i < WINDOW_SIZE; i++) normal_template[i] = current_segment[i];
        template_ready = true;
        std::cout << "[SYSTEM] Initialized: Normal template stored.\n";
        return;
    }

    float dtw_score = calculate_dtw(normal_template, current_segment);
    std::string diagnosis = "NORMAL SINUS RHYTHM";
    std::string alert_level = "GREEN - STABLE";

    // Diagnostic Logic
    if (dtw_score > DTW_THRESHOLD) {
        diagnosis = "PVC (Premature Ventricular Contraction)";
        alert_level = "RED - CRITICAL";
    } else if (rr_interval < RR_LOW_THRESHOLD) {
        diagnosis = "APC (Atrial Premature Contraction)";
        alert_level = "YELLOW - WARNING (Early Beat)";
    } else if (rr_interval > RR_HIGH_THRESHOLD) {
        diagnosis = "BRADYCARDIAC EVENT";
        alert_level = "YELLOW - WARNING (Slow Rate)";
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n==========================================\n";
    std::cout << " BEAT AT " << (float)(total_count - HALF_WINDOW) / SAMPLE_RATE << "s | Score: " << dtw_score << "\n";
    std::cout << "------------------------------------------\n";
    std::cout << " Status    : " << alert_level << "\n";
    std::cout << " Diagnosis : " << diagnosis << "\n";
    std::cout << " RR Interval: " << rr_interval << "s\n";
    std::cout << "==========================================\n";
}

// --- 3. Core Processing Pipeline ---
SystemState current_state = DETECTION_MODE;
int write_index = 0;
int last_beat_sample = -REFRACTORY_PERIOD;
int samples_since_detection = 0;
int16_t previous_sample = 0;
int r_peak_index = 0;

void process_sample(int16_t sample, int total_count) {
    ecg_buffer[write_index] = sample;

    if (current_state == DETECTION_MODE) {
        int32_t derivative = sample - previous_sample;
        if ((derivative * derivative) > QRS_THRESHOLD && (total_count - last_beat_sample) > REFRACTORY_PERIOD) {
            r_peak_index = write_index;
            current_state = COLLECTION_MODE;
            samples_since_detection = 0;
        }
    } 
    else if (current_state == COLLECTION_MODE) {
        samples_since_detection++;
        if (samples_since_detection >= HALF_WINDOW) {
            for (int i = 0; i < WINDOW_SIZE; i++) {
                int pos = (r_peak_index - HALF_WINDOW + i + BUFFER_SIZE) % BUFFER_SIZE;
                current_segment[i] = ecg_buffer[pos];
            }
            float rr = (float)(total_count - HALF_WINDOW - last_beat_sample) / SAMPLE_RATE;
            classify_beat(total_count, rr);
            last_beat_sample = total_count - HALF_WINDOW;
            current_state = DETECTION_MODE;
        }
    }
    previous_sample = sample;
    write_index = (write_index + 1) % BUFFER_SIZE;
}

int main() {
    std::ifstream file("mit_bih_106.csv");
    if (!file.is_open()) return 1;

    std::cout << "Starting Final Calibrated ECG Detection...\n";
    std::string line;
    std::getline(file, line); 

    int total_samples = 0;
    while (std::getline(file, line) && total_samples < 8000) { 
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ','); 
        if (std::getline(ss, token, ',')) {
            try {
                int16_t val = static_cast<int16_t>(std::stof(token) * 1000.0f);
                process_sample(val, total_samples);
                total_samples++;
            } catch (...) {}
        }
    }
    file.close();
    return 0;
}
