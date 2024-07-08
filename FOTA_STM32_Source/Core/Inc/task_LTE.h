#ifndef TASK_LTE
#define TASK_LTE
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "uart.h"
#include "string.h"
#include "TaskProcessFirmware.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FTPServer "27.66.247.225"
#define FTPPort "21"
#define Username "user"
#define Password "mt112199"
#define FileName "Receive_From_A7680C.hex"
#define FilePath "C:/"
#define LengthOfEndLine 14 // Contain CR LF
#define PhoneNumber "+84582387989"

/*******************************************************************************
 * API
 ******************************************************************************/
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
extern void sendSMSNotification(char* message);
extern void sendSuccessNotification();
extern void sendFailNotification();
extern void sendUserWarning();

#endif