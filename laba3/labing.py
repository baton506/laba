import matplotlib.pyplot as plt
import numpy as np
import re


def parse_output(filename):
    data = []
    header_re = re.compile(r"T=([\d.eE+-]+)\s+n=(\d+)")
    row_re = re.compile(r"^([А-Яа-яA-Za-z]+)\s+([\d.eE+-]+)\s+([\d.eE+-]+)")
    curr_t, curr_n = None, None

    with open(filename, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            h = header_re.search(line)
            if h:
                curr_t, curr_n = float(h.group(1)), int(h.group(2))
                continue
            r = row_re.search(line)
            if r and curr_t is not None:
                exact_abs = np.sqrt(curr_t / np.pi)
                exact_sq = curr_t / 2.0
                err_abs = (float(r.group(2)) / exact_abs) * 100  # ПОИСК ОТН ПОГР
                err_sq = (float(r.group(3)) / exact_sq) * 100  # ПОИСК ОТН ПОГР
                data.append({'n': curr_n, 't': curr_t, 'method': r.group(1), 'err_abs': err_abs, 'err_sq': err_sq})
    return data


def plot_by_temperature(data):
    unique_t = sorted(list(set(d['t'] for d in data)))
    unique_n = sorted(list(set(d['n'] for d in data)))
    methods = list(dict.fromkeys(d['method'] for d in data))

    for t_val in unique_t:
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
        t_data = [d for d in data if d['t'] == t_val]

        x = np.arange(len(unique_n))
        width = 0.12

        for i, m in enumerate(methods):
            y_abs = [next((d['err_abs'] for d in t_data if d['n'] == n and d['method'] == m), 1e-15) for n in unique_n]
            y_sq = [next((d['err_sq'] for d in t_data if d['n'] == n and d['method'] == m), 1e-15) for n in unique_n]

            ax1.bar(x + i * width, y_abs, width, label=m)
            ax2.bar(x + i * width, y_sq, width, label=m)

        for ax, label in zip([ax1, ax2], ["<|v|>", "<v^2>"]):
            ax.set_title(f'Ошибка {label} при T = {t_val}')
            ax.set_xlabel('Количество точек (n)')
            ax.set_ylabel('Относительная ошибка (%)')
            ax.set_xticks(x + width * (len(methods) / 2 - 0.5))
            ax.set_xticklabels(unique_n)
            ax.set_yscale('log')
            ax.grid(True, which="both", ls="--", alpha=0.4)
            ax.legend()

        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    results = parse_output("C:\\2_semestr\\papka\\output.txt")
    if results:
        plot_by_temperature(results)
