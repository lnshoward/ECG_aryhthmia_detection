#include <iostream>
#include <cstdint>

// Define constants based on project requirements
#define SAMPLE_RATE 360
#define BUFFER_SECONDS 10
#define BUFFER_SIZE (SAMPLE_RATE * BUFFER_SECONDS)

// Statically allocate the buffer to simulate embedded memory constraints
// int16_t is used because raw ECG data easily fits in 16 bits
int16_t ecg_buffer[BUFFER_SIZE];
int write_index = 0;

// Function to push a new ECG reading into the buffer
void push_sample(int16_t sample) {
    ecg_buffer[write_index] = sample;
    // The modulo operator (%) ensures the index wraps back to 0 when it hits 3600
    write_index = (write_index + 1) % BUFFER_SIZE; 
}

int main() {
    std::cout << "Initializing 10-second ECG Buffer..." << std::endl;
    std::cout << "Buffer Size: " << BUFFER_SIZE << " samples." << std::endl;

    // Simulate taking 3605 sensor readings (5 more than the buffer can hold)
    for (int i = 0; i < 3605; i++) {
        push_sample(i); 
    }

    std::cout << "Current write_index after 3605 insertions: " << write_index << std::endl;
    std::cout << "Value at index 0 (overwritten): " << ecg_buffer[0] << std::endl;
    std::cout << "Value at index 4 (overwritten): " << ecg_buffer[4] << std::endl;
    std::cout << "Value at index 5 (untouched from first loop): " << ecg_buffer[5] << std::endl;

    return 0;
}

