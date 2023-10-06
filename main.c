/* OTUS C-2023-07 Барашева Ангелина ДЗ к 6 занятию "Алгоритмы поиска и сортировки"
Реализация хеш-таблицы с открытой адресацией

вход: путь файла
выход: частота встречаемости слова
возврат: 0 */

/*Глобальные переменные*/
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <sys/stat.h>

unsigned long count_word = 0; // количество слов в файле, size of the Hash Table

/*----------------------------------------------------------------------------------------------

Функция:    hash_function
Назначение: Функция "Хеш-функция"
Описание:   Функция вычисляет хеш-значения, складывая элементы в строке и находит остаток от
            деления на количество всех слов в файле
Вход:       str - массив символов типа unsigned char

Выход:      -
Возврат:    значение хеш-функции

-----------------------------------------------------------------------------------------------*/

const int p = 170;  // сумма русских и английских строчных и заглавныз букв

unsigned long hash_function(unsigned char * str) {
    unsigned long hash = 0, p_pow = 1;
    for(size_t i = 0; str[i]; i++) {
        hash += (str[i] + 1) * p_pow;
        p_pow *= p;
    }
    return hash % count_word;
}

// Структура пары ключ значение в хеш-таблице
typedef struct Ht_item Ht_item;

// Структура пары ключ значение в хеш-таблице
struct Ht_item {
    unsigned char * key;    // Слово из файла, ключ
    unsigned int value;
    unsigned int size_key;
};

typedef struct HashTable HashTable;

struct HashTable {
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

Ht_item * create_item(unsigned char* key, unsigned int value) {
    int i = 0;
    Ht_item* item = (Ht_item*)calloc(1, sizeof(Ht_item));
    if(!item) {printf("Allocation error"); exit(1);}

    while(key[i]!=0) {item->size_key++; i++;}

    item->key = (unsigned char*)calloc(item->size_key + 1, sizeof(unsigned char));

    memcpy(item->key, key, (item->size_key+1)*sizeof(unsigned char));
    item->value = value;

    return item;
}

/*----------------------------------------------------------------------------------------------

Функция:    create_table
Назначение: Функция "Создать хеш-таблицу"
Описание:   Функция создаёт хеш-таблицу
Вход:       size - размер таблицы

Выход:      -
Возврат:    указатель на структуру типа HashTable * - хеш-таблица

-----------------------------------------------------------------------------------------------*/

HashTable * create_table(unsigned int size) {
    // Creates a new HashTable
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item**)calloc(table->size, sizeof(Ht_item*));
    for(int i = 0; i < table->size; i++) {
        table->items[i] = NULL;
    }
    return table;
}

/*----------------------------------------------------------------------------------------------

Функция:    free_item
Назначение: Функция "Освободить память под пару ключ-значение"
Описание:   Функция освобождает память под пару ключ-значение
Вход:       item - пара ключ-значение

Выход:      -
Возврат:    -

-----------------------------------------------------------------------------------------------*/

void free_item(Ht_item * item) {
    free(item->key);
    free(item);
}

/*----------------------------------------------------------------------------------------------

Функция:    free_hashtable
Назначение: Функция "Освободить память под хеш-таблицу"
Описание:   Функция освобождает выделенную память под хеш-таблицу
Вход:       table - пара ключ-значение

Выход:      -
Возврат:    -

-----------------------------------------------------------------------------------------------*/

void free_hashtable(HashTable * table) {

    for(int i = 0; i < table->size; i++) {
        Ht_item * item = table->items[i];
        if(item != NULL) {free(item);}
    }

    free(table->items);
    free(table);
}

/*----------------------------------------------------------------------------------------------

Функция:    handle_collision
Назначение: Функция "Разрешить коллизию"
Описание:   Функция разрешает коллизию методом линейного пробирования
Вход:       table - пара ключ-значение
            index - индекс в хеш-таблице, посчитанный хеш-функцией
            item  - пара ключ-значение

Выход:      -
Возврат:    -

-----------------------------------------------------------------------------------------------*/

void handle_collision(HashTable * table, unsigned long index, Ht_item * item) {

    for(index = 0; index < table->size; index++) {
        if(table->items[index] == NULL) {break;}
    }

    if(index == table->size-1) {
        printf("Insert Error: Hash Table is full\n");
        return;
    }

    table->items[index] = item;
    table->count++;
}

/*----------------------------------------------------------------------------------------------

Функция:    ht_insert
Назначение: Функция "Разрешить коллизию"
Описание:   Функция разрешает коллизию методом линейного пробирования
Вход:       table - хеш-таблица
            key   - ключ, слово из файла
            value - значение, частота встречаемости слова в файле

Выход:      -
Возврат:    -

-----------------------------------------------------------------------------------------------*/

void ht_insert(HashTable * table, unsigned char * key, unsigned int value) {

    Ht_item * item = create_item(key, value);

    unsigned long index = hash_function(key);

    Ht_item * current_item = table->items[index];

    if(current_item == NULL) {
        // Ключа не существует
        if(table->count == table->size) {

            printf("Insert Error: Hash Table is full\n");

            free_item(item);
            return;
        }

        table->items[index] = item;
        table->count++;
    }
    else {
        if(memcmp(current_item->key, item->key, current_item->size_key+1) == 0) {
            table->items[index]->value++; // inc frequency
            free_item(item);
            return;
        }
        else {
            handle_collision(table, index, item);
            return;
        }
    }
}

/*----------------------------------------------------------------------------------------------

Функция:    ht_serch
Назначение: Функция "Поиск в таблице"
Описание:   Функция разрешает коллизию методом линейного пробирования
Вход:       table - пара ключ-значение
            index - индекс в хеш-таблице, посчитанный хеш-функцией
            item  - пара ключ-значение

Выход:      -
Возврат:    -

-----------------------------------------------------------------------------------------------*/

int ht_serch(HashTable * table, unsigned char * key) {

    int index_search = 0;

    for(; index_search < table->size; index_search++) {
        if(!memcmp(table->items[index_search]->key, key, table->items[index_search]->size_key)) {
                return table->items[index_search]->value;
        }
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

/*----------------------------------------------------------------------------------------------

Функция:    count_start_if
Назначение: Функция "Счёт начинается, если"
Описание:   Функция проверяет, начинать ли счёт символов в зависимости от функции pred
Вход:       str - массив символов типа unsigned char
            pred - функция типа int

Выход:      -
Возврат:    количество подсчитанных символов, удовлетворяющих условию

-----------------------------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------------------------

Функция:    count_words
Назначение: Функция "Подсчёт слов"
Описание:   Функция считает слова в массиве unsigned char
Вход:       str - массив символов типа unsigned char

Выход:      -
Возврат:    количество подсчитанных слов в строке типа unsigned char

-----------------------------------------------------------------------------------------------*/

unsigned long count_words(unsigned char* str) {

    unsigned long count = 0;
    int i = 0;
    while(1) {
        int spaces_or_other_symbols_count = count_start_if(&str[i], is_it_space_or_other_symbols);
        i += spaces_or_other_symbols_count;
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


int main(int argc, char* argv[]) {
    struct stat file_t;         // структура для считывания параметров файла с помощью функции stat
    FILE * file;                // сюда будет считан файл

    unsigned char * file_in;    // массив, в который будет записан файл

    setlocale(LC_ALL, "");
    printf("Введите путь к файлу: ");

    //scanf("%s", argv[1]);          // вводим путь к файлу
    if(argc > 1) { scanf("%s", argv[1]);}
    else {printf("Not arguments");}
    file = fopen(argv[1], "r");

    if(file == NULL) {perror("Error"); exit(0);}    // проверка открытия файла
    stat(argv[1], &file_t);        // в экземпляр структуры file_t записываем данные о файле по введённому пути

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
    //print_search(ht, "до");
    free_hashtable(ht);
    fclose(file);
    free(file_in);
}
