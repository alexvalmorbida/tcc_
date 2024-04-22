#include "modbus.h"


int message_processing(int modo, char *meg){


    return 0;
}

int convert_char_to_int(char *menssge){

    return 0;
}
int get_addr(){

    return ;
}

int set_addr(int addr){

    current_parameters.ADDR = addr;
}
int get_modo(){

    return current_parameters.modo_modbus;
}

int set_modo(int modo){
    if (modo == MODBUS_RTU){
        current_parameters.modo_modbus = MODBUS_RTU;
        return 0;
    }
    else if (modo == MODBUS_ASCII){
        current_parameters.modo_modbus = MODBUS_ASCII;
        return 1;
    }
    else{
        return -1;
    }
    
    
}

int get_baud_rate_modbus(){
    return current_parameters.modo_modbus;
}
modbus_parameters load_parameters();
modbus_parameters save_parameters();