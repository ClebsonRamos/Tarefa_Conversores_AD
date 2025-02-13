//-----BIBLIOTECAS UTILIZADAS-----
#include <stdio.h>
#include "pico/stdlib.h"
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
static volatile bool estado_botao_joystick = false;
uint16_t wrap_direcao_x = 4095, whap_direcao_y = 4095;

//-----PROTÓTIPOS DAS FUNÇÕES-----
void inicializacao_dos_pinos(void);

//-----FUNÇÃO PRINCIPAL-----
int main(void){
    inicializacao_dos_pinos();

    while(true){

    }

    return 0;
}

//-----FUNÇÕES COMPLEMENTARES-----
void inicializacao_dos_pinos(void){
    gpio_init(PINO_LED_VERDE);
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);
    gpio_put(PINO_LED_VERDE, false);
}
