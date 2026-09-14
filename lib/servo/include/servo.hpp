#pragma once

#include <zephyr/drivers/pwm.h>

#define TOL 9

/**
 * Servo.hpp
 *
 * Controle de servo padrão (0-180°) via PWM (canal LEDC do ESP32S3),
 * usando a API genérica de PWM do Zephyr.
 */
class Servo
{
private:
    struct pwm_dt_spec pwm_;
    uint32_t pulso_min_ns_;
    uint32_t pulso_max_ns_;
    int angulo_atual_;

public:
    /**
     * @param pwm            Especificação PWM (canal/período) vinda do devicetree.
     * @param pulso_min_ns   Largura de pulso (ns) correspondente a 0°.
     * @param pulso_max_ns   Largura de pulso (ns) correspondente a 180°.
     */
    explicit Servo(const struct pwm_dt_spec &pwm,
                   uint32_t pulso_min_ns = 300000,
                   uint32_t pulso_max_ns = 2300000);

    /** Move o servo para o ângulo informado, em graus, entre [0,180]. */
    void write(int angulo_graus);

    int getAnguloAtual() const { return angulo_atual_; }

    /**
     * Converte um ângulo [0,180] na largura de pulso (ns) correspondente.
     * Extraído como método estático puro (sem tocar hardware) para
     * poder ser testado sem depender de um dispositivo PWM real.
     */
    static uint32_t calcularPulso(int angulo_graus, uint32_t pulso_min_ns, uint32_t pulso_max_ns);
};
