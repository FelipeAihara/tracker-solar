#include "pot.hpp"

Potenciometro::Potenciometro(const struct device *adc, adc_channel_cfg adc_ch, uint32_t vref, uint8_t resolution) :
    adc_(adc), adc_ch_(adc_ch), vref_mv(vref)
{
    seq.channels = BIT(adc_ch_.channel_id);
    seq.buffer = &buf;
    seq.buffer_size = sizeof(buf);
    seq.resolution = resolution;
    seq.options = NULL;
    seq.calibrate = false;

    if (!device_is_ready(adc_)) {
        printk("ADC peripheral is not ready\r\n");
    }

    int ret = adc_channel_setup(adc_, &adc_ch_);
    if (ret < 0) {
        printk("Could not set up ADC %d\r\n", adc_ch_.channel_id);
    }
}

int Potenciometro::read()
{
    if (adc_ == NULL || !device_is_ready(adc_)) {
        return -1;
    }

    int ret = adc_read(adc_, &seq);
    if (ret < 0) {
        printk("Could not read ADC: %d\r\n", ret);
        return -1;
    }

    val_mv = ((uint32_t)buf * vref_mv) / (1 << seq.resolution);
    return mvParaAngulo(val_mv, vref_mv);
}

int Potenciometro::mvParaAngulo(uint32_t val_mv, uint32_t vref_mv)
{
    if (vref_mv == 0) {
        return 0;
    }

    int angulo = static_cast<int>((val_mv * 180U) / 2800); // Número mágico de 2800 vem do fato de que
                                                           // o ADC do ESP32S3 é muito impreciso e 
                                                           // satura em 2800 (não vref_mv)
    if (angulo < 0)   angulo = 0;
    if (angulo > 180) angulo = 180;
    return angulo;
}
