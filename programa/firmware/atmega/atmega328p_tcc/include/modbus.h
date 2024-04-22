#ifndef _MODBUS_H
#define _MODBUS_H


//------------------------- Bibliotecas ------------------------------

#include <string.h>
#include <stdio.h>


//------------------------- Macros ------------------------------

#define MODBUS_RTU 0
#define MODBUS_ASCII 1


//------------------------- Variaveis ------------------------------


typedef struct modbus_parameters {
    int16_t ADDR;
    int8_t modo_modbus;
    int32_t baud_rate_modbus;

 } modbus_parameters;

modbus_parameters current_parameters;


//------------------------- Prototipo de funções ------------------------------

int message_processing(int modo, char *meg);
void return_data(char *);
int get_addr();
int set_addr(int addr);
int get_modo();
int set_modo(int modo);
modbus_parameters load_parameters();
modbus_parameters save_parameters();


#endif