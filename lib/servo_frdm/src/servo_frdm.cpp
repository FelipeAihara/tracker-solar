#include "servo.hpp"

Servo::Servo(TPM_Type *tpm, uint8_t ch, GPIO_Type *p, uint8_t pn) 
    : tpm_(tpm), channel_(ch), port_(p), pin_(pn)
{
    angulo_atual_ = 90;
    pwm_tpm_Init(tpm_, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Ch_Init(tpm_, channel_, TPM_PWM_H, port_, pin_);
    int duty_inicial = calcularDuty(angulo_atual_);
    pwm_tpm_CnV(tpm_, channel_, duty_inicial);
}

void Servo::write(int theta)
{
    if (theta < 0) theta = 0;
    if (theta > 180) theta = 180;

    int duty = calcularDuty(theta);
    pwm_tpm_CnV(tpm_, channel_, duty);
    angulo_atual_ = theta; // Atualiza o estado interno
}
