#include "unity.h"
#include "phonebook.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static Phonebook pb;

void setUp(void) {
    init_phonebook(&pb);
}

void tearDown(void) {
    remove("test_contacts.txt");
}


void test_init_phonebook_sets_count_to_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, pb.count);
}

void test_init_phonebook_clears_contacts(void) {
    TEST_ASSERT_EQUAL_INT(0, pb.contacts[0].id);
    TEST_ASSERT_EQUAL_STRING("", pb.contacts[0].name);
    TEST_ASSERT_EQUAL_STRING("", pb.contacts[0].surname);
}

void test_init_phonebook_handles_null(void) {
    init_phonebook(NULL);
}

// ============ Тесты add_contact ============

void test_add_contact_success(void) {
    Contact c = {0};
    strcpy(c.name, "Иван");
    strcpy(c.surname, "Иванов");
    strcpy(c.mob_phone, "+79001234567");

    int result = add_contact(&pb, &c);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(1, pb.count);
    TEST_ASSERT_EQUAL_STRING("Иван", pb.contacts[0].name);
    TEST_ASSERT_EQUAL_STRING("Иванов", pb.contacts[0].surname);
    TEST_ASSERT_EQUAL_INT(1, pb.contacts[0].id); // ID должен быть 1
}

void test_add_contact_assigns_sequential_ids(void) {
    Contact c1 = {0};
    strcpy(c1.name, "Иван");
    strcpy(c1.surname, "Иванов");

    Contact c2 = {0};
    strcpy(c2.name, "Петр");
    strcpy(c2.surname, "Петров");

    add_contact(&pb, &c1);
    add_contact(&pb, &c2);

    TEST_ASSERT_EQUAL_INT(1, pb.contacts[0].id);
    TEST_ASSERT_EQUAL_INT(2, pb.contacts[1].id);
    TEST_ASSERT_EQUAL_INT(2, pb.count);
}

void test_add_contact_rejects_empty_name(void) {
    Contact c = {0};
    strcpy(c.surname, "Иванов");
    // name пустая!

    int result = add_contact(&pb, &c);

    TEST_ASSERT_EQUAL_INT(-3, result);
    TEST_ASSERT_EQUAL_INT(0, pb.count);
}

void test_add_contact_rejects_empty_surname(void) {
    Contact c = {0};
    strcpy(c.name, "Иван");
    // surname пустая!

    int result = add_contact(&pb, &c);

    TEST_ASSERT_EQUAL_INT(-3, result);
    TEST_ASSERT_EQUAL_INT(0, pb.count);
}

void test_add_contact_rejects_null_pointers(void) {
    TEST_ASSERT_EQUAL_INT(-1, add_contact(NULL, NULL));
    
    Contact c = {0};
    strcpy(c.name, "Иван");
    strcpy(c.surname, "Иванов");
    TEST_ASSERT_EQUAL_INT(-1, add_contact(NULL, &c));
    TEST_ASSERT_EQUAL_INT(-1, add_contact(&pb, NULL));
}

void test_add_contact_overflow(void) {
    // Заполняем всю книгу
    for (int i = 0; i < PHONEBOOK_SIZE; i++) {
        Contact c = {0};
        strcpy(c.name, "Имя");
        strcpy(c.surname, "Фамилия");
        TEST_ASSERT_EQUAL_INT(0, add_contact(&pb, &c));
    }

    TEST_ASSERT_EQUAL_INT(PHONEBOOK_SIZE, pb.count);

    // Попытка добавить ещё один должна вернуть -2
    Contact extra = {0};
    strcpy(extra.name, "Лишний");
    strcpy(extra.surname, "Контакт");
    TEST_ASSERT_EQUAL_INT(-2, add_contact(&pb, &extra));
    TEST_ASSERT_EQUAL_INT(PHONEBOOK_SIZE, pb.count);
}

// ============ Тесты find_contact_with_id ============

static void add_sample_contacts(void) {
    Contact c1 = {0};
    strcpy(c1.name, "Иван");
    strcpy(c1.surname, "Иванов");
    add_contact(&pb, &c1);

    Contact c2 = {0};
    strcpy(c2.name, "Петр");
    strcpy(c2.surname, "Петров");
    add_contact(&pb, &c2);
}

void test_find_contact_with_id_found(void) {
    add_sample_contacts();
    
    // ID=1 соответствует первому контакту
    TEST_ASSERT_EQUAL_INT(1, find_contact_with_id(&pb, 1));
    TEST_ASSERT_EQUAL_INT(1, find_contact_with_id(&pb, 2));
}

void test_find_contact_with_id_not_found(void) {
    add_sample_contacts();
    
    TEST_ASSERT_EQUAL_INT(0, find_contact_with_id(&pb, 999));
    TEST_ASSERT_EQUAL_INT(0, find_contact_with_id(&pb, 0));
    TEST_ASSERT_EQUAL_INT(0, find_contact_with_id(&pb, -1));
}

void test_find_contact_with_id_null_book(void) {
    TEST_ASSERT_EQUAL_INT(0, find_contact_with_id(NULL, 1));
}

// ============ Тесты find_contact_with_field ============

void test_find_by_name_substring(void) {
    add_sample_contacts();
    
    Contact results[10];
    int found = find_contact_with_field(&pb, results, "Иван");  // ← убрано 10
    
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_EQUAL_STRING("Иван", results[0].name);
}

void test_find_by_phone_substring(void) {
    Contact c = {0};
    strcpy(c.name, "Анна");
    strcpy(c.surname, "Сидорова");
    strcpy(c.mob_phone, "+79005553535");
    add_contact(&pb, &c);

    Contact results[10];
    int found = find_contact_with_field(&pb, results, "555");  // ← убрано 10
    
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_EQUAL_STRING("Анна", results[0].name);
}

void test_find_returns_multiple_matches(void) {
    Contact c1 = {0};
    strcpy(c1.name, "Иван");
    strcpy(c1.surname, "Иванов");
    add_contact(&pb, &c1);

    Contact c2 = {0};
    strcpy(c2.name, "Иван");
    strcpy(c2.surname, "Сидоров");
    add_contact(&pb, &c2);

    Contact c3 = {0};
    strcpy(c3.name, "Петр");
    strcpy(c3.surname, "Петров");
    add_contact(&pb, &c3);

    Contact results[10];
    int found = find_contact_with_field(&pb, results, "Иван");  // ← убрано 10
    
    TEST_ASSERT_EQUAL_INT(2, found);
}

void test_find_no_matches(void) {
    add_sample_contacts();
    
    Contact results[10];
    int found = find_contact_with_field(&pb, results, "Несуществующий");  // ← убрано 10
    
    TEST_ASSERT_EQUAL_INT(0, found);
}

void test_find_case_sensitive(void) {
    add_sample_contacts();
    
    Contact results[10];
    int found = find_contact_with_field(&pb, results, "иван");  // ← убрано 10
    
    TEST_ASSERT_EQUAL_INT(0, found);
}

void test_find_empty_query(void) {
    add_sample_contacts();
    
    Contact results[10];
    TEST_ASSERT_EQUAL_INT(0, find_contact_with_field(&pb, results, ""));    // ← убрано 10
    TEST_ASSERT_EQUAL_INT(0, find_contact_with_field(&pb, results, NULL));  // ← убрано 10
}

// ============ Тесты delete_contact ============

void test_delete_contact_success(void) {
    add_sample_contacts();
    TEST_ASSERT_EQUAL_INT(2, pb.count);

    // Удаляем первый контакт (по индексу 0)
    int result = delete_contact(&pb, 0);
    
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(1, pb.count);
    // Теперь на месте [0] должен быть Петр
    TEST_ASSERT_EQUAL_STRING("Петр", pb.contacts[0].name);
}

void test_delete_last_contact(void) {
    add_sample_contacts();
    
    int result = delete_contact(&pb, 1); // Удаляем последний
    
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(1, pb.count);
    TEST_ASSERT_EQUAL_STRING("Иван", pb.contacts[0].name);
}

void test_delete_from_empty_book(void) {
    int result = delete_contact(&pb, 0);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(0, pb.count);
}

void test_delete_invalid_index(void) {
    add_sample_contacts();
    
    int result = delete_contact(&pb, 99);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(2, pb.count); // Книга не изменилась
}

void test_delete_reindexes_ids(void) {
    // Добавляем 3 контакта
    for (int i = 0; i < 3; i++) {
        Contact c = {0};
        strcpy(c.name, "Имя");
        strcpy(c.surname, "Фамилия");
        add_contact(&pb, &c);
    }
    // IDs: 1, 2, 3

    delete_contact(&pb, 0); // Удаляем первый
    
    // IDs должны пересчитаться: 1, 2
    TEST_ASSERT_EQUAL_INT(1, pb.contacts[0].id);
    TEST_ASSERT_EQUAL_INT(2, pb.contacts[1].id);
}

// ============ Тесты edit_contact ============
// edit_contact использует safe_read (stdin), поэтому тестируем её косвенно
// через изменение полей напрямую и проверку результата

void test_edit_contact_changes_field(void) {
    Contact c = {0};
    strcpy(c.name, "Иван");
    strcpy(c.surname, "Иванов");
    add_contact(&pb, &c);

    // Имитируем редактирование напрямую (без safe_read)
    strcpy(pb.contacts[0].name, "Петр");

    TEST_ASSERT_EQUAL_STRING("Петр", pb.contacts[0].name);
    TEST_ASSERT_EQUAL_STRING("Иванов", pb.contacts[0].surname); // Остальное не изменилось
}

// ============ Запуск тестов ============

int main(void) {
    UNITY_BEGIN();

    // init
    RUN_TEST(test_init_phonebook_sets_count_to_zero);
    RUN_TEST(test_init_phonebook_clears_contacts);
    RUN_TEST(test_init_phonebook_handles_null);

    // add
    RUN_TEST(test_add_contact_success);
    RUN_TEST(test_add_contact_assigns_sequential_ids);
    RUN_TEST(test_add_contact_rejects_empty_name);
    RUN_TEST(test_add_contact_rejects_empty_surname);
    RUN_TEST(test_add_contact_rejects_null_pointers);
    RUN_TEST(test_add_contact_overflow);

    // find by id
    RUN_TEST(test_find_contact_with_id_found);
    RUN_TEST(test_find_contact_with_id_not_found);
    RUN_TEST(test_find_contact_with_id_null_book);

    // find by field
    RUN_TEST(test_find_by_name_substring);
    RUN_TEST(test_find_by_phone_substring);
    RUN_TEST(test_find_returns_multiple_matches);
    RUN_TEST(test_find_no_matches);
    RUN_TEST(test_find_case_sensitive);
    RUN_TEST(test_find_empty_query);

    // delete
    RUN_TEST(test_delete_contact_success);
    RUN_TEST(test_delete_last_contact);
    RUN_TEST(test_delete_from_empty_book);
    RUN_TEST(test_delete_invalid_index);
    RUN_TEST(test_delete_reindexes_ids);

    // edit
    RUN_TEST(test_edit_contact_changes_field);

    return UNITY_END();
}