/*
 * Testes da classe Solarimetria.
 *
 * Os valores esperados de beta/gamma foram gerados reproduzindo, em
 * Python, exatamente os mesmos passos do algoritmo de posição solar
 * usado em solarimetria.cpp (fórmula de baixa precisão de Meeus), para
 * as mesmas datas/horas/coordenadas — funcionando como teste de
 * regressão do cálculo.
 */

#include <zephyr/ztest.h>
#include <solarimetria.hpp>

static struct tempo fazer_tempo(uint8_t year, uint8_t month, uint8_t mday,
                                 uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tempo t = {};
    t.year  = year;   // anos desde 2000 (ex.: 26 = 2026)
    t.month = month;
    t.mday  = mday;
    t.hour  = hour;
    t.min   = min;
    t.sec   = sec;
    return t;
}

/* Caso 1: São Paulo, meio-dia local, 15/jan/2026 (verão, sol ao Norte). */
ZTEST(solarimetria, test_sao_paulo_meio_dia)
{
    Solarimetria s(-23.5570, -46.7290); /* fuso -3h padrão */
    struct tempo t = fazer_tempo(26, 1, 15, 12, 0, 0);

    s.atualizar(t);

    zassert_true(s.solVisivel(), "Sol deveria estar visível ao meio-dia");
    zassert_equal(s.getBeta(), 85, "beta inesperado: %d", s.getBeta());
    zassert_equal(s.getGamma(), 2, "gamma inesperado: %d", s.getGamma());
}

/* Caso 2: São Paulo, meia-noite local — sol abaixo do horizonte. */
ZTEST(solarimetria, test_sao_paulo_meia_noite_repouso)
{
    Solarimetria s(-23.5570, -46.7290);
    struct tempo t = fazer_tempo(26, 1, 15, 0, 0, 0);

    s.atualizar(t);

    zassert_false(s.solVisivel(), "Sol não deveria estar visível à meia-noite");
    zassert_equal(s.getBeta(), Solarimetria::BETA_REPOUSO,
                  "beta deveria ir para a posição de repouso");
    zassert_equal(s.getGamma(), 149 - Solarimetria::GAMMA_REPOUSO,
                  "gamma deveria ir para a posição de repouso");
}

/* Caso 3: equador, meio-dia UTC, próximo do equinócio — sol perto do zênite. */
ZTEST(solarimetria, test_equador_equinocio_proximo_zenite)
{
    Solarimetria s(0.0, 0.0, 0.0); /* fuso 0 (UTC) */
    struct tempo t = fazer_tempo(26, 3, 20, 12, 0, 0);

    s.atualizar(t);

    zassert_true(s.solVisivel(), "Sol deveria estar visível ao meio-dia no equador");
    zassert_equal(s.getBeta(), 92, "beta inesperado: %d", s.getBeta());
    zassert_equal(s.getGamma(), 148, "gamma inesperado: %d", s.getGamma());
}

/* Beta e gamma nunca devem sair dos limites mecânicos dos servos. */
ZTEST(solarimetria, test_limites_mecanicos_respeitados)
{
    Solarimetria s(-23.5570, -46.7290);

    for (uint8_t hour = 0; hour < 24; hour++) {
        struct tempo t = fazer_tempo(26, 6, 10, hour, 0, 0);
        s.atualizar(t);

        zassert_true(s.getBeta() >= Solarimetria::BETA_MIN &&
                     s.getBeta() <= Solarimetria::BETA_MAX,
                     "beta fora da faixa mecânica na hora %d: %d", hour, s.getBeta());
    }
}

/* atualizar() deve ser determinístico: mesma entrada -> mesma saída. */
ZTEST(solarimetria, test_deterministico)
{
    Solarimetria s1(-23.5570, -46.7290);
    Solarimetria s2(-23.5570, -46.7290);
    struct tempo t = fazer_tempo(26, 1, 15, 12, 0, 0);

    s1.atualizar(t);
    s2.atualizar(t);

    zassert_equal(s1.getBeta(), s2.getBeta());
    zassert_equal(s1.getGamma(), s2.getGamma());
}

ZTEST_SUITE(solarimetria, nullptr, nullptr, nullptr, nullptr, nullptr);
