/*
 * Testes da classe Button — apenas a transição pura de modo
 * (Button::proximoModo), que não depende de GPIO real.
 */

#include <zephyr/ztest.h>
#include <button.hpp>

ZTEST(button, test_ciclo_completo)
{
    zassert_equal(Button::proximoModo(MODO_AUTOMATICO), MODO_MANUAL_1G,
                  "AUTOMATICO deveria ir para MANUAL_1G");
    zassert_equal(Button::proximoModo(MODO_MANUAL_1G), MODO_MANUAL_2B,
                  "MANUAL_1G deveria ir para MANUAL_2B");
    zassert_equal(Button::proximoModo(MODO_MANUAL_2B), MODO_AUTOMATICO,
                  "MANUAL_2B deveria voltar para AUTOMATICO");
}

ZTEST(button, test_ciclo_fecha_apos_tres_cliques)
{
    Modo m = MODO_AUTOMATICO;
    m = Button::proximoModo(m);
    m = Button::proximoModo(m);
    m = Button::proximoModo(m);

    zassert_equal(m, MODO_AUTOMATICO, "após 3 cliques deveria voltar ao modo inicial");
}

ZTEST_SUITE(button, nullptr, nullptr, nullptr, nullptr, nullptr);
