
#include "bsp.h"
#include "cli.h"
#include "hw_def.h"
#include "my_gpio.h"
#include "stm32f4xx_hal.h"
#include "uart.h"
#include "button.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

// button on/off --> enable/disable
void cliButton(uint8_t argc, char **argv){
    if(argc == 2) {
        if(strcmp(argv[1], "enable") == 0) {
            buttonEnable(true);
            cliPrintf("Button Interrupt Report : Enabled\r\n");
        } else if(strcmp(argv[1], "disable") == 0) {
            buttonEnable(false);
            
            cliPrintf("Button Interrupt Report : Disabled\r\n");
        } else {
            cliPrintf("Usage: button [enable|disable]\r\n");
        }
    } else {
        cliPrintf("Usage: button [enable|disable]\r\n");
        cliPrintf("Current Status : %s\r\n", buttonGetEnable() ? "Enabled" : "Disabled");
    }
}


static bool isSafeAddress(uint32_t addr) {
    // Define the valid memory regions for STM32F411CEU6
    if ((addr >= 0x08000000) && (addr <0x0807FFFF)) { // Flash memory
        return true;
    } else if (((addr >= 0x20000000) && (addr < 0x2001FFFF))) { // f411 SRAM
        return true;
    } else if (((addr >= 0x1FFF0000) && (addr < 0x1FFF7A0F))) { // Peripheral registers
        return true;
    } else if ((addr >= 0x40000000) && (addr < 0x5FFFFFFF)) { // System control space
        return true;  
    } else {
          return false; // Address is outside of valid regions
    }
    return false; // Address is outside of valid regions
}
// Memory Dump : md 0x8000-0000 32
void cliMd(uint8_t argc, char **argv){
  if(argc >= 2) {
    uint32_t addr = strtoul(argv[1], NULL, 16);
    uint32_t length = 16; // default length
    if(argc >= 3) {
      length = strtoul(argv[2], NULL, 0);
    }
    for(uint32_t i=0; i<length; i+=16) {
      // uint32_t data = *((volatile uint32_t*)(addr + i));
      cliPrintf("0x%08X ", addr + i);

      for (uint32_t j = 0; j < 16; j++) {
        if (i + j < length) { 
          uint32_t target_addr = addr + i + j;
          if (isSafeAddress(target_addr)) {
            uint8_t val = *(volatile uint8_t*)target_addr;
            cliPrintf("%02X ", val);
          } 
          else {
            cliPrintf("Not Vaild Address\r\n");
            break;
          } 
        }
        else {
          cliPrintf("   ");
        }
      }
      cliPrintf(" | ");
      for (uint32_t j = 0; j < 16; j++) {
        if (i + j < length) { 
          uint32_t target_addr = addr + i + j;
          if (isSafeAddress(target_addr)) {
            uint8_t val = *((volatile uint8_t*)target_addr);
            cliPrintf("%c", (val >= 32 && val <= 126) ? val : '.');
          } 
          else {
            cliPrintf("Not Vaild Address\r\n");
            break;
          } 
        }
      }
      cliPrintf("\r\n");
    }

    // uint32_t data = *((volatile uint32_t*)addr);
    // cliPrintf("0x%08X: 0x%08X\r\n", addr, data);
  } else {
    cliPrintf("Usage: md [address(hex)] [length]\r\n");
    cliPrintf("       md 0x)80000000 32\r\n");
  } 
}


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
  cliAdd("md", cliMd);
  cliAdd("button", cliButton);
//   cliAdd("cls", cliclear);


}
void apMain(void) {

  uartPrintf(0, "Hello World!\r\n");
  while (1) {
    cliMain();
  }
}


