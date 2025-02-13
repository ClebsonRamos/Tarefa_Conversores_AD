//-----BIBLIOTECAS UTILIZADAS-----
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

//-----DIRETIVAS PARA AS CONSTANTES-----
#define PINO_LED_AZUL 12
#define PINO_LED_VERDE 11
#define PINO_LED_VERMELHO 13
#define PINO_BOTAO_JOYSTICK 22
#define PINO_JOYSTICK_X 27
#define PINO_JOYSTICK_Y 26
#define PINO_BOTAO_A 5
#define PINO_DISPLAY_SDA 14
#define PINO_DISPLAY_SCL 15

//-----VARIÁVEIS GLOBAIS-----
static volatile bool estado_botao_A = false;
static volatile bool estado_botao_joystick = false;
uint16_t wrap_direcao_x = 4095, wrap_direcao_y = 4095;
uint16_t duty_cycle_x = 0, duty_cycle_y = 0;
uint numero_slice_x, numero_slice_y;
float divisor_de_clock_xy = 4.0;
static volatile uint32_t tempo_passado = 0;

//-----PROTÓTIPOS DAS FUNÇÕES-----
void configuracao_inicial_pwm(void);
void funcao_de_interrupcao(uint pino, uint32_t evento);
void inicializacao_dos_pinos(void);
bool tratamento_deboucin(void);

//-----FUNÇÃO PRINCIPAL-----
int main(void){
    inicializacao_dos_pinos();
    configuracao_inicial_pwm();

    // Habilitação dos botões para ativação das interrupções
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &funcao_de_interrupcao);
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &funcao_de_interrupcao);

    while(true){

    }

    return 0;
}

//-----FUNÇÕES COMPLEMENTARES-----
void configuracao_inicial_pwm(void){
    numero_slice_x = pwm_gpio_to_slice_num(PINO_JOYSTICK_X);
    pwm_set_clkdiv(numero_slice_x, divisor_de_clock_xy);
    pwm_set_wrap(numero_slice_x, wrap_direcao_x);
    pwm_set_gpio_level(PINO_JOYSTICK_X, duty_cycle_x);
    pwm_set_enabled(numero_slice_x, false);

    numero_slice_y = pwm_gpio_to_slice_num(PINO_JOYSTICK_Y);
    pwm_set_clkdiv(numero_slice_y, divisor_de_clock_xy);
    pwm_set_wrap(numero_slice_y, wrap_direcao_y);
    pwm_set_gpio_level(PINO_JOYSTICK_Y, duty_cycle_y);
    pwm_set_enabled(numero_slice_y, false);
}

void funcao_de_interrupcao(uint pino, uint32_t evento){
    if(pino == PINO_BOTAO_A){
        if(tratamento_deboucin()){
            estado_botao_A = !estado_botao_A;
            pwm_set_enabled(numero_slice_x, estado_botao_A);
            pwm_set_enabled(numero_slice_y, estado_botao_A);
        }
    }else if(pino == PINO_BOTAO_JOYSTICK){
        if(tratamento_deboucin()){
            estado_botao_joystick = !estado_botao_joystick;
            gpio_put(PINO_LED_VERDE, estado_botao_joystick);
        }
    }
}

void inicializacao_dos_pinos(void){
    gpio_init(PINO_LED_VERDE);
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);
    gpio_put(PINO_LED_VERDE, false);

    adc_init();
    adc_gpio_init(PINO_JOYSTICK_X);
    adc_gpio_init(PINO_JOYSTICK_Y);
}

bool tratamento_deboucin(void){
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());
    if(tempo_atual - tempo_passado > 200000){
        tempo_passado = tempo_atual;
        return true;
    }else
        return false;
}
