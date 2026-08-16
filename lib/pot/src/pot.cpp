/* 
Esse código não está muito bom. Problemas nas linhas 8 e 13, em que eu coloquei as propriedades
de um canal específico. Entretanto, como ambos os canais têm as mesmas propriedades, não muda
muito.
*/ 

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

    int theta;
    int ret = adc_read(adc_, &seq);
    if (ret < 0) {
        printk("Could not read ADC: %d\r\n", ret);
    }

    val_mv = ((uint32_t)buf * vref_mv )/ (1 << seq.resolution);
    theta = (val_mv * 180) / 3300;
    return theta;
}