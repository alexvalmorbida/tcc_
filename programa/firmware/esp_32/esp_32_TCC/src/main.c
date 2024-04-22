#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include <esp_err.h>
#include <sdkconfig.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <freertos/FreeRTOS.h>
#include <esp_transport_tcp.h>
#include <soc/uart_pins.h>
#include <soc/interrupts.h>
#include <driver/uart.h>
#include <esp_netif.h>
#include <ping/ping_sock.h>


/*não esta funcionando, motivo ? se corrigio os erros de path, mas mesmo assim retorna erro

links de foruns 

https://community.platformio.org/t/platformio-is-broken-for-any-esp32/26580
https://community.platformio.org/t/problem-with-xtensa-esp32-elf-when-compiling/20736/2
https://community.platformio.org/t/how-to-add-paths-that-depend-on-selected-framework-version/31503/6
https://community.platformio.org/t/i-cant-get-this-program-to-run-on-esp32-with-platformio/32367
*/
//include "../examples/common_components/protocol_examples_common/include/protocol_examples_common.h"
/*
Reescrever as funcoes de exeple_connect() e exemple_disconect()

----------function setup-------
    example_connect();
    vTaskDelay(5000/portTICK_PERIOD_MS);
    example_disconnect();

*/
 
//------------------------ Macros -------------------------

    //WIFI (modo STA)

#define  ESP_WIFI_SSID "DEZ_LUCIA"
#define  ESP_WIFI_PASS "345alex34567"
#define  ENDERECO_MSG "192.168.0.110"


    // Modbus
#define MB_TCP_PORT_NUMBER      (CONFIG_FMB_TCP_PORT_DEFAULT)
#define MB_MDNS_PORT            (502)






//------------------------ Variaveis -------------------------


wifi_sta_config_t conf;

wifi_config_t config = {.sta.ssid = ESP_WIFI_SSID, .sta.password = ESP_WIFI_PASS,
                        .sta.bssid_set = false, .sta.threshold.authmode = WIFI_AUTH_WPA2_PSK};




wifi_interface_t config_mode;

int estado_wifi = 0;

//static const char *TAG_STA = "WIFI STA";


//------------------------ Prototipos de funções -------------------------
void setup_pins();
void setup_serial();
void setup_wifi();
void setup_tcp_ip();
void setup_modBus();
void setup();
void modo_off();
void send_msg_atmega();
char *receive_atemega();
void send_msg_SCADA();
char receive_msg_SCADA();
void wifi_handler();
void info_IP();
void ping();


void app_main(){

    setup();
    while (1){
        //printf("Meu IP: ");
        //printf(ESP_IF_WIFI_STA, "\n");
        info_IP();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    


}



//------------------------ Dclarações de Funções -------------------------



void setup_pins(){

}

void setup_serial(){


}

void setup_wifi(){
    wifi_init_config_t config_init = WIFI_INIT_CONFIG_DEFAULT();

    //Inicializa o NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    //Inicializa a interface de Rede
    ESP_ERROR_CHECK(esp_netif_init());

    //Inicializa a tarefa padrão de tratamento de eventos
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    
    

    //aloca recurso e inicia o drive de WIFI, tambem inicia a terefa de WIFI
    ESP_ERROR_CHECK(esp_wifi_init(&config_init));

    //Seta o modo do Wifi da ESP32 como modo STA
    esp_wifi_set_mode(WIFI_MODE_STA);


    //Configura o modo de operação como STA e passa os parametros de ssid e password da rede
    esp_wifi_set_config(WIFI_MODE_STA, &config);

    ESP_ERROR_CHECK(esp_wifi_start());

    //ESP_ERROR_CHECK(esp_wifi_connect());


}

void setup_tcp_ip(){
    //inicializa a pilha TCP ip
    esp_netif_init();
    
}

void setup_modBus(){
    setup_tcp_ip();



}

void setup(){

    //setup_pins();
    //setup_serial();
    setup_wifi();
    //setup_tcp_ip();
    //setup_modBus();

    
}

void wifi_handler(){
    switch (estado_wifi){
    case 1:
        printf("teste");
        break;
    
    default:
        break;
    }
}

void info_IP(){
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(IP_EVENT_STA_GOT_IP,&ip_info);
    printf("My IP: " IPSTR "\n", IP2STR(&ip_info.ip));
    printf("My GW: " IPSTR "\n", IP2STR(&ip_info.gw));
    printf("My NETMASK: " IPSTR "\n", IP2STR(&ip_info.netmask));
}

void modo_off(){

}




//teste Ping
//-----------------------------------------------
