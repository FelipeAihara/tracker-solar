#include "button.hpp"
#include <zephyr/sys/printk.h>

void button_isr_static(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    Button *btn = CONTAINER_OF(cb, Button, button_cb_data_);
    btn->handle_interrupt();
}

void button_debounce_work_handler(struct k_work *work)
{
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);
    Button *btn = CONTAINER_OF(dwork, Button, debounce_work_);

    Modo anterior = btn->modo_atual_;
    btn->modo_atual_ = Button::proximoModo(anterior);
    btn->contador_cliques_++;

    printk("Botao pressionado (clique #%u): modo %d -> %d\r\n",
           btn->contador_cliques_, anterior, btn->modo_atual_);
}

Button::Button(const struct gpio_dt_spec btn) : btn_(btn), contador_cliques_(0)
{
    if (!gpio_is_ready_dt(&btn_)) {
        printk("Botao: dispositivo GPIO nao esta pronto\r\n");
    }

    int ret = gpio_pin_configure_dt(&btn_, GPIO_INPUT | GPIO_PULL_UP);
    if (ret < 0) {
        printk("Botao: falha ao configurar pino (%d)\r\n", ret);
    }

    ret = gpio_pin_interrupt_configure_dt(&btn_, GPIO_INT_EDGE_FALLING);
    if (ret < 0) {
        printk("Botao: falha ao configurar interrupcao (%d)\r\n", ret);
    }

    gpio_init_callback(&button_cb_data_, button_isr_static, BIT(btn_.pin));
    gpio_add_callback(btn_.port, &button_cb_data_);
    modo_atual_ = MODO_AUTOMATICO;

    k_work_init_delayable(&debounce_work_, button_debounce_work_handler);

    printk("Botao pronto no pino %d (modo inicial: AUTOMATICO)\r\n", btn_.pin);
}

Modo Button::proximoModo(Modo atual)
{
    switch (atual)
    {
        case MODO_AUTOMATICO:
            return MODO_MANUAL;
        case MODO_MANUAL:
            return MODO_AUTOMATICO;
        default:
            return MODO_AUTOMATICO;
    }
}

void Button::handle_interrupt()
{
    k_work_reschedule(&debounce_work_, K_MSEC(DEBOUNCE_MS_));
}
