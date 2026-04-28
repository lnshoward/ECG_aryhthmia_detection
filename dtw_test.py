import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# 1. Standard DTW Implementation (O(N*M))
def calculate_dtw(s1, s2):
    n, m = len(s1), len(s2)
    dtw_matrix = np.zeros((n+1, m+1))
    
    # Initialize with infinity
    dtw_matrix[1:, 0] = np.inf
    dtw_matrix[0, 1:] = np.inf
    dtw_matrix[0, 0] = 0
    
    # Fill the matrix
    for i in range(1, n+1):
        for j in range(1, m+1):
            cost = abs(s1[i-1] - s2[j-1])
            # The DP step: find the minimum cost to get to this cell
            dtw_matrix[i, j] = cost + min(dtw_matrix[i-1, j],    # Insertion
                                          dtw_matrix[i, j-1],    # Deletion
                                          dtw_matrix[i-1, j-1])  # Match
    return dtw_matrix[n, m]

# 2. Load and Segment Data
df = pd.read_csv('mit_bih_100.csv')
signal = df.iloc[:10000, 1].values * 1000

# We'll use the sample indices we saw in your previous run:
# Normal Beat at sample 366
# Early/Premature Beat at sample 2041 (approx)
normal_template = signal[366-128 : 366+128]
another_normal = signal[660-128 : 660+128]
premature_beat = signal[2041-128 : 2041+128]

# 3. Compare
dist_norm_vs_norm = calculate_dtw(normal_template, another_normal)
dist_norm_vs_early = calculate_dtw(normal_template, premature_beat)

print(f"DTW Distance (Normal vs Normal): {dist_norm_vs_norm:,.2f}")
print(f"DTW Distance (Normal vs Premature): {dist_norm_vs_early:,.2f}")

# 4. Visualize the comparison
plt.figure(figsize=(10, 4))
plt.plot(normal_template, label="Normal Template", color="green")
plt.plot(premature_beat, label="Premature Beat", color="red", linestyle="--")
plt.title("Visual Comparison: Normal vs. Premature Beat")
plt.legend()
plt.show()
