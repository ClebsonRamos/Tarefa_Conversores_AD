Projeto: Controle de LEDs e display SSP1306 com joystick e manipulação de PWM


DESCRIÇÃO

Este projeto manipula uma animação em um display SSD1306, formada por um quadrado e 8x8 pixels, onde a posição desse quadrado é alterada em função dos valores lidos por 2 potenciômetros associados à um joystick na placa BitDogLab. A comunicação do joystick com a tela é realizada por um conversor analógico-digital, que interpreta os valores lidos do joystick, e atualiza a posição do quadrado na tela através do protocolo de comunicação I2C.
O botão A ativa os LEDs azul e vermelho, que tem a intensidade das luzes alteradas via PWM em função dos valores lidos dos potenciômetros do joystick. O botão do joystick ativa a alteração de espessura de uma borda mostrada no display, além de ativar o LED verde que indica que essa alteração foi realizada. O código foi desenvolvido em linguagem C utilizando o Pico SDK.


COMPONENTES

* Microcontrolador Raspberry Pi Pico W.
* Joystick presente na placa BitDogLab.
* LEDs RGB.
* Display SSD1306.


FUNCIONALIDADES

1. Tela SSD1306:
   * Exibe a imagem da movimentação do quadrado de 8x8 pixels sendo controlado pelo joystick.
2. Controle dos LEDs RGB:
   * Os LEDs azul e vermelho são ativados ou desativados ao pressionar o botão A.
   * Eles têm suas intensidades de luz manipuladas via PWM em função da movimentação do joystick.
3. Joystick:
   * Controla a posição do quadrado de 8x8 pixels no display e, caso ativados, controla a intensidade dos LEDs azul e vermelho.
   * O botão do joystick altera a espessura de um retângulo exibido na borda na tela.

ESTRUTURA DO CÓDIGO

1. Função main():
   * Chama as funçoes de inicialização dos pinos, do display e do PWM.
   * Habilita as interrupções que serão acionadas pelo botão A e pelo botão do joystick.
   * Chama as funções desenhar_quadrado() e desenhar_retangulo() para exibir inicialmente o quadrado 8x8 centralizado na tela e a borda com espessura fina.
   * No loop principal, lê continuamente os valores obitidos pelo ADC do joystick, realiza uma conversão por proporção em função das dimensões do display e passa via parâmetro para as funções desenhar_quadrado() e manipulacao_pwm_leds().
2. Função configuracao_inicial_pwm():
   * Habilita os pinos dos LEDs azul e vermelho para manipulação via PWM.
3. Função desenhar_quadrado():
   * Desenha o quadrado de 8x8 pixels no display, de acordo com as coordenadas X e Y na tela passadas via parâmetros.
4. Função desenhar_retangulo():
   * Desenha um retângulo como borda no display em função da variável estado_botao_joystick, que registra se o botão do joystick foi acionado ou não.
5. Função funcao_de_interrupcao():
   * Gerencia as rotinas de interrupção atribuídas ao acionamento dos botões A e do joystick.
6. Função inicializacao_do_display():
   * Inicializa e configura o display SSD1306.
7. Função inicializacao_dos_pinos():
   * Inicializa e habilita os pinos GPIOs atribuídos ao display, LEDs, joysticks e botões para o propósito em que será utilizado.
8. Função manipulacao_pwm_leds():
   * Realiza o cálculo aritmético para a alteração da intensidade dos LEDs azul e vermelho em função dos valores lidos do joystick, passados por parâmetro.
9. Função tratamento_debounce():
    * Realiza o tratamento via software da oscilação na leitura do sinal de acionamento dos botões habilitados (debounce).

LINK DO VÍDEO DA APRESENTAÇÃO DO PROJETO
  * https://drive.google.com/file/d/1neddTUD8DTCwfEF_0WsQgHPh7B7oDs6P/view?usp=sharing
