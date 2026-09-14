#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

typedef enum
{
    MODO_AUTOMATICO,
    MODO_MANUAL,
} Modo;

class Button
{
private:
    static constexpr int32_t DEBOUNCE_MS_ = 250;

    const struct gpio_dt_spec btn_;
    struct gpio_callback button_cb_data_;
    struct k_work_delayable debounce_work_;

    // volatile: incrementado no work handler (contexto de thread da
    // system workqueue) e lido no loop principal.
    volatile uint32_t contador_cliques_;

    friend void button_isr_static(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
    friend void button_debounce_work_handler(struct k_work *work);

public:
    // volatile: modificado no work handler de debounce e lido no loop
    // principal — sem isso o compilador pode cachear a leitura e o
    // loop nunca "ver" a mudança de modo.
    volatile Modo modo_atual_;
    Button(const struct gpio_dt_spec btn);

    // Chamado na ISR a cada borda (inclusive bounce). So reagenda o
    // work de debounce; nao toca em modo_atual_ nem contador_cliques_
    // diretamente — isso e feito no handler, ja fora de contexto de ISR.
    void handle_interrupt();

    Modo getModoAtual() const { return modo_atual_; }
    uint32_t getContadorCliques() const { return contador_cliques_; }

    /**
     * Calcula o próximo modo no ciclo AUTOMATICO -> MANUAL_1G -> MANUAL_2B
     * -> AUTOMATICO. Extraído como método estático puro (sem tocar
     * hardware) para poder ser testado sem GPIO real.
     */
    static Modo proximoModo(Modo atual);
};
