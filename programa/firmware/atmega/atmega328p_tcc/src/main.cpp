
//------------------------- Bibliotecas ------------------------------
#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <Wire.h>

//------------------------- Macros de Bits ------------------------------

#define set_bit(adress,bit) (adress|=(1<<bit))
#define clr_bit(adress,bit) (adress&=~(1<<bit))
#define cpl_bit(adress,bit) (adress^=(1<<bit))
#define rd_bit(adress,bit) (adress&(1<<bit))

//------------------------- Macros de pinos------------------------------

#define F_CPU 16000000UL
#define PIN_RES_1 PORTB5        //Revisado
#define PIN_RES_2 PORTB4        //Revisado
#define PIN_FAN PORTB3          //Revisado
#define PIN_PELT PORTB2         //Revisado
#define PIN_LINHA_1 PORTB1      //Revidado
#define PIN_LINHA_2 PORTB0      //Revidado
#define PIN_LINHA_3 PORTD7      //Revidado
#define PIN_LINHA_4 PORTD6      //Revidado
#define PIN_COLUNA_1 PORTD2     //Revidado
#define PIN_COLUNA_2 PORTB3     //Revidado
#define PIN_COLUNA_3 PORTB4     //Revidado
#define PIN_COLUNA_4 PORTB5     //Revidado
#define PIN_TEMPERATURE PORTC0  //Revisado


//------------------------- Macros de acionamentos de pinos------------------------------

#define StartOn (rd_bit(PINB, PIN_START)>0?0:1) //Ativo no nível baixo

//Seta pino do resistor 1 como HIGH e LOW
#define HIGH_PIN_RESISTOR_1 (set_bit(PORTB, PIN_RES_1))
#define LOW_PIN_RESISTOR_1 (clr_bit(PORTB, PIN_RES_1))

//Seta pino do resistor 2 como HIGH e LOW
#define HIGH_PIN_RESISTOR_2 (set_bit(PORTB, PIN_RES_2))
#define LOW_PIN_RESISTOR_2 (clr_bit(PORTB, PIN_RES_2))

//Seta pino do FAN como HIGH e LOW
#define HIGH_PIN_FAN (set_bit(PORTB, PIN_FAN))
#define LOW_PIN_FAN (clr_bit(PORTB, PIN_FAN))

//Seta pino do PELT como HIGH e LOW
#define HIGH_PIN_PELT (set_bit(PORTB, PIN_PELT))
#define LOW_PIN_PELT (clr_bit(PORTB, PIN_PELT))

//Seta pinos da linha 1:4 (teclado) como HIGH e LOW
#define HIGH_PIN_LINHA_1 (set_bit(PORTB, PIN_LINHA_1))
#define HIGH_PIN_LINHA_2 (set_bit(PORTB, PIN_LINHA_2))
#define HIGH_PIN_LINHA_3 (set_bit(PORTB, PIN_LINHA_3))
#define HIGH_PIN_LINHA_4 (set_bit(PORTB, PIN_LINHA_4))
#define LOW_PIN_LINHA_1 (clr_bit(PORTB, PIN_LINHA_1))
#define LOW_PIN_LINHA_2 (clr_bit(PORTB, PIN_LINHA_2))
#define LOW_PIN_LINHA_3 (clr_bit(PORTB, PIN_LINHA_3))
#define LOW_PIN_LINHA_4 (clr_bit(PORTB, PIN_LINHA_4))

//RS232
#define TAXA 9600
#define BAUND_RATE (((F_CPU/(16UL*TAXA)))-1)


//------------------------- Constantes ------------------------------
  //referente a o teclado matriciado
const byte rows_keyboard = 4;
const byte columns_keyboard = 4;

  const char keyboard[rows_keyboard][columns_keyboard] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
  };


//------------------------- Estruturas ------------------------------

typedef struct PWMS {
    
    float PWM_RES_1 = 0;
    float PWM_RES_2 = 0;
    float PWM_Peltier = 0;
    float PWM_FAN = 0;

 } PWMS;

typedef struct info {

  const String microchip = "atmega328p";
  const String board = "arduino uno";
  String modo_operacao = "controle simples";

} info;

typedef struct variaveis_controlador {

  float KP = 0;
  float KI = 0;
  float KD = 0;

} variaveis_controlador;

typedef struct controlador {

  String eq_control = "s+1";

} controlador;

typedef struct flags {

  /* Bits |7|6|5|4|3|2|1|0| Bits

      Bit 7
      Bit 6
      Bit 5
      Bit 4
      Bit 3
      Bit 2
      Bit 1
      Bit 0
 */

  int8_t flags_control_1 = 0b00000000;
  
   /* Bits |7|6|5|4|3|2|1|0| Bits

      Bit 7
      Bit 6
      Bit 5
      Bit 4
      Bit 3
      Bit 2 DATA
      Bit 1 ACK
      Bit 0 START
 */

  int8_t flags_i2c = 0b00000000;

} flags;

typedef struct pilha {

  int *funcoes;

} pilha;



//------------------------- Variaveis ------------------------------

PWMS current_PWM;
info current_info;
variaveis_controlador controller;
flags flag;
int8_t Index_msg;
char Command[9];
float temperature;


//------------------------- Declaração das funções ------------------------------


  //função destinada a pre-setar como o chip ira começar.
void pre_setup_atmega();

  //função destinada a pre-setar como o chip ira começar.
void setup_atmega();

  //função destinada para configurar modo escrita no lcd sem modulo i2c.
void write_lcd();

    //função destinada para configurar modo escrita no lcd com modulo i2c.
void sendData_i2c(char SLA_W, char *DATA);

void ERROR_start_i2c();

void ERROR_addr_i2c();

void ERROR_data_i2c();


  //função destinada para carregar todos os parametros salvos.
unsigned char load_all_parameters();

  //função destinada para salvar todos os parametros salvos.
void save_all_parameters();

  //Função destinada a fazer o gerenciamento de tempo do microcontrolador
void pilha_funcionamento();

  //interpreta as instruções do teclado
void instrucion_keyboard();

float getPWM_RES_1();
void setPWM_RES_1();
float getPWM_RES_2();
void setPWM_RES_2();
float getPWM_Peltier();
void setPWM_Peltier();
float getPWM_Fan();
void setPWM_Fan();

  //distinado a fazer a leitura do sensor de temperatura.
float read_temp();

  //destinado a controlar apenas o Resistor 1;
void controlador_1();

  //destiado a controlar o Resistor 1, Fan e o Peltier
void controlador_2();

void ProcessMessageReceived(char *message);

unsigned char GetGainsMsg(char *message);

void sendMessage(char menssagem);

char convertFloadChar(float number);

int main(){
  
  setup_atmega();
  while (1){

  }
  


  return 0;
}



//------------------------- Definições das funções ------------------------------

void pre_setup_atmega(){


}

void setup_atmega(){
  pre_setup_atmega();



}


void sendData_i2c(char SLA_W, char *DATA){

  int8_t START = 8;
  int8_t MT_SLA_ACK = 18;
  int8_t MT_DATA_ACK = 28;

  //condição de start
  TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN));
  //Aguarde o sinalizador TWINT definido.  Isto indica que a condição START foi transmitida
  while (!(TWCR & (1<<TWINT)));
  /*
  Verifique o valor do registro de status do TWI.
  Mascare os bits do pré-escalador.
  Se o status for diferente de START vá para ERRO
  */
  if ((TWSR & 0xF8) != START) ERROR_start_i2c();
  //Carregue SLA_W no registro TWDR.  Limpar bit TWINT TWCR para iniciar a transmissão do endereço
  TWDR = SLA_W;
  TWCR = (1<<TWINT)|(1<<TWEN);
  //Aguarde o sinalizador TWINT definido.  Isso indica que o SLA+W foi transmitido e ACK/NACK foi recebido.
  while (!(TWCR & (1<<TWINT)))
  //Verifique o valor do registro de status do TWI.  Mascare os bits do pré-escalador.  Se o status for diferente de MT_SLA_ACK vá para ERROR
  if ((TWSR & 0xF8) != MT_SLA_ACK) ERROR_addr_i2c();

//Arrumar para fluxo de caracteres
  for (int i = 0; i < sizeof(DATA); i++){
     //Carregue DATA no registro TWDR. Limpe o bit TWINT no TWCR para iniciar a transmissão de dados
    TWDR = DATA[1];
    TWCR = (1<<TWINT) | (1<<TWEN);
    //Aguarde o sinalizador TWINT definido.  Isso indica que os DADOS foram transmitidos e ACK/NACK foram recebidos.
    while (!(TWCR & (1<<TWINT)));
    /*  
    Verifique o valor do registro de status do TWI.
    Mascare os bits do pré-escalador.
    Se o status for diferente de MT_DATA_ACK vá para ERROR
    */
    if ((TWSR & 0xF8)!= MT_DATA_ACK) ERROR_data_i2c();
    // condição de STOP
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); 
  }
}

void ERROR_start_i2c(){

 //seta flag de erro do I2C, quando ocorrer uma leitura da esp passar a flag
  set_bit(flag.flags_i2c, 1);

}

void ERROR_addr_i2c(){

  //seta flag de erro do I2C, quando ocorrer uma leitura da esp passar a flag
  set_bit(flag.flags_i2c, 2);

}

void ERROR_data_i2c(){

  //seta flag de erro do I2C, quando ocorrer uma leitura da esp passar a flag
  set_bit(flag.flags_i2c, 3);
}

unsigned char load_all_parameters(unsigned int endereco){

  while (EECR & (1 << EEPE));
  EEAR = endereco;
  EECR |= (1<<EERE);
  return EEDR;

}

void save_all_parameters(unsigned int endereco, unsigned char dado){

  while (EECR & (1 << EEPE));
  EEAR = endereco;
  EEDR = dado;
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
}


ISR(USART_RX_vect){

  char receivedByte = UDR0;
  UDR0 = receivedByte;

  if(receivedByte == '<'){
    Index_msg = 0;
    Command[Index_msg] = receivedByte;
    Index_msg++;
    return;
  }
  else if (receivedByte == '>'){
    Command[Index_msg] = receivedByte;
    Index_msg++;
    ProcessMessageReceived(Command);
  }

  Command[Index_msg] = receivedByte;
  Index_msg++;
}

ISR (INT0_vect){

}

  //------------------------- ProcessMessageReceived ------------------------------

    /*    Protocolo de comunicação

      <MENSSAGEM>

      < Indica começo da menagem
      > Indica o Final da messagem

      Protocolo de comunicação sera dado da seguinte forma

        <pXXX.XX>; onde p indica que se trata do ganho proporcional e XXX.XX um numero flutuante, esse comando seta o ganho p

        <iXXX.XX>; onde i indica que se trata do ganho integrador e XXX.XX um numero flutuante, esse comando seta o ganho i

        <dXXX.XX>; onde d indica que se trata do ganho derivativo e XXX.XX um numero flutuante, esse comando seta o ganho d

        <gp>; retorna o ganho proporcional onde é um numero flutuante na forma XXX.XX

        <gi>; retorna o ganho integrador onde é um numero flutuante na forma XXX.XX

        <gd>; retorna o ganho derivativo onde é um numero flutuante na forma XXX.XX

        <mX>; sendo o modo de controle (ver se sera implementado) onde X é o modo

        <t>; repasa a temperatura

        <pr1>; repassa o pwm do resistor 1
        
        <pr2>; repassa o pwm do resistor 2

        <pf>; repassa o pwm do fan

        <pp>; repassa o pwm da pastilha de peltier

        <a>; repassa a temperatura pwm do resitor 1, pwm do fan e, pwm da pastilha de peltier
        (Observar de passar nessa mesma ordem)

    */
    

void ProcessMessageReceived(char *message){
  //USART_TX_STRING(message);
  if(message[1] == 'p' && message[8] == '>'){
    controller.KP = GetGainsMsg(message);
  }
  else if (message[1] == 'i' && message[8] == '>'){
    controller.KI = GetGainsMsg(message);
  }
  else if (message[1] == 'd' && message[8] == '>'){
    controller.KD = GetGainsMsg(message);
  }
  else if(message[1] == 'g' && message[2] == 'p' && message[3] == '>'){
    char kp_char = (%f, controller.KP);
  }
  else if(message[1] == 'g' && message[2] == 'i' && message[3] == '>'){
    float ki = controller.KI;
  }
  else if(message[1] == 'g' && message[2] == 'd' && message[3] == '>'){
    float kp = controller.KD;
  }
  else if(message[1] == 't' && message[2] == '>'){
    char temperature_char = temperature;
    sendMessage(temperature_char);
  }
  else if(message[1] == 'a' && message[2] == '>'){
    char info;
    sendMessage(info);
  }
}


unsigned char GetGainsMsg(char *message){
  
  int c = message[2] - '0';
  int d = message[3] - '0';
  int u = message[4] - '0';
  int k = message[6] - '0';
  int p = message[7] - '0';
  return (c*100 + d*10 + u + k*0.1 + p*0.01);
}

char convertFloadChar(float number){

  unsigned char number_convert[6];
  int c, d, u, k, p;

  c = floor(number/100);
  d = floor((number-c*100)/10);
  u = floor(((number) - (c*100+d*10))/1);
  k = floor((((number) - (c*100+d*10+u))*10));
  p = floor((((number) - (c*100+d*10+u+k*0,1))*100));
  
  number_convert[0] = c + '0';
  number_convert[1] = d + '0';
  number_convert[2] = u + '0';
  number_convert[3] = '.';
  number_convert[4] = k + '0';
  number_convert[5] = p + '0';
  

  return number_convert;

}

