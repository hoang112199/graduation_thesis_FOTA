#ifndef TASK_LTE
#define TASK_LTE

#include "uart.h"
#include "string.h"
#include "TaskProcessFirmware.h"

extern int8_t Sim_sendCommand(char* command,char* response,uint32_t timeout);
extern void Sim_configGPRSnFTP(void);
extern void Sim_downloadFileFromFTP(void);
extern void display(void* data);
extern void Sim_Config(void);
extern void CheckSimConditionCommand(int index);
extern int processRespond(char* respond, int index);
extern void SIM_Reset(void);
extern void SIM_SendCommandAfterTimeout(void);
void SIM_RequestFirmwareCommand(int beginLine);
extern int ProcessFirmwareReceived(char* respond);
extern void clearDynamicCommand(char dynamicArray[]);



#endif