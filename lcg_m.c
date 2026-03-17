#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <locale.h>

#define ull unsigned long long
//#define m 18446744073709551616 // 2^64
//====== Параметры генератора ======
ull a =4207992786397510733ULL;
ull c = 1;
int size = 1000000;
uint64_t lcg_state;

void lcg_init(uint64_t seed){
    lcg_state = seed;
}

uint64_t lcg_next(void) {
    lcg_state = lcg_state * a + c;
    return lcg_state;
}

//void theorem()
//{
//        ull lower = (ull)(0.01 * m);
//        ull upper = (ull)(0.99 * m);
//        for (ull a = lower; a <= upper; a++)
//        {
//            if (a % 8 == 5) printf("%llu\n", a);
//        }
//}

// Мощность (a-1)^s ≡ 0 mod m
void Power() {
    ull b = a - 1;
    for (int s = 1; s <= 64; s++) {
        if (b == 0) 
        { 
            printf("Мощность = %d\n", s);
            return;
        }
        b = b * (a - 1);
    }
    printf("Мощность не найдена за 64 шага\n");
}


void Period() {
    ull seed = rand();
    lcg_init(seed);
    
    ull first = lcg_next();
    ull current = lcg_next();
    ull period = 1;
    while (current != first && period != 0) {
        current = lcg_next();
        period++;
    }
    if (period == 0) {
        printf("Период | 2^64 (полный период, счетчик переполнился)\n");
    } else {
        printf("Период | %llu\n", period);
    }
}

void razBROs_s_hikv() {
    const int k = 100;
    const int tests = 100;
      double crit[6] = { 69.22989, 77.84633, 81.49925, 117.48688, 123.22522, 134.64162 };
    int result[4] = {0};

    for (int t = 0; t < tests; t++) {
        lcg_init(rand());
        int intervalK[100] = {0};

        for (int i = 0; i < size; i++) {
            uint64_t val = lcg_next();
            int idx = (int)(((__uint128_t)val * k) >> 64);
            intervalK[idx]++;
        }

        if (t == 0) {
            printf("Разброс по 100 интервалам (первый прогон):\n");
            double exp = (double)size / k;
            for (int i = 0; i < k; i++) {
                printf("Интервал %3d: %d (откл %+6.2f%%)\n", i, intervalK[i], (intervalK[i]-exp)/exp*100);
            }
        }

        double expected = (double)size / k;
        double hi2 = 0.0;
        for (int i = 0; i < k; i++) {
            double diff = (double)intervalK[i] - expected;
            hi2 += diff * diff / expected;
        }

        if (hi2 <= crit[0] || hi2 >= crit[5])
            result[0]++;
        else if (hi2 <= crit[1] || hi2 >= crit[4])
            result[1]++;
        else if (hi2 <= crit[2] || hi2 >= crit[3])
            result[2]++;
        else
            result[3]++;
    }

    
    printf("\nХи-квадрат тест (%d прогонов, интервалов = %d):\n", tests, k);
    printf("  Подходящие          : %d\n", result[3]);
    printf("  Почти подозрительные: %d\n", result[2]);
    printf("  Подозрительные      : %d\n", result[1]);
    printf("  Недостаточно случайные: %d\n", result[0]);

    double bad_ratio = (result[0] + result[1] + result[2]) / (double)tests;
    if (bad_ratio > 2.0 / 3.0)
        printf("  Итог: параметры генератора ПЛОХИЕ (доля плохих > 2/3)\n");
    else
        printf("  Итог: параметры генератора ПРИЕМЛЕМЫ\n");
}

// Функция подсчета уникальных чисел в руке (упрощенный покер по Кнуту)
int count_distinct(int hand[5]) {
    int unique = 0;
    for (int i = 0; i < 5; i++) {
        int j;
        for (j = 0; j < i; j++)
            if (hand[i] == hand[j]) break;
        if (i == j) unique++;
    }
    return unique; // Возвращает от 1 до 5
}

void knut_poker() {
    ull seed = rand();
    lcg_init(seed);
    int count_hands = size / 5;
    int observed[4] = {0}; // 0: <=2 разл., 1: 3 разл., 2: 4 разл., 3: 5 разл.

    uint64_t step = 0xFFFFFFFFFFFFFFFFULL / 13;
    uint64_t limit = step * 13;

    for (int h = 0; h < count_hands; h++) {
        int hand[5];
        for (int i = 0; i < 5; i++) {
            uint64_t x;
            do { x = lcg_next(); } while (x >= limit);
            hand[i] = (int)(x / step);
        }
        
        int distinct = count_distinct(hand);
        
        
        if (distinct <= 2) observed[0]++; 
        else if (distinct == 3) observed[1]++;
        else if (distinct == 4) observed[2]++;
        else if (distinct == 5) observed[3]++;
    }

    double proba[4] = {0.006337, 0.115542, 0.462169, 0.415952};
    double chi2 = 0.0;
    double n = (double)count_hands;
    
    char* labels[] = {"1 или 2 разных", "3 разных", "4 разных", "5 разных"};
    
    for (int i = 0; i < 4; i++) {
        double expected = n * proba[i];
        double diff = observed[i] - expected;
        chi2 += (diff * diff) / expected;
        printf("  %-15s: набл. = %d, ожид. = %.2f\n", labels[i], observed[i], expected);
    }

    printf("Итоговое Хи-квадрат: %.4f\n", chi2);

    double crit_v[6] = { 0.015, 0.352, 0.584, 6.251, 7.815, 11.345 };
    if (chi2 <=crit_v[0] || chi2 >=crit_v[5])
        printf("  Результат: ПЛОХО\n");
    else if (chi2 <= crit_v[1] || chi2 >= crit_v[4])
        printf("  Результат: ПОЧТИ ПОДОЗРИТЕЛЬНО\n");
    else if (chi2 <= crit_v[2] || chi2 >= crit_v[3])
        printf("  Результат: ПОДОЗРИТЕЛЬНО\n");
    else
        printf("  Результат: ПРИЕМЛЕМО\n");
}


int main() {
    setlocale(LC_ALL, ""); 
    srand(time(NULL));
    printf("\n=== Параметры лкг ===\n");
    printf("a=%llu\n",a);
    printf("m=2^64\n");
    printf("c=%llu\n",c);
    printf("\n=== Тест мощности ===\n");
    Power();

    printf("\n=== Тест периода ===\n");
    printf("Период=m(2^64)\n");
    //Period(); 12,5 - теоретический!!

    printf("\n=== Тест разброса и хи-квадрат ===\n");
    razBROs_s_hikv();

    printf("\n=== Покер_критерий ===\n");
    knut_poker();

    return 0;п
}

