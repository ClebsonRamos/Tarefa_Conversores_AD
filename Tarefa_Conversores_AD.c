//-----BIBLIOTECAS UTILIZADAS-----
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "inclusao/ssd1306.h"
#include "hardware/timer.h"

//-----DIRETIVAS PARA AS CONSTANTES-----
#define PINO_LED_AZUL 12
#define PINO_LED_VERDE 11
#define PINO_LED_VERMELHO 13
#define PINO_BOTAO_JOYSTICK 22
#define PINO_JOYSTICK_X 27 // Canal 1
#define PINO_JOYSTICK_Y 26 // Canal 0
#define PINO_BOTAO_A 5
#define PINO_DISPLAY_SDA 14
#define PINO_DISPLAY_SCL 15
#define I2C_PORTA i2c1
#define ENDERECO 0x3C

//-----VARIÁVEIS GLOBAIS-----
static volatile bool estado_botao_A = false; // Controla o estado do botão A
static volatile bool estado_botao_joystick = false; // Controla o estado do botão do joystick
uint16_t wrap_direcao_xy = 2048; // Período do PWM
uint16_t duty_cycle_x = 0, duty_cycle_y = 0; // Período de nível alto de sinal para as direções X e Y do PWM.
static volatile uint numero_slice_x, numero_slice_y;
static volatile uint32_t tempo_passado = 0; //Registro do tempo para debounce dos botões.
ssd1306_t ssd; // Inicialização da estrutura do display.

uint8_t quadrado[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Vetor com o código em hexadecimal para o quadrado da tela.

//-----PROTÓTIPOS DAS FUNÇÕES-----
void configuracao_inicial_pwm(void);
void desenhar_quadrado(uint8_t x, uint8_t y);
void desenhar_retangulo(void);
void funcao_de_interrupcao(uint pino, uint32_t evento);
void inicializacao_do_display(void);
void inicializacao_dos_pinos(void);
void manipulacao_pwm_leds(uint16_t x, uint16_t y);
bool tratamento_debounce(void);

//-----FUNÇÃO PRINCIPAL-----
int main(void){
    uint16_t aux_x, aux_y;
    uint16_t adc_valor_x, adc_valor_y; // Variáveis para armazenamento temporário do valor lido pelo ADC.

    inicializacao_dos_pinos();
    inicializacao_do_display();
    configuracao_inicial_pwm();

    // Habilitação dos botões para ativação das interrupções
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &funcao_de_interrupcao);
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &funcao_de_interrupcao);

    desenhar_quadrado(60, 28); // Inicia com o quadrado desenhado no centro da tela.
    desenhar_retangulo(); // Desenha uma borda retangular na tela.

    while(true){
        adc_select_input(0);
        adc_valor_y = adc_read();
        adc_select_input(1);
        adc_valor_x = adc_read();
        aux_x = adc_valor_x * 15 / 512;
        aux_y = 56 - adc_valor_y * 7 / 512;
        desenhar_quadrado(aux_x, aux_y);
        if(estado_botao_A)
            manipulacao_pwm_leds(adc_valor_x, adc_valor_y);
        sleep_ms(50);
    }

    return 0;
}

//-----FUNÇÕES COMPLEMENTARES-----
// Função para configuração inicial para o PWM dos LEDs azul e vermelho.
void configuracao_inicial_pwm(void){
    numero_slice_x = pwm_gpio_to_slice_num(PINO_LED_VERMELHO);
    pwm_set_wrap(numero_slice_x, wrap_direcao_xy);
    pwm_set_enabled(numero_slice_x, false);

    numero_slice_y = pwm_gpio_to_slice_num(PINO_LED_AZUL);
    pwm_set_wrap(numero_slice_y, wrap_direcao_xy);
    pwm_set_enabled(numero_slice_y, false);
}

// Função para desenhar o quadrado de 8x8 pixels.
void desenhar_quadrado(uint8_t x, uint8_t y){
    uint16_t index = 0;

    ssd1306_fill(&ssd, true);
    desenhar_retangulo();

    for(uint8_t i = 0; i < 8; ++i){
        uint8_t line = quadrado[i];
        for(uint8_t j = 0; j < 8; ++j){
            ssd1306_pixel(&ssd, x + i, y + j, line & (1 << j));
        }
    }
    ssd1306_send_data(&ssd);
}

// Função para desenhar a borda retangular da tela.
void desenhar_retangulo(void){
    // Alterna entre borda fina e grossa de acordo com o estado do botão do joystick.
    if(estado_botao_joystick)
        ssd1306_rect(&ssd, 3, 3, 122, 58, false, true);
    else
        ssd1306_rect(&ssd, 1, 1, 126, 62, false, true);
}

// Função para gerenciamento da interrupção habilitada para os botões.
void funcao_de_interrupcao(uint pino, uint32_t evento){
    if(pino == PINO_BOTAO_A){
        bool resultado_debouce = tratamento_debounce();
        if(resultado_debouce){
            estado_botao_A = !estado_botao_A;
            if(!estado_botao_A)
                manipulacao_pwm_leds(0, 0);
            pwm_set_enabled(numero_slice_x, estado_botao_A);
            pwm_set_enabled(numero_slice_y, estado_botao_A);
        }
    }else if(pino == PINO_BOTAO_JOYSTICK){
        bool resultado_debouce = tratamento_debounce();
        if(resultado_debouce){
            estado_botao_joystick = !estado_botao_joystick;
            gpio_put(PINO_LED_VERDE, estado_botao_joystick);
        }
    }
}

// Função para inicialização do display SSD1306.
void inicializacao_do_display(void){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORTA); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Função para inicialização dos pinos utilizados neste programa.
void inicializacao_dos_pinos(void){
    gpio_init(PINO_LED_VERDE);
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);
    gpio_put(PINO_LED_VERDE, false);

    adc_init();
    adc_gpio_init(PINO_JOYSTICK_X);
    adc_gpio_init(PINO_JOYSTICK_Y);

    gpio_init(PINO_BOTAO_A);
    gpio_set_dir(PINO_BOTAO_A, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_A);

    gpio_init(PINO_BOTAO_JOYSTICK);
    gpio_set_dir(PINO_BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_JOYSTICK);

    gpio_set_function(PINO_LED_VERMELHO, GPIO_FUNC_PWM);
    gpio_set_function(PINO_LED_AZUL, GPIO_FUNC_PWM);

    i2c_init(I2C_PORTA, 400 * 1000); // Inicialização do protocolo I2C em 400 kHz.
    gpio_set_function(PINO_DISPLAY_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PINO_DISPLAY_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_DISPLAY_SDA);
    gpio_pull_up(PINO_DISPLAY_SCL);
}

// Função para amanipulação da intensidade dos LEDs via PWM.
void manipulacao_pwm_leds(uint16_t x, uint16_t y){
    if(x > 2048){
        duty_cycle_x = x - 2048;
    }else if(x < 2048){
        duty_cycle_x = 2048 - x;
    }else if(x == 2048){
        duty_cycle_x = 0;
    }

    if(y > 2048){
        duty_cycle_y = y - 2048;
    }else if(y < 2048){
        duty_cycle_y = 2048 - y;
    }else if(y == 2048){
        duty_cycle_y = 0;
    }

    pwm_set_gpio_level(PINO_LED_VERMELHO, duty_cycle_x);
    pwm_set_gpio_level(PINO_LED_AZUL, duty_cycle_y);
}

// Função para tratamento do bounce.
bool tratamento_debounce(void){
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if(tempo_atual - tempo_passado > 200){ // Retorna true caso o tempo passado seja maior que 200 milissegundos.
        tempo_passado = tempo_atual;
        return true;
    }else // Retorna falso em caso contrário.
        return false;
}
