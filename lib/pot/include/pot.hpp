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
};
