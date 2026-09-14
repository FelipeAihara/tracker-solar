#pragma once

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>

class Potenciometro
{
private:
    const struct device *adc_;
    const struct adc_channel_cfg adc_ch_;
    struct adc_sequence seq;
    uint16_t val_mv;
    uint16_t buf;
    uint32_t vref_mv;

public:
    Potenciometro(const struct device *adc, adc_channel_cfg adc_ch, uint32_t vref, uint8_t resolution);

    int read();

    /**
     * Converte uma leitura já em milivolts para um ângulo em graus [0,180],
     * proporcional à tensão de referência do canal. Extraído como método
     * estático puro (sem tocar hardware) para poder ser testado sem ADC real.
     */
    static int mvParaAngulo(uint32_t val_mv, uint32_t vref_mv);
};
