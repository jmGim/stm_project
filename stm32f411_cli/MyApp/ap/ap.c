
#include "bsp.h"
#include "cli.h"
#include "hw_def.h"
#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>

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
  cliAdd("cls", cliclear);

}
void apMain(void) {

  uartPrintf(0, "Hello World!\r\n");
  while (1) {
    cliMain();
  }
}


