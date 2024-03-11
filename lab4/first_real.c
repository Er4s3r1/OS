#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int GCF(int A, int B) {
    while (B != 0) {
        int temp = B;
        B = A % B;
        A = temp;
    }
    return A;
}

char* translation(long x)
{
    char* binary = (char*)malloc(64 * sizeof(char)); // выделяем память под строку для двоичного представления
    int index = 0; // индекс для заполнения строки
    
    // Если входное число равно нулю, возвращаем строку "0"
    if (x == 0) {
        binary[0] = '0';
        binary[1] = '\0';
        return binary;
    }
    
    // Проверяем, является ли число отрицательным
    int isNegative = 0;
    if (x < 0) {
        isNegative = 1;
        x = (-1)*x; // берем модуль числа
    }

    // Переводим число в двоичную систему счисления
    while (x > 0) {
        binary[index++] = (x % 2) + '0'; // получаем остаток от деления и добавляем его в строку, преобразуя в символ
        x = x / 2; // делим на 2 для получения следующей цифры
    }
    
    if (isNegative) {
        binary[index++] = '-'; // если число было отрицательным, добавляем знак минуса в конец строки
    }
    
    binary[index] = '\0'; // добавляем завершающий символ
    
    // Переворачиваем строку, т.к. мы добавляли цифры в обратном порядке
    int i, j;
    char temp;
    for (i = 0, j = index - 1; i < j; i++, j--) {
        temp = binary[i];
        binary[i] = binary[j];
        binary[j] = temp;
    }
    return binary;
}
