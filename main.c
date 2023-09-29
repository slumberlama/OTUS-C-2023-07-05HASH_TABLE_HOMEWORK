/* OTUS C-2023-07 Барашева Ангелина ДЗ к 6 занятию "Алгоритмы поиска и сортировки"
Реализация хеш-таблицы с открытой адресацией
С открытой адресацией это значит, что если наша хеш-функция не совершенна и в результате работы с ней мы хотим использовать уже занятую ячейку,
то нам придётся искать следующий свободный слот памяти и записать туда

Для создания хеш-функции нужно предположить, какое примерно количество разных слов в файле ожидается
кодировка, файла, таблица Windows CP-1251 для нахождения суммы слов

т.е. у нас будет 3 колонки ID, слово, частота его встречаемости

в самой программе мы берём файл, считываем все слова в один большой массив и работаем с массивом или работаем как с одной большой строкой, но строки скорее вссего будут
заканчиваться
смотрим, сколько у нас слов это делаем с помощью определения количества пробелов

можем создать тупо столько ячеек, сколько слов в файле, чтобы  точно хватило и коллизий не возникло, но ТО поиск по таблице будет занимать много времени, т.к.
достаточно ячеек будут пустыми

вход: путь файла, как массив / файл как параметр входа в программу
выход: частота встречаемости слова
возврат: 0 */

/*Глобальные переменные*/
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <sys/stat.h>

unsigned long count_word = 0; // количество слов в файле, size of the Hash Table

unsigned long hash_function(unsigned char * str) {
    unsigned long i = 0;
    for(int j = 0; str[j]; j++) {
        i += str[j];
    }
    //printf("%d\n", i);
    return i % count_word;
}

typedef struct Ht_item Ht_item;

// Define the Hash Table Item here
struct Ht_item {
    unsigned char * key;
    unsigned int value;
    unsigned int size_key;
};

typedef struct HashTable HashTable;

// Define the Hash Table here
struct HashTable {
    // Contains an array of pointers
    // to items
    Ht_item ** items;
    unsigned int size;
    unsigned int count;
};

unsigned int size_of_key = 0;    // размер ключа

/*----------------------------------------------------------------------------------------------

Функция:    create_item
Назначение: Функция "Создать пару ключ-значение"
Описание:   Функция создаёт пару ключ-значение
Вход:       key - ключ
			value - значение

Выход:      -
Возврат:    указатель на структуру типа Ht_item * - пара ключ-значение

-----------------------------------------------------------------------------------------------*/

Ht_item * create_item(unsigned char * key, unsigned int value) {
    // Creates a pointer to a new hash table item
    int i = 0;
    Ht_item*item = (Ht_item*)malloc(sizeof(Ht_item));
    item->size_key = 0;
    while(key[i]!=0) {item->size_key++; i++;}

    printf("%d\n", item->size_key);

    item->key = (unsigned char*)calloc(item->size_key + 1, sizeof(unsigned char));

    memcpy(item->key, key, item->size_key*sizeof(unsigned char));
    item->value = value;
    //item->size_key = size_of_key;

    return item;
}

HashTable * create_table(unsigned int size) {
    // Creates a new HashTable
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item**)calloc(table->size, sizeof(Ht_item*));
    for(int i = 0; i < table->size; i++) {
        table->items[i] = NULL;
    }
    //table->overflow_buckets = create_overflow_buckets(table);
    return table;
}

void free_item(Ht_item * item) {
    // Frees an item
    free(item->key);
    //free(item->value);
    free(item);
}

void free_hashtable(HashTable * table) {
    // Frees the table
    for(int i = 0; i < table->size; i++) {
        Ht_item * item = table->items[i];
        if(item != NULL) {free(item);}
    }
    //free_overflow_buckets(table);
    free(table->items);
    free(table);
}
// функция разрешает коллизию методом Linear probing
void handle_collision(HashTable * table, unsigned long index, Ht_item * item) {

    for(index = 0; index <= count_word; index++) {
        if(table->items[index] == NULL) {break;}
    }

    if( (index == count_word) && (table->items[count_word] != NULL) ) {
        printf("Insert Error: Hash Table is full\n");
        return;
    }

    table->items[index] = item;
    table->count++;
}

void ht_insert(HashTable * table, unsigned char * key, unsigned int value) {
    // Create the item
    Ht_item * item = create_item(key, value);

    // Compute the index
    unsigned long index = hash_function(key);

    Ht_item * current_item = table->items[index];

    if(current_item == NULL) {
        // Key does not exist
        if(table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item(item);
            return;
        }
        // Insert directly
        table->items[index] = item;
        table->count++;
    }
    else {
        // Scenario 1: We only need to increment value
        if(memcmp(current_item->key, key, current_item->size_key) == 0) {
            table->items[index]->value++; // inc frequency
            free_item(item);
            return;
        }
        else {
            // Scenario 2: Collision
            handle_collision(table, index, item);
            return;
        }
    }
}

int ht_serch(HashTable * table, unsigned char * key) {
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    int index = hash_function(key);

    //Ht_item * item = table->items[index];
    // Ensure thst we move to items which are not NULL
    while(table->items[index] != NULL) {
        if(memcmp(table->items[index]->key, key, table->items[index]->size_key) == 0) {
                return table->items[index]->value;
        }
        index++;
    }
    return 0;
}

void print_search(HashTable * table, unsigned char * key) {
    int val;
    if((val = ht_serch(table, key)) == 0) {
        printf("%s does not exist\n", key);
        return;
    }
    else {
        printf("Key:%s, Values:%d\n", key, val);
    }
}

void print_hashtable(HashTable * table) {
    int i;
    printf("\n-------------------\n");
    for(i = 0; i<table->size; i++) {
        if(table->items[i]) {
            printf("Index:%d, Слово:%s Частота:%d\n", i, table->items[i]->key, table->items[i]->value);
        }
        else {printf("Index:%d, e m p t y\n", i);}
    }
    printf("-------------------\n");
}

/*----------------------------------------------------------------------------------------------

Функция:    is_it_space_or_other_symbols
Назначение: Функция "Это буква?"
Описание:   Функция проверяет, не является ли входной символ пробелом
Вход:       c - символ типа unsigned char

Выход:      -
Возврат:    0 - пробел, 1 - другой символ

-----------------------------------------------------------------------------------------------*/

int is_it_space_or_other_symbols(unsigned char c) {
    if( (c < 0x2c) || ( (c > 0x2d) && (c < 0x41) ) ||
        ( (c > 0x5a) && (c < 0x61) ) ||
        ( (c > 0x7a) && (c < 0x80) ) || (c == ' ') || (c == ',') || (c == '"') ) {return 1;}
    else {return 0;}
}

/*----------------------------------------------------------------------------------------------

Функция:    is_not_space_or_other_symbols
Назначение: Функция "Это пробел?"
Описание:   Функция проверяет, не является ли входной символ пробелом
Вход:       c - символ типа unsigned char

Выход:      -
Возврат:    0 - пробел, 1 - другой символ

-----------------------------------------------------------------------------------------------*/

int is_not_space_or_other_symbols(unsigned char c) {
    return 0 == is_it_space_or_other_symbols(c);
}

// счёт начинается, если
int count_start_if(unsigned char * str, int (*pred)(unsigned char)) {
    int count = 0;
    while(*str!=0) {
        if(pred(*str)) {
            count++;
            str++;
        }
        else {break;}
    }
    return count;
}

// счётчик слов
unsigned long count_words(unsigned char* str) {
    //int length = strlen(str);

    unsigned long count = 0;
    int i = 0;
    while(1) {
        int spaces_count = count_start_if(&str[i], is_it_space_or_other_symbols);
        i += spaces_count;
        if(str[i] == '\0') {break;}

        int word_length = count_start_if(&str[i], is_not_space_or_other_symbols);
        if(word_length > 0) {count++;}

        i += word_length;
        if(str[i] == '\0') {break;}
    }
    return count;
}

int count_start_and_end_if(unsigned char * str, int (*pred)(unsigned char)) {
    int count = 0;
    while(*str!=0) {
        if(pred(*str)) {
            count++;
            str++;
        }
        else {break;}
    }
    return count;
}


int main() {    //int argc, char** argv
    struct stat file_t;         // структура для считывания параметров файла с помощью функции stat
    FILE * file;                // сюда будет считан файл
    char path[MAX_PATH];        // путь к файлу
    unsigned char * file_in;    // массив, в который будет записан файл

    setlocale(LC_ALL, "");
    printf("Введите путь к файлу: ");
    scanf("%s", path);          // вводим путь к файлу
    file = fopen(path, "r");

    if(file == NULL) {perror("Error"); exit(0);}    // проверка открытия файла
    stat(path, &file_t);        // в экземпляр структуры file_t записываем данные о файле по введённому пути

    file_in = (unsigned char*)malloc(file_t.st_size*sizeof(unsigned char));   // выделяем память под массив
    if(fread(file_in, 1, file_t.st_size, file) != file_t.st_size) {perror("Error"); exit(0);}
    //if(file == NULL) {perror("Error"); exit(0);}    // проверка успешности чтения из файла
    count_word = count_words(file_in);              // количество слов в файле
    printf("Количество слов: %ld\n", count_word);
    //если count_word == 0 выход

    HashTable * ht = create_table(count_word);  // создаём хэш-таблицу

    unsigned char word[count_word][30];  // массив для слов, 30 примерная максимальная длина слова
    memset(word, 0, count_word*30);
    int i = 0;  // счётчик количества символов в массиве
    for(int j = 0; j < count_word; j++) {
        while(1) {
            int spaces_or_other_symbols_count = count_start_if(&file_in[i], is_it_space_or_other_symbols);
            i += spaces_or_other_symbols_count;
            if(i >= file_t.st_size) {break;}

            int word_length = count_start_if(&file_in[i], is_not_space_or_other_symbols);
            if(word_length > 0) {

                memcpy(word[j], file_in+i, word_length);
            }
            i += word_length;
            if(i >= file_t.st_size) {break;}
            break;
        }
    }

    for(int z = 0; z < count_word; z++) {
        ht_insert(ht, word[z], 1);
    }

    print_hashtable(ht);
    print_search(ht, "как");
    free_hashtable(ht);
    fclose(file);
    free(file_in);
}
