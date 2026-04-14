
#include "bsp.h"
#include "cli.h"
#include "hw_def.h"
#include "my_gpio.h"
#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// argv[1]  : "read" or "write"
// argv[2]  : pin_num Name (A5, B12, C0, ...)

void cliGpio(uint8_t argc, char **argv){
    if(argc >= 3) {
        char port_char = tolower(argv[2][0]);
        int pin_num = atoi(&argv[2][1]);
        uint8_t port_idx = port_char - 'a'; // if port_char is 'a', port_idx will be 0, if 'b' then 1, and so on. return value will be 0 for A, 1 for B, 2 for C, 3 for D, 4 for E, 5 for F, 6 for G, 7 for H
        int8_t state = gpioExtRead(port_idx, pin_num);
        
        if (strcmp(argv[1], "read") == 0) {
            
            if (state < 0) {
                cliPrintf("Invalid PORT or PIN_num (Ex. A5, B12, C0, ...) \r\n");
                
            } else {
                cliPrintf("GPIO %c%d: %d\r\n\n", toupper(port_char), pin_num, state);
            }
        } else if (strcmp(argv[1], "write") == 0 && argc == 4) {
            // argv[3] : 0 or 1
            int val = atoi(argv[3]);
            if(gpioExtWrite(port_idx, pin_num , val) == true){
                cliPrintf("GPIO %c%d Set to %d\r\n", toupper(port_char), pin_num, val);
            } else {
                cliPrintf("Invalid PORT or PIN (Ex. A5, B12, C0, ...) \r\n");
                cliPrintf("Failed to write to GPIO %c%d\r\n", toupper(port_char), pin_num);
            }
          }
          else {
              cliPrintf("Usage: gpio read [a~h][0~15] \r\n");
              cliPrintf("       gpio write [a~h][0~15] [0, 1]\r\n");
          }
        
    } else {
        cliPrintf("Usage: gpio read [a~h][0~15] \r\n");
        cliPrintf("       gpio write [a~h][0~15] [0, 1]\r\n");
    }
}

void cliLed(uint8_t argc, char **argv) {
  if (argc == 2) {
    if (strcmp(argv[1], "on") == 0) {
      ledOn();
      cliPrintf("LED ON\r\n");
    } else if (strcmp(argv[1], "off") == 0) {
      ledOff();
      cliPrintf("LED OFF\r\n");
    } else if (strcmp(argv[1], "toggle") == 0) {
      ledToggle();
      cliPrintf("LED TOGGLE\r\n");
    } else {
      cliPrintf("Invalid Command\r\n");
    }
  } else {
    cliPrintf("Usage: led [on|off|toggle]\r\n");
  }
}

void cliInfo(uint8_t argc, char **argv) {
  if (argc == 1) {
    cliPrintf("=================================\r\n");
    cliPrintf("  HW Model    :    STM32F411CEU6\r\n");
    cliPrintf("  FW Version  :    V1.0.0\r\n");
    cliPrintf(" Build Date : %s %s\r\n", __DATE__, __TIME__);

    uint32_t uid0, uid1, uid2;
    uint32_t hal = HAL_GetHalVersion();
        cliPrintf("  HAL Version : %d.%d.%d\r\n", (hal >> 24) & 0xFF, (hal >> 16) & 0xFF, hal & 0xFFFF);

    uid0 = HAL_GetUIDw0();
    uid1 = HAL_GetUIDw1();
    uid2 = HAL_GetUIDw2();

    uint32_t rev = HAL_GetREVID();
        cliPrintf("  HW Revision : %d\r\n", rev);
    uint32_t dev = HAL_GetDEVID();
        cliPrintf("  Device ID   : %d\r\n", dev);   
    
    cliPrintf("    Serial Number : %08x-%08x-%08x\r\n", uid0, uid1, uid2);
    cliPrintf("=================================\r\n");

  }
  if (argc == 2 || strcmp(argv[1], "uptime") == 0) {
    cliPrintf("System Uptime: %d ms \r\n", millis());
  } else {
    cliPrintf("Usage: info \r\n");
    cliPrintf("       info [uptime]\r\n");

  }
}

void cliSys(uint8_t argc, char **argv) {
  if (argc == 2 && strcmp(argv[1], "reset") == 0) {
    NVIC_SystemReset();
  } else {
    cliPrintf("Usage: sys reset\n");
  }
}

void apInit(void) {
  hwInit();
  cliAdd("led", cliLed);
  cliAdd("info", cliInfo);
  cliAdd("sys", cliSys);
  cliAdd("gpio", cliGpio);
//   cliAdd("cls", cliclear);


}
void apMain(void) {

  uartPrintf(0, "Hello World!\r\n");
  while (1) {
    cliMain();
  }
}


