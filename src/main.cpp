// #include <servo.hpp>
// #include <rtc.hpp>
// #include <tracker.hpp>
// #include <button.hpp>
#include <pot.hpp>

// const struct device *i2c = DEVICE_DT_GET(DT_NODELABEL(i2c0));
// static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_NODELABEL(user_button_0), gpios);
static const struct device *adc = DEVICE_DT_GET(DT_ALIAS(my_adc));
static const struct adc_channel_cfg adc_ch = ADC_CHANNEL_CFG_DT(DT_ALIAS(my_adc_channel));

int main(void)
{
    int theta;
    Servo myServoGamma(TPM1, 0, GPIOB, 0);
    Servo myServoBeta(TPM1, 1, GPIOB, 1);
    Potenciometro myPot(adc, adc_ch);
    MyRTC myTimer(i2c);
    Tracker tracker(-23.5570, -46.7290);
    Button myButton(btn);
    struct tempo agora;

    //struct tempo set = { .sec=0, .min=15, .hour=11,
    //                            .wday=4, .mday=2, .month=7, .year=26 };
    //myTimer.write(&set);

    while(1)
    {
        switch(myButton.modo_atual_)
        {
            case MODO_AUTOMATICO:
                myTimer.read();
                agora = myTimer.getTempo();
                tracker.atualizar(agora);
                myServoGamma.write(tracker.getGamma());
                if (tracker.getBeta() < 45) theta = 45;
                if (tracker.getBeta() > 135) theta = 135;
                myServoBeta.write(theta);
                k_msleep(1000);
                break;

            case MODO_MANUAL_1G:
                theta = myPot.read();
                myServoGamma.write(theta);
                break;

            case MODO_MANUAL_2B:
                theta = myPot.read();
                if (theta < 45) theta = 45;
                if (theta > 135) theta = 135;
                myServoBeta.write(theta);
                break;
        }
    }
}