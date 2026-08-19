#include "unity.h"
#include "queue.h"
#include <stdlib.h>

Queue *q = NULL;

// Эта функция вызывается ПЕРЕД каждым тестом
void setUp(void) {
    q = init_queue();
}

// Эта функция вызывается ПОСЛЕ каждого теста
void tearDown(void) {
    free_queue(q);
    free(q); // Освобождаем саму структуру очереди (см. примечание ниже!)
}

// 1. Тест инициализации очереди
void test_init_queue(void) {
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_NULL(q->head);
    TEST_ASSERT_NULL(q->tail);
    TEST_ASSERT_EQUAL_INT(0, q->count);
}

// 2. Тест добавления элементов
void test_add_node(void) {
    add_node(q, "msg1", 10);
    TEST_ASSERT_EQUAL_INT(1, q->count);
    TEST_ASSERT_NOT_NULL(q->head);
    TEST_ASSERT_EQUAL_STRING("msg1", q->head->message);
    TEST_ASSERT_EQUAL_INT(10, q->head->priority);

    add_node(q, "msg2", 20);
    TEST_ASSERT_EQUAL_INT(2, q->count);
    TEST_ASSERT_EQUAL_STRING("msg2", q->tail->message);
}

// 3. Тест обычного извлечения (первый элемент)
void test_pop_without_priority(void) {
    add_node(q, "msg1", 10);
    add_node(q, "msg2", 20);

    Node *popped = pop(q, WITHOUT_PRIORITY);
    
    TEST_ASSERT_NOT_NULL(popped);
    TEST_ASSERT_EQUAL_STRING("msg1", popped->message);
    TEST_ASSERT_EQUAL_INT(1, q->count);
    TEST_ASSERT_EQUAL_STRING("msg2", q->head->message); // Второй элемент стал первым
    
    free(popped);
}

// 4. Тест извлечения по точному приоритету (из середины)
void test_pop_with_exact_priority(void) {
    add_node(q, "msg1", 10);
    add_node(q, "msg2", 20); // Будем извлекать его
    add_node(q, "msg3", 30);

    Node *popped = pop(q, WITH_PRIORITY, 20);
    
    TEST_ASSERT_NOT_NULL(popped);
    TEST_ASSERT_EQUAL_STRING("msg2", popped->message);
    TEST_ASSERT_EQUAL_INT(2, q->count);
    
    // Проверяем, что список корректно "склеился"
    TEST_ASSERT_EQUAL_STRING("msg1", q->head->message);
    TEST_ASSERT_EQUAL_STRING("msg3", q->head->next->message);
    
    free(popped);
}

// 5. Тест извлечения с приоритетом не ниже заданного
void test_pop_not_lower_priority(void) {
    add_node(q, "msg1", 10);
    add_node(q, "msg2", 20);
    add_node(q, "msg3", 50); // Будем извлекать его (>= 40)
    add_node(q, "msg4", 15);

    Node *popped = pop(q, NOT_LOWER_PRIORITY, 40);
    
    TEST_ASSERT_NOT_NULL(popped);
    TEST_ASSERT_EQUAL_STRING("msg3", popped->message);
    TEST_ASSERT_EQUAL_INT(3, q->count);
    
    free(popped);
}

// 6. Тест извлечения из пустой очереди
void test_pop_empty_queue(void) {
    Node *popped = pop(q, WITHOUT_PRIORITY);
    TEST_ASSERT_NULL(popped);
    
    popped = pop(q, WITH_PRIORITY, 10);
    TEST_ASSERT_NULL(popped);
}

// Главная функция запуска тестов
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_init_queue);
    RUN_TEST(test_add_node);
    RUN_TEST(test_pop_without_priority);
    RUN_TEST(test_pop_with_exact_priority);
    RUN_TEST(test_pop_not_lower_priority);
    RUN_TEST(test_pop_empty_queue);
    
    return UNITY_END();
}