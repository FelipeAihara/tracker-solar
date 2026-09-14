/*
 * Testes da classe Potenciometro — apenas a conversão pura mV -> graus
 * (Potenciometro::mvParaAngulo), que não depende de um ADC real.
 */

#include <zephyr/ztest.h>
#include <pot.hpp>

ZTEST(pot, test_extremos)
{
    zassert_equal(Potenciometro::mvParaAngulo(0, 3894), 0,
                  "0 mV deveria corresponder a 0 grau");
    zassert_equal(Potenciometro::mvParaAngulo(3894, 3894), 180,
                  "vref inteiro deveria corresponder a 180 graus");
}

ZTEST(pot, test_meio_curso)
{
    /* Metade da tensão de referência ~ metade do curso (90°). */
    zassert_within(Potenciometro::mvParaAngulo(1947, 3894), 90, 1,
                    "metade do curso deveria ser ~90 graus");
}

ZTEST(pot, test_usa_vref_configurado_e_nao_valor_fixo)
{
    /*
     * Regressão: a conversão deve escalar pelo vref realmente configurado
     * (ex.: 3894 mV neste projeto), não por uma constante fixa como 3300.
     * Com vref=3894, um valor de 3300 mV NÃO deveria já ser 180°.
     */
    int angulo = Potenciometro::mvParaAngulo(3300, 3894);
    zassert_true(angulo < 180, "3300 mV com vref=3894 não deveria saturar em 180 graus");
    zassert_within(angulo, 152, 2, "angulo inesperado: %d", angulo);
}

ZTEST(pot, test_satura_nos_limites)
{
    /* Leitura acima do vref (ruído/tolerância) não deve estourar 180°. */
    zassert_equal(Potenciometro::mvParaAngulo(5000, 3894), 180);
}

ZTEST(pot, test_vref_zero_nao_gera_divisao_por_zero)
{
    zassert_equal(Potenciometro::mvParaAngulo(1000, 0), 0);
}

ZTEST_SUITE(pot, nullptr, nullptr, nullptr, nullptr, nullptr);
