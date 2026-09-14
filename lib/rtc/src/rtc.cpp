#include "rtc.hpp"

MyRTC::MyRTC(const struct device *dev) {  // ← const aqui também
    i2c_dev_ = dev;
    if (!device_is_ready(i2c_dev_)) {
        printk("I2C peripheral is not ready\r\n");
    }
}

int MyRTC::read() {
    uint8_t reg = 0x00;
    uint8_t raw[7];
    int ret = i2c_write_read(i2c_dev_, DS3231_I2C_ADDR, &reg, 1, raw, sizeof(raw));
    if (ret) {
        return ret;
    }
    tempo_.sec   = BCD2BIN(raw[0] & 0x7F);
    tempo_.min   = BCD2BIN(raw[1] & 0x7F);
    tempo_.hour  = BCD2BIN(raw[2] & 0x3F);  
    tempo_.wday  = raw[3] & 0x07;
    tempo_.mday  = BCD2BIN(raw[4] & 0x3F);
    tempo_.month = BCD2BIN(raw[5] & 0x1F);
    tempo_.year  = BCD2BIN(raw[6]);
    return 0;
}

int MyRTC::write(tempo *t) {
    uint8_t buf[8] = {
        0x00,
        BIN2BCD(t->sec), BIN2BCD(t->min), BIN2BCD(t->hour),
        t->wday, BIN2BCD(t->mday), BIN2BCD(t->month), BIN2BCD(t->year)
    };
    return i2c_write(i2c_dev_, buf, sizeof(buf), DS3231_I2C_ADDR);
}

void MyRTC::printHorario()
{
    read();
    printk("%02u:%02u:%02u  %02u/%02u/20%02u\n",
        tempo_.hour, tempo_.min, tempo_.sec, tempo_.mday, tempo_.month, tempo_.year);
}