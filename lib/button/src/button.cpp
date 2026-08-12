#include "button.hpp"

void button_isr_static(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    Button *btn = CONTAINER_OF(cb, Button, button_cb_data_);
    btn->handle_interrupt();
}

Button::Button(const struct gpio_dt_spec btn) : btn_(btn)
{
    gpio_pin_configure_dt(&btn_, GPIO_INPUT | GPIO_PULL_UP);
    gpio_pin_interrupt_configure_dt(&btn_, GPIO_INT_EDGE_FALLING);
    gpio_init_callback(&button_cb_data_, button_isr_static, BIT(btn_.pin));
    gpio_add_callback(btn_.port, &button_cb_data_);
    modo_atual_ = MODO_AUTOMATICO;
}

void Button::handle_interrupt()
{
    switch(modo_atual_)
    {
        case MODO_AUTOMATICO:
            modo_atual_ = MODO_MANUAL_1G;
            break;
        case MODO_MANUAL:
            modo_atual_ = MODO_AUTOMATICO;
            break;
    }
}