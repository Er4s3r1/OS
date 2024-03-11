#include <iostream>
#include <string.h>

// Максимальное количество значений в массиве
#define MAX_VALUES 100

// Структура для представления записи в словаре
struct KeyValuePair {
    char key[50];            // Ключ (строка)
    char values[MAX_VALUES][50];  // Массив значений (строк)
    int valueCount;          // Текущее количество значений в массиве
};

// Структура словаря
struct Dictionary {
    struct KeyValuePair entries[100];  // Массив записей в словаре
    int entryCount;           // Текущее количество записей в словаре
};


struct Dictionary createDictionary() {
    struct Dictionary dictionary;
    dictionary.entryCount = 0;
    return dictionary;
}


// Функция для добавления значения по ключу
void addToDictionary(struct Dictionary *dictionary, const char *key, const char *value) {
    // Ищем запись с таким ключом
    for (int i = 0; i < dictionary->entryCount; ++i) {
        if (strcmp(dictionary->entries[i].key, key) == 0) {
            // Нашли запись с таким ключом
            // Добавляем значение в массив
            if (dictionary->entries[i].valueCount < MAX_VALUES) {
                strcpy(dictionary->entries[i].values[dictionary->entries[i].valueCount], value);
                dictionary->entries[i].valueCount++;
            } else {
                printf("Превышено максимальное количество значений для ключа %s\n", key);
            }
            return;
        }
    }

    // Если запись с таким ключом не найдена, создаем новую запись
    if (dictionary->entryCount < 100) {
        strcpy(dictionary->entries[dictionary->entryCount].key, key);
        strcpy(dictionary->entries[dictionary->entryCount].values[0], value);
        dictionary->entries[dictionary->entryCount].valueCount = 1;
        dictionary->entryCount++;
    } else {
        printf("Превышено максимальное количество записей в словаре\n");
    }
}

// Функция для поиска ключа и возвращения значения по ключу
const char *findInDictionary(const struct Dictionary *dictionary, const char *key) {
    for (int i = 0; i < dictionary->entryCount; ++i) {
        if (strcmp(dictionary->entries[i].key, key) == 0) {
            // Нашли запись с таким ключом
            // Возвращаем первое значение из массива (если оно есть)
            if (dictionary->entries[i].valueCount > 0) {
                return dictionary->entries[i].values[0];
            } else {
                return "Нет значений для данного ключа";
            }
        }
    }

    // Если запись с таким ключом не найдена
    return "Ключ не найден";
}

// Функция для проверки наличия ключа в словаре
int keyExists(const struct Dictionary *dictionary, const char *key) {
    for (int i = 0; i < dictionary->entryCount; ++i) {
        if (strcmp(dictionary->entries[i].key, key) == 0) {
            // Нашли запись с таким ключом
            return 1; // Возвращаем 1, если ключ найден
        }
    }

    // Если запись с таким ключом не найдена
    return 0; // Возвращаем 0, если ключ не найден
}

// Функция для добавления ключа в словарь
void addKeyToDictionary(struct Dictionary *dictionary, const char *key) {
    // Проверяем, существует ли уже запись с таким ключом
    if (dictionary->entryCount < 100) {
        // Если запись с таким ключом не найдена, создаем новую запись
        strcpy(dictionary->entries[dictionary->entryCount].key, key);
        dictionary->entryCount++;
    } else {
        printf("Превышено максимальное количество записей в словаре\n");
    }
}


// Функция для поиска следующей строки в массиве по ключу
char* getNextValue(struct Dictionary* dictionary, const char* key, const char* currentValue) {
    for (int i = 0; i < dictionary->entryCount; i++) {
        if (strcmp(dictionary->entries[i].key, key) == 0) {
            for (int j = 0; j < dictionary->entries[i].valueCount; j++) {
                // Ищем текущее значение в массиве
                if (strcmp(dictionary->entries[i].values[j], currentValue) == 0) {
                    // Возвращаем следующее значение (или первое, если текущее последнее)
                    return dictionary->entries[i].values[(j + 1) % dictionary->entries[i].valueCount];
                }
            }
        }
    }
    return NULL;  // Если ключ или значение не найдены
}

void removeFromDictionary(struct Dictionary *dictionary, const char *key) {
    for (int i = 0; i < dictionary->entryCount; ++i) {
        if (strcmp(dictionary->entries[i].key, key) == 0) {
            // Нашли запись с ключом, удаляем её
            for (int j = i; j < dictionary->entryCount - 1; ++j) {
                // Сдвигаем оставшиеся записи влево
                strcpy(dictionary->entries[j].key, dictionary->entries[j + 1].key);
                memcpy(dictionary->entries[j].values, dictionary->entries[j + 1].values, sizeof(dictionary->entries[j].values));
                dictionary->entries[j].valueCount = dictionary->entries[j + 1].valueCount;
            }
            // Уменьшаем количество записей
            dictionary->entryCount--;
            break;
        }
    }
}

void removePersonFromGameDictionary(struct Dictionary *dictionary, const char *key, const char *valueToRemove) {
    for (int i = 0; i < dictionary->entryCount; ++i) {
        if (strcmp(dictionary->entries[i].key, key) == 0) {
            // Нашли запись с указанным ключом
            for (int j = 0; j < dictionary->entries[i].valueCount; ++j) {
                if (strcmp(dictionary->entries[i].values[j], valueToRemove) == 0) {
                    // Нашли строку для удаления из массива значений
                    // Сдвигаем оставшиеся элементы массива
                    for (int k = j; k < dictionary->entries[i].valueCount - 1; ++k) {
                        strcpy(dictionary->entries[i].values[k], dictionary->entries[i].values[k + 1]);
                    }
                    // Уменьшаем счетчик значений
                    dictionary->entries[i].valueCount--;
                    printf("String '%s' removed from key '%s'\n", valueToRemove, key);
                    return;
                }
            }
        }
    }

    printf("String '%s' not found for key '%s'\n", valueToRemove, key);
}
