#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int GCF(int A, int B) {
    int min = A < B ? A : B; // Находим минимальное из двух чисел
    int gcf = 1; // Переменная для хранения наибольшего общего делителя

    for (int i = 1; i <= min; i++) {
        if (A % i == 0 && B % i == 0) {
            gcf = i; // Если i делит оба числа, обновляем значение наибольшего общего делителя
        }
    }

    return gcf; // Возвращаем наибольший общий делитель
}

char* translation(long x) {
    if (x == 0) {
        char* result = (char*)malloc(2 * sizeof(char)); // выделение памяти под "0\0"
        result[0] = '0';
        result[1] = '\0';
        return result;
    }

    int num = labs(x); // берем модуль числа

    int count = 0;
    long temp = num;
    while (temp > 0) {
        temp = temp / 3;
        count++;
    }

    char* result = (char*)malloc((count + 1) * sizeof(char)); // выделение памяти под троичное представление числа

    int index = count - 1;
    while (num > 0) {
        result[index] = (num % 3) + '0';
        num = num / 3;
        index--;
    }
    result[count] = '\0';

   if (x < 0) {
        char* finalResult = (char*)malloc((count + 2) * sizeof(char));
        finalResult[0] = '-'; // добавляем знак минуса в начало строки
        for (int i = 0; i <= count; i++) {
            finalResult[i + 1] = result[i]; // добавляем троичное представление числа после знака
        }
        free(result); // освобождаем память, выделенную для временного троичного представления
        return finalResult;
    }

    return result;
}
