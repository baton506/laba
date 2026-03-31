import matplotlib.pyplot as plt
import numpy as np

results = {1: {"n": [], "t": [], "s": [], "label": "Shell (N/2)"},
           2: {"n": [], "t": [], "s": [], "label": "Hibbard"},
           3: {"n": [], "t": [], "s": [], "label": "Fibonacci"}}

with open("C:\\2_semestr\\papka\\output.txt", "r") as f:
    for line in f:
        t_id, n, time, swaps = map(float, line.split())
        r = results[int(t_id)]
        r["n"].append(n)
        r["t"].append(time)
        r["s"].append(swaps)

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

for rid in results:
    r = results[rid]
    n_arr, t_arr, s_arr = np.array(r["n"]), np.array(r["t"]), np.array(r["s"])

    k_t = np.polyfit(np.log(n_arr), np.log(t_arr), 1)[0]
    t_fit = np.exp(np.polyval(np.polyfit(np.log(n_arr), np.log(t_arr), 1), np.log(n_arr)))

    ax1.scatter(n_arr, t_arr, label=f"{r['label']} (k={k_t:.2f})")
    ax1.plot(n_arr, t_fit, '--')

    k_s = np.polyfit(np.log(n_arr), np.log(s_arr), 1)[0]
    s_fit = np.exp(np.polyval(np.polyfit(np.log(n_arr), np.log(s_arr), 1), np.log(n_arr)))

    ax2.scatter(n_arr, s_arr, label=f"{r['label']} (k={k_s:.2f})")
    ax2.plot(n_arr, s_fit, '--')

ax1.set_title("Время работы (T ≈ N^k)")
ax1.set_xlabel("N")
ax1.set_ylabel("мс")
ax1.legend()
ax1.grid(True)

ax2.set_title("Перестановки (S ≈ N^k)")
ax2.set_xlabel("N")
ax2.set_ylabel("Кол-во")
ax2.legend()
ax2.grid(True)

plt.tight_layout()
plt.show()
