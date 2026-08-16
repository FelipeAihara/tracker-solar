// #include <servo.hpp>
// #include <rtc.hpp>
// #include <tracker.hpp>
// #include <button.hpp>
#include <pot.hpp>

// const struct device *i2c = DEVICE_DT_GET(DT_NODELABEL(i2c0));
// static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_NODELABEL(user_button_0), gpios);
static const struct device *adc = DEVICE_DT_GET(DT_ALIAS(my_adc));

static const struct adc_channel_cfg adc_ch1 = ADC_CHANNEL_CFG_DT(DT_ALIAS(my_adc_channel1));
#define POT1_VREF DT_PROP(DT_ALIAS(my_adc_channel1), zephyr_vref_mv)
#define POT1_RES  DT_PROP(DT_ALIAS(my_adc_channel1), zephyr_resolution)

static const struct adc_channel_cfg adc_ch2 = ADC_CHANNEL_CFG_DT(DT_ALIAS(my_adc_channel2));
#define POT2_VREF DT_PROP(DT_ALIAS(my_adc_channel2), zephyr_vref_mv)
#define POT2_RES  DT_PROP(DT_ALIAS(my_adc_channel2), zephyr_resolution)

int main(void)
{
    // int theta;
    // Servo myServoGamma(TPM1, 0, GPIOB, 0);
    // Servo myServoBeta(TPM1, 1, GPIOB, 1);
    int theta1;
    int theta2;
    Potenciometro myPot1(adc, adc_ch1, POT1_VREF, POT1_RES);
    Potenciometro myPot2(adc, adc_ch2, POT2_VREF, POT2_RES);
    // MyRTC myTimer(i2c);
    // Tracker tracker(-23.5570, -46.7290);
    // Button myButton(btn);
    // struct tempo agora;

    //struct tempo set = { .sec=0, .min=15, .hour=11,
    //                            .wday=4, .mday=2, .month=7, .year=26 };
    //myTimer.write(&set);

    while(1)
    {
        theta1 = myPot1.read();
        theta2 = myPot2.read();
        printk("Theta 1: %d, theta 2: %d\n", theta1, theta2);
        k_msleep(1000);
        // switch(myButton.modo_atual_)
        // {
        //     case MODO_AUTOMATICO:
        //         myTimer.read();
        //         agora = myTimer.getTempo();
        //         tracker.atualizar(agora);
        //         myServoGamma.write(tracker.getGamma());
        //         if (tracker.getBeta() < 45) theta = 45;
        //         if (tracker.getBeta() > 135) theta = 135;
        //         myServoBeta.write(theta);
        //         k_msleep(1000);
        //         break;

        //     case MODO_MANUAL_1G:
        //         theta = myPot.read();
        //         myServoGamma.write(theta);
        //         break;

        //     case MODO_MANUAL_2B:
        //         theta = myPot.read();
        //         if (theta < 45) theta = 45;
        //         if (theta > 135) theta = 135;
        //         myServoBeta.write(theta);
        //         break;
        // }
    }
}