#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>

const double D_PI = 3.14159265358979;

float maxwell_pdf(float v, float T) {
    float norm_coeff = 1.0f / (float)std::sqrt(T * D_PI);
    return norm_coeff * std::exp(-(v * v) / T);
}

// интегрирование втупую складывая прямоугольники: плохо из-за накопления ошибки плавпющей точки
float integrate_stupid(const float psi[], const float pdf[], float dv, unsigned n) {
    float sum = 0.0f;
    for (unsigned i = 0; i < n; ++i) sum += psi[i] * pdf[i];
    return sum * dv;
}

// интегрирование через рекурсию и разделяй и властвуй: повышает точность с ON до OlogN
// нет стак оверфлоу т.к. для ляма log2(1000000)=20, вызов на 8(адрес возврата)+8(адрес data)+4(число n)+<16(спец регистры)=36 байт, 20*36байт<1Кб (стек 1-8Мб)
float integrate_recursive_engine(const float data[], unsigned n) {
    if (n == 1) return data[0];
    return integrate_recursive_engine(data, n / 2) + integrate_recursive_engine(data + n / 2, n - (n / 2));
} //странное вычитание выше нужно чтоб не потерять элемент при неч n 
float integrate_recursive(const float psi[], const float pdf[], float dv, unsigned n) {
    float* temp = new float[n];
    for (unsigned i = 0; i < n; ++i) temp[i] = psi[i] * pdf[i]; //чтобы не тянуть в рекурсию сразу 2 массива, а только 1
    float res = integrate_recursive_engine(temp, n);
    delete[] temp;
    return res * dv;
}

// через цикл, симулируя рекурсию
float integrate_pairs(const float psi[], const float pdf[], float dv, unsigned n) {
    float* temp = new float[n];
    for (unsigned i = 0; i < n; ++i) temp[i] = psi[i] * pdf[i];
    unsigned current_n = n;
    while (current_n > 1) {
        unsigned next_n = current_n / 2;
        for (unsigned i = 0; i < next_n; ++i) {
            temp[i] = temp[2 * i] + temp[2 * i + 1];
        }
        if (current_n % 2 == 1) { // поправка в случае нечет n
            temp[next_n] = temp[current_n - 1];
            next_n++; //учет что a/b округл вниз для int: просто округл вверх
        }
        current_n = next_n;
    }
    float res = temp[0];
    delete[] temp;
    return res * dv;
}

float integrate_kahan(const float psi[], const float pdf[], float dv, unsigned n) {
    float sum = 0.0f, c = 0.0f;
    for (unsigned i = 0; i < n; ++i) {
        float y = (psi[i] * pdf[i]) - c;
        float t = sum + y; // в t  может быть немного меньшая часть y из-за округления
        c = (t - sum) - y; // c<0 потерянные гроши с обратным знаком: дальше прибавляются с нужным знаком получая реальную сумму
        sum = t;
    }
    return sum * dv;
} //крутой метод, правда если оптимизировать то станет integrate_stuptd, и ариф операций много. Зато если не хватает места на дабл - самое то

// Fused Multiplication-Addition: сначала a*b, потом +c В ОДИН ШАГ с окр в конце, убирая ошибки на умножении...
float integrate_fma(const float psi[], const float pdf[], float dv, unsigned n) {
    float sum = 0.0f;
    for (unsigned i = 0; i < n; ++i) sum = std::fma(psi[i], pdf[i], sum);
    return sum * dv;
} // ...но проблему лям+один не решает :(

// ВОТ ОН, САМЫЙ ПРАКТИЧНЫЙ КРУТОЙ ЭФФЕКТИВНЫЙ МЕГАМЕТОД ЧЕРЕЗ ДАБЛ (дает 15 (!!!) знаков точности мантиссы, флоут дает 7)
double integrate_double(const float psi[], const float pdf[], float dv, unsigned n) {
    double sum = 0.0;
    for (unsigned i = 0; i < n; ++i) sum += static_cast<double>(psi[i]) * static_cast<double>(pdf[i]);
    return sum * static_cast<double>(dv);
}

void run_test(float T, unsigned n, std::ofstream& out) {
    float sigma = std::sqrt(T / 2.0f);
    float v_max = 8.0f * sigma; //8 сигм это 99.9997% точности
    float v_min = -v_max;
    float dv = (v_max - v_min) / n;

    float* psi_abs = new float[n];
    float* psi_sq = new float[n];
    float* pdf = new float[n];

    for (unsigned i = 0; i < n; ++i) {
        float v = v_min + i * dv;
        psi_abs[i] = std::abs(v);
        psi_sq[i] = v * v;
        pdf[i] = maxwell_pdf(v, T);
    }

    double exact_abs = std::sqrt(T / D_PI);
    double exact_sq = T / 2.0; // эталон, <v^2> = T/2

    out << "T=" << T << "   n=" << n << "   dv=" << std::scientific << std::setprecision(7) << dv << "\n";
    out << std::left << std::setw(20) << "Метод" << "   " << std::right << std::setw(20) << "Ошибка <|v|>" << "   " << std::setw(20) << "Ошибка <v^2>" << "\n";

    auto write_row = [&](const char* name, double res_abs, double res_sq) {
        out << std::left << std::setw(20) << name << "   " 
            << std::right << std::scientific << std::setprecision(7) 
            << std::setw(20) << std::abs(res_abs - exact_abs) << "   "
            << std::setw(20) << std::abs(res_sq - exact_sq) << "\n";
    };

    write_row("Втупую", integrate_stupid(psi_abs, pdf, dv, n), integrate_stupid(psi_sq, pdf, dv, n));
    write_row("Рекурсия", integrate_recursive(psi_abs, pdf, dv, n), integrate_recursive(psi_sq, pdf, dv, n));
    write_row("Циклом", integrate_pairs(psi_abs, pdf, dv, n), integrate_pairs(psi_sq, pdf, dv, n));
    write_row("Кэхэн", integrate_kahan(psi_abs, pdf, dv, n), integrate_kahan(psi_sq, pdf, dv, n));
    write_row("FMA", integrate_fma(psi_abs, pdf, dv, n), integrate_fma(psi_sq, pdf, dv, n));
    write_row("Дабл", integrate_double(psi_abs, pdf, dv, n), integrate_double(psi_sq, pdf, dv, n));
    
    out << "\n";

    delete[] psi_abs; delete[] psi_sq; delete[] pdf;
}


int main() {
    std::ofstream out("output.txt");
    float temperatures[] = {0.1f, 1.0f, 100.0f};
    unsigned point_counts[] = {10, 100, 500, 1000, 10000, 100000};
    for (float t : temperatures) {
        for (unsigned n : point_counts) {
            run_test(t, n, out);
        }
    }
    out.close();
    return 0;
}
