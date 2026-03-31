import matplotlib.pyplot as plt
import numpy as np

n_val = []
times = []
swaps = []

with open("C:\\2_semestr\\papka\\output.txt", "r") as f:
    for line in f:
        n, t, s = map(float, line.split())
        n_val.append(int(n))
        times.append(t)
        swaps.append(int(s))

n_arr = np.array(n_val)
t_arr = np.array(times)
s_arr = np.array(swaps)

def fit_log_n(x, y):
    x_log = x * np.log(x)
    coeffs = np.polyfit(x_log, y, 1)
    return coeffs[0] * x_log + coeffs[1]

t_fit = fit_log_n(n_arr, t_arr)
s_fit = fit_log_n(n_arr, s_arr)

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

ax1.scatter(n_arr, t_arr, color='blue', label='Опыт')
ax1.plot(n_arr, t_fit, 'r--', label='Приближение O(N log N)')
ax1.set_title("Время работы")
ax1.set_xlabel("N")
ax1.set_ylabel("мс")
ax1.legend()
ax1.grid(True)

ax2.scatter(n_arr, s_arr, color='green', label='Опыт')
ax2.plot(n_arr, s_fit, 'r--', label='Приближение O(N log N)')
ax2.set_title("Количество перестановок")
ax2.set_xlabel("N")
ax2.set_ylabel("Кол-во")
ax2.legend()
ax2.grid(True)

plt.tight_layout()
plt.show()
