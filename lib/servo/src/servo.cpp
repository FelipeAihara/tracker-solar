#include "servo.hpp"
#include <zephyr/sys/printk.h>

Servo::Servo(const struct pwm_dt_spec &pwm, uint32_t pulso_min_ns, uint32_t pulso_max_ns)
    : pwm_(pwm), pulso_min_ns_(pulso_min_ns), pulso_max_ns_(pulso_max_ns), angulo_atual_(90)
{
    if (!pwm_is_ready_dt(&pwm_)) {
        printk("Dispositivo PWM do servo nao esta pronto\r\n");
        return;
    }
    write(angulo_atual_);
}

uint32_t Servo::calcularPulso(int angulo_graus, uint32_t pulso_min_ns, uint32_t pulso_max_ns)
{
    if (angulo_graus < 0)   angulo_graus = 0;
    if (angulo_graus > 180) angulo_graus = 180;

    return pulso_min_ns +
           static_cast<uint32_t>((pulso_max_ns - pulso_min_ns) * static_cast<uint32_t>(angulo_graus) / 180U);
}

void Servo::write(int angulo_graus)
{
    if (angulo_graus - angulo_atual_ <= TOL && angulo_graus - angulo_atual_ >= -TOL) return;
    if (angulo_graus < 0)   angulo_graus = 0;
    if (angulo_graus > 180) angulo_graus = 180;

    if (!pwm_is_ready_dt(&pwm_)) {
        return;
    }

    uint32_t pulso_ns = calcularPulso(angulo_graus, pulso_min_ns_, pulso_max_ns_);
    int ret = pwm_set_pulse_dt(&pwm_, pulso_ns);
    if (ret < 0) {
        printk("Nao foi possivel definir o pulso do servo: %d\r\n", ret);
        return;
    }
    angulo_atual_ = angulo_graus;
}
