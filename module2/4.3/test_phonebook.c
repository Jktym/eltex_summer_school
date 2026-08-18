#include "unity.h"
#include "phonebook.h"
#include <stdlib.h>
#include <string.h>

// Глобальный указатель на тестовую книгу
Phonebook *pb;

// Вызывается ПЕРЕД каждым тестом
void setUp(void) {
    pb = malloc(sizeof(Phonebook));
    pb->head = NULL;
    pb->count = 0;
}

// Вызывается ПОСЛЕ каждого теста
void tearDown(void) {
    free_phonebook(pb);
    free(pb);
}

// 1. Тест заполнения полей
void test_fill_contact_field(void) {
    Data d = {0};
    
    fill_contact_field(&d, "Иван", name_field);
    fill_contact_field(&d, "Иванов", surname_field);
    fill_contact_field(&d, "+79991234567", mob_phone_field);
    
    TEST_ASSERT_EQUAL_STRING("Иван", d.name);
    TEST_ASSERT_EQUAL_STRING("Иванов", d.surname);
    TEST_ASSERT_EQUAL_STRING("+79991234567", d.mob_phone);
}

// 2. Тест добавления и алфавитной сортировки
void test_add_contact_sorting(void) {
    Data d1 = {0}; strcpy(d1.surname, "Яковлев"); strcpy(d1.name, "Ян");
    Data d2 = {0}; strcpy(d2.surname, "Алексеев"); strcpy(d2.name, "Алексей");
    Data d3 = {0}; strcpy(d3.surname, "Иванов"); strcpy(d3.name, "Иван");

    pb = add_contact(pb, d1);
    pb = add_contact(pb, d2);
    pb = add_contact(pb, d3);

    TEST_ASSERT_EQUAL_INT(3, pb->count);

    // Проверяем, что элементы встали по алфавиту (Алексеев -> Иванов -> Яковлев)
    // И проверяем переиндексацию (ID должны быть 1, 2, 3)
    Contact *current = pb->head;
    
    TEST_ASSERT_EQUAL_STRING("Алексеев", current->data->surname);
    TEST_ASSERT_EQUAL_INT(1, current->data->id);
    
    current = current->next;
    TEST_ASSERT_EQUAL_STRING("Иванов", current->data->surname);
    TEST_ASSERT_EQUAL_INT(2, current->data->id);
    
    current = current->next;
    TEST_ASSERT_EQUAL_STRING("Яковлев", current->data->surname);
    TEST_ASSERT_EQUAL_INT(3, current->data->id);
}

// 3. Тест удаления контакта и сдвига ID
void test_delete_contact(void) {
    Data d1 = {0}; strcpy(d1.surname, "Алексеев");
    Data d2 = {0}; strcpy(d2.surname, "Иванов");
    Data d3 = {0}; strcpy(d3.surname, "Яковлев");

    pb = add_contact(pb, d1);
    pb = add_contact(pb, d2);
    pb = add_contact(pb, d3);

    // Удаляем "Иванова" (ID = 2)
    pb = delete_contact(pb, 2);

    TEST_ASSERT_EQUAL_INT(2, pb->count);

    // Теперь "Яковлев" должен стать ID = 2
    Contact *current = pb->head->next; // Второй элемент
    TEST_ASSERT_EQUAL_STRING("Яковлев", current->data->surname);
    TEST_ASSERT_EQUAL_INT(2, current->data->id);
}

// 4. Тест удаления единственного контакта (очистка списка)
void test_delete_single_contact(void) {
    Data d1 = {0}; strcpy(d1.surname, "Алексеев");
    pb = add_contact(pb, d1);
    
    pb = delete_contact(pb, 1);
    
    TEST_ASSERT_NULL(pb->head);
    TEST_ASSERT_EQUAL_INT(0, pb->count);
}

// 5. Тест получения контакта по ID
void test_get_contact(void) {
    Data d1 = {0}; strcpy(d1.surname, "Сидоров");
    pb = add_contact(pb, d1);
    
    Contact *found = get_contact(pb, 1);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Сидоров", found->data->surname);
    
    Contact *not_found = get_contact(pb, 99);
    TEST_ASSERT_NULL(not_found);
}

// 6. Тест поиска по подстроке
void test_find_contact_with_field(void) {
    Data d1 = {0}; strcpy(d1.surname, "Петров"); strcpy(d1.work, "Google");
    Data d2 = {0}; strcpy(d2.surname, "Иванов"); strcpy(d2.work, "Yandex");
    Data d3 = {0}; strcpy(d3.surname, "Сидоров"); strcpy(d3.work, "Google");

    pb = add_contact(pb, d1);
    pb = add_contact(pb, d2);
    pb = add_contact(pb, d3);

    Contact *results[10];
    
    // Ищем всех, кто работает в "Google"
    int found = find_contact_with_field(pb, results, "Google");
    
    TEST_ASSERT_EQUAL_INT(2, found);
    
    // Ищем по части фамилии
    found = find_contact_with_field(pb, results, "ов");
    TEST_ASSERT_EQUAL_INT(3, found); // Все три фамилии оканчиваются на "ов"
}

// Точка входа для тестов
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_fill_contact_field);
    RUN_TEST(test_add_contact_sorting);
    RUN_TEST(test_delete_contact);
    RUN_TEST(test_delete_single_contact);
    RUN_TEST(test_get_contact);
    RUN_TEST(test_find_contact_with_field);
    
    return UNITY_END();
}