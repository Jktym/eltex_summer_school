#include "unity.h"
#include "phonebook.h"
#include <stdlib.h>
#include <string.h>

Phonebook *pb;

// Вызывается ПЕРЕД каждым тестом
void setUp(void) {
    pb = malloc(sizeof(Phonebook));
    pb->root = NULL;
    pb->count = 0;
}

// Вызывается ПОСЛЕ каждого теста
void tearDown(void) {
    free_phonebook(pb);
    free(pb);
}

// 1. Тест заполнения полей (логика не меняется)
void test_fill_contact_field(void) {
    Data d = {0};
    fill_contact_field(&d, "Иван", name_field);
    fill_contact_field(&d, "Иванов", surname_field);
    fill_contact_field(&d, "+79991234567", mob_phone_field);
    
    TEST_ASSERT_EQUAL_STRING("Иван", d.name);
    TEST_ASSERT_EQUAL_STRING("Иванов", d.surname);
    TEST_ASSERT_EQUAL_STRING("+79991234567", d.mob_phone);
}

// 2. Тест добавления и правильного формирования дерева (BST)
void test_add_contact_tree_structure(void) {
    // Важно: порядок добавления определяет форму дерева!
    Data d1 = {0}; strcpy(d1.surname, "Иванов");   // Станет корнем (root)
    Data d2 = {0}; strcpy(d2.surname, "Алексеев"); // Пойдет влево (А < И)
    Data d3 = {0}; strcpy(d3.surname, "Яковлев");  // Пойдет вправо (Я > И)

    pb = add_contact(pb, d1);
    pb = add_contact(pb, d2);
    pb = add_contact(pb, d3);

    TEST_ASSERT_EQUAL_INT(3, pb->count);
    
    // Проверяем корень дерева
    TEST_ASSERT_NOT_NULL(pb->root);
    TEST_ASSERT_EQUAL_STRING("Иванов", pb->root->data->surname);
    
    // Проверяем левое поддерево
    TEST_ASSERT_NOT_NULL(pb->root->left);
    TEST_ASSERT_EQUAL_STRING("Алексеев", pb->root->left->data->surname);
    
    // Проверяем правое поддерево
    TEST_ASSERT_NOT_NULL(pb->root->right);
    TEST_ASSERT_EQUAL_STRING("Яковлев", pb->root->right->data->surname);
}

// 3. Тест поиска контакта (дерево)
void test_find_contact_with_field(void) {
    Data d1 = {0}; strcpy(d1.surname, "Иванов"); strcpy(d1.work, "Yandex");
    Data d2 = {0}; strcpy(d2.surname, "Алексеев"); strcpy(d2.work, "Google");
    Data d3 = {0}; strcpy(d3.surname, "Яковлев"); strcpy(d3.work, "Google");

    pb = add_contact(pb, d1);
    pb = add_contact(pb, d2);
    pb = add_contact(pb, d3);

    Contact *results[10];
    
    // Ищем всех, кто работает в "Google"
    int found = find_contact_with_field(pb, results, "Google");
    TEST_ASSERT_EQUAL_INT(2, found);
    
    // Ищем по части фамилии (все оканчиваются на "ев" или "ов", но давай поищем "ов")
    found = find_contact_with_field(pb, results, "ов");
    TEST_ASSERT_EQUAL_INT(2, found); // Иванов и Яковлев
}

// 4. Тест получения контакта по ID
void test_get_contact(void) {
    Data d1 = {0}; strcpy(d1.surname, "Сидоров");
    pb = add_contact(pb, d1);
    
    // Получаем ID только что добавленного контакта
    int id = pb->root->data->id;
    
    Contact *found = get_contact(pb, id);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Сидоров", found->data->surname);
    
    Contact *not_found = get_contact(pb, 999);
    TEST_ASSERT_NULL(not_found);
}

// 5. Тест удаления контакта
void test_delete_contact_tree(void) {
    Data d1 = {0}; strcpy(d1.surname, "Иванов");
    Data d2 = {0}; strcpy(d2.surname, "Алексеев");
    
    pb = add_contact(pb, d1);
    pb = add_contact(pb, d2);
    
    // Запоминаем ID Алексеева (он в левом узле)
    int id_to_delete = pb->root->left->data->id;
    
    pb = delete_contact(pb, id_to_delete);
    
    TEST_ASSERT_EQUAL_INT(1, pb->count);
    TEST_ASSERT_NULL(pb->root->left); // Левый узел должен стать пустым
}

// 6. Тест очистки дерева
void test_free_phonebook_tree(void) {
    Data d1 = {0}; strcpy(d1.surname, "Иванов");
    Data d2 = {0}; strcpy(d2.surname, "Алексеев");
    Data d3 = {0}; strcpy(d3.surname, "Яковлев");
    
    pb = add_contact(pb, d1);
    pb = add_contact(pb, d2);
    pb = add_contact(pb, d3);
    
    free_phonebook(pb);
    
    TEST_ASSERT_NULL(pb->root);
    TEST_ASSERT_EQUAL_INT(0, pb->count);
}

// Точка входа для тестов
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_fill_contact_field);
    RUN_TEST(test_add_contact_tree_structure);
    RUN_TEST(test_find_contact_with_field);
    RUN_TEST(test_get_contact);
    RUN_TEST(test_delete_contact_tree);
    RUN_TEST(test_free_phonebook_tree);
    
    return UNITY_END();
}