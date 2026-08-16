#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)

extern "C" 
{
    #include "pwm_z42.h"
    #include "MKL25Z4.h"
}

 
#define TPM_MODULE 7500
#define DUTY_MIN 112
#define DUTY_MAX 862

class Servo
{
private:
    TPM_Type *tpm_;
    uint8_t channel_;
    GPIO_Type *port_;
    uint8_t pin_;
    int angulo_atual_;

    int calcularDuty(int angulo) {
        return DUTY_MIN + ((DUTY_MAX - DUTY_MIN) * angulo) / 180;
    }

public:
    Servo(TPM_Type *tpm, uint8_t ch, GPIO_Type *p, uint8_t pn);
    void write(int theta);

    int getAnguloAtual() { return angulo_atual_; }
};