#include <string.h>
#include "calc.h"
#include "menu.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void simple_test_add(void) {
    float result = add(100, 200);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 300, result);
}

void simple_test_sub(void) {
    float result = sub(150, 50);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 100, result);
}

void simple_test_mul(void) {
    float result = mul(10, 15);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 150, result);
}

void simple_test_div(void) {
    float result = div(50, 5);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 10, result);
}

void negative_test_add(void) {
    float result = add(-100, 50);
    TEST_ASSERT_FLOAT_WITHIN(0.001, -50, result);
}

void negative_test_add_2(void) {
    float result = add(-100, -50);
    TEST_ASSERT_FLOAT_WITHIN(0.001, -150, result);
}

void negative_test_sub(void) {
    float result = sub(-100, 50);
    TEST_ASSERT_FLOAT_WITHIN(0.001, -150, result);
}

void negative_test_sub_2(void) {
    float result = sub(-100, -50);
    TEST_ASSERT_FLOAT_WITHIN(0.001, -50, result);
}

void negative_test_mul(void) {
    float result = mul(-100, 5);
    TEST_ASSERT_FLOAT_WITHIN(0.001, -500, result);
}

void negative_test_mul_2(void) {
    float result = mul(-100, -5);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 500, result);
}

void negative_test_div(void) {
    float result = div(-500, 5);
    TEST_ASSERT_FLOAT_WITHIN(0.001, -100, result);
}

void negative_test_div_2(void) {
    float result = div(-500, -5);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 100, result);
}

void test_zero_mul(void) {
    float result = mul(1000, 0);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0, result);
}

void test_zero_div(void) {
    FILE *fake_stdin = tmpfile();
    fputs("4\n100\n0\n0\n", fake_stdin);
    rewind(fake_stdin);

    FILE *fake_stdout = tmpfile();
    
    FILE *old_stdin = stdin;
    FILE *old_stdout = stdout;
    stdin = fake_stdin;
    stdout = fake_stdout;       
    menu();

    fflush(fake_stdout);
    stdin = old_stdin;
    stdout = old_stdout;
    
    rewind(fake_stdout);
    char output_buffer[4096] = {0};
    size_t bytes_read = fread(output_buffer, 1, sizeof(output_buffer) - 1, fake_stdout);
    output_buffer[bytes_read] = '\0';

    fclose(fake_stdin);
    fclose(fake_stdout);

    TEST_ASSERT_TRUE_MESSAGE(
        strstr(output_buffer, "На 0 делить нельзя!!") != NULL,
        "Ожидалось сообщение об ошибке.\n"
    );
}

void test_unexpected_sym() {
    FILE *fake_stdin = tmpfile();
    fputs("dsad\n0\n", fake_stdin);
    rewind(fake_stdin);

    FILE *fake_stdout = tmpfile();

    FILE *old_stdin = stdin;
    FILE *old_stdout = stdout;
    stdin = fake_stdin;
    stdout = fake_stdout;
    menu();
    fflush(fake_stdout);
    stdin = old_stdin;
    stdout = old_stdout;

    rewind(fake_stdout);
    char output_buffer [4096] = {0};
    size_t bytes_read = fread(output_buffer, 1, sizeof(output_buffer) - 1, fake_stdout);
    output_buffer[bytes_read] = '\0';

    fclose(fake_stdin);
    fclose(fake_stdout);

    TEST_ASSERT_TRUE_MESSAGE(
        strstr(output_buffer, "Нет такого пункта! Попробуйте ещё раз.") != NULL, 
        "Ожидало сообщение об ошибке.\n"
    );
}

void test_ptr_func_add(void) {
    float (*ptr_func) (float, float);
    ptr_func = select(1);
    TEST_ASSERT_EQUAL_PTR(add, ptr_func);
}

void test_ptr_func_sub(void) {
    float (*ptr_func) (float, float);
    ptr_func = select(2);
    TEST_ASSERT_EQUAL_PTR(sub, ptr_func);
}

void test_ptr_func_mul(void) {
    float (*ptr_func) (float, float);
    ptr_func = select(3);
    TEST_ASSERT_EQUAL_PTR(mul, ptr_func);
}

void test_ptr_func_div(void) {
    float (*ptr_func) (float, float);
    ptr_func = select(4);
    TEST_ASSERT_EQUAL_PTR(div, ptr_func);
}

void test_oper_print(void) {
    TEST_ASSERT_EQUAL_INT('+', print_oper(1));
    TEST_ASSERT_EQUAL_INT('-', print_oper(2));
    TEST_ASSERT_EQUAL_INT('*', print_oper(3));
    TEST_ASSERT_EQUAL_INT('/', print_oper(4));

}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(simple_test_add);
    RUN_TEST(simple_test_sub);
    RUN_TEST(simple_test_mul);
    RUN_TEST(simple_test_div);
    RUN_TEST(negative_test_add);
    RUN_TEST(negative_test_add_2);
    RUN_TEST(negative_test_sub);
    RUN_TEST(negative_test_sub_2);
    RUN_TEST(negative_test_mul);
    RUN_TEST(negative_test_mul_2);
    RUN_TEST(negative_test_div);
    RUN_TEST(negative_test_div_2);
    RUN_TEST(test_zero_mul);
    RUN_TEST(test_zero_div);
    RUN_TEST(test_unexpected_sym);
    RUN_TEST(test_ptr_func_add);
    RUN_TEST(test_ptr_func_sub);
    RUN_TEST(test_ptr_func_mul);
    RUN_TEST(test_ptr_func_div);
    RUN_TEST(test_oper_print);
    return UNITY_END();
}