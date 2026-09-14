/*
 * Testes da classe Servo — apenas a conversão pura ângulo -> pulso PWM
 * (Servo::calcularPulso), que não depende de um dispositivo PWM real.
 */

#include <zephyr/ztest.h>
#include <servo.hpp>

#define PULSO_MIN 1000000U /* 1 ms -> 0°   */
#define PULSO_MAX 2000000U /* 2 ms -> 180° */

ZTEST(servo, test_extremos)
{
    zassert_equal(Servo::calcularPulso(0, PULSO_MIN, PULSO_MAX), PULSO_MIN);
    zassert_equal(Servo::calcularPulso(180, PULSO_MIN, PULSO_MAX), PULSO_MAX);
}

ZTEST(servo, test_meio_curso)
{
    zassert_equal(Servo::calcularPulso(90, PULSO_MIN, PULSO_MAX), 1500000U);
}

ZTEST(servo, test_satura_abaixo_de_zero)
{
    zassert_equal(Servo::calcularPulso(-10, PULSO_MIN, PULSO_MAX), PULSO_MIN);
}

ZTEST(servo, test_satura_acima_de_180)
{
    zassert_equal(Servo::calcularPulso(200, PULSO_MIN, PULSO_MAX), PULSO_MAX);
}

ZTEST_SUITE(servo, nullptr, nullptr, nullptr, nullptr, nullptr);
