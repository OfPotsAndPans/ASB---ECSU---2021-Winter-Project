/*
 * Abdulsalam Bdeir
 * 1/4/21 - 1/8/21
 * This is where I am trying to write my program to record audio using GY-MAX4466 microphone module and the ESP8266 
 * 
 * When the ESP8266 is connected to WiFi it should begin to store audio data 
 * When buffer is full send the data to a listening server 
 * Should set the server IP in a new function 
 * 
 * Resources: 
 * https://github.com/PetteriAimonen/esp-walkie-talkie , 
 * http://essentialscrap.com/esptalkie/ , 
 * https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-reference/system/system.html , 
 * https://hackaday.com/2016/07/15/baby-monitor-rebuild-is-also-esp8266-audio-streaming-how-to/ , 
 * https://stackoverflow.com/questions/28586644/speech-streaming-over-wifi , 
 * http://cholla.mmto.org/esp8266/gpio/ , 
 * https://stackoverflow.com/questions/54587157/analog-to-digital-sampling-rate-affected-by-string-function-on-esp8266/54591853#54591853 ,  
 * https://github.com/espressif/esp8266_mp3_decoder ,
 * https://hackaday.com/2018/04/12/audio-hacking-with-the-esp8266/ ,
 * https://www.hackster.io/middleca/sending-sound-over-the-internet-f097b4
 * 
 * I spent the majority of my time trying to figure out how to store the analog data in the ESP8266 and how to then convert that series to digital output.
 * I did not get very far. A lot of resources used parts I did not have. 
 * The one resource I found online that was able to do without any other parts had little documentation and I was unable to understand what they were doing 
 *  
  */
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "mem.h"
#include "espconn.h" //https://github.com/esp8266/Arduino/blob/master/tools/sdk/include/espconn.h
#include <string.h>

static const int pin = 2;
bool got_ip = false, now_connected = false;
bool buffer_full = false;
struct espconn *my_connection;

#ifdef MEASURE_SEND_TIME
void FLASH_ATTR data_sent_cb_function(void)
{
  gpio_low(10);
}
#endif
  

// Add your WiFi credentials so ESP8266 can connect to the WiFi AP.
void  FLASH_ATTR user_set_station_config(void)
{
  const char* ssid = "Jharnka";
  const char* password = "removed"; //password removed for security 
  struct station_config stationConf;
  stationConf.bssid_set = 0; //don't check MAC of AP
  wifi_station_set_config_current(&stationConf);
}

void FLASH_ATTR now_connected(void *arg)
{
  Serial.println("Now we are connected to TCP server\n");
  now_connected = true;
}
void FLASH_ATTR disconnected(void *arg, sint8 err)
{
  Serial.println("It's broken! %d\n", err);
  espconn_connect(my_connection);
}

#define CS 15
#define MOSI 13
#define MISO 12
#define SCLK 14

////Read Buffer 
//
//uint8_t cnt = 0;
//uint8_t read_byte = 0;
//gpio_low(CS); //start low
//while(count > 0)
//  {
//    for(cnt = 8; cnt > 0 ; cnt--)
//    {
//      gpio_high(SCLK);
//      read_byte |= (GPIO_INPUT_GET(MISO) << (cnt - 1 ));
//      gpio_low(SCLK);
//    }
//    *buf = read_byte;
//    buf++;
//    os_delay_us(8);//pause processing
//    count--;
//    read_byte = 0;//
//  }
//  gpio_high(CS); //now 




//void setup() {
//  // put your setup code here, to run once:
//
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
//
//}
