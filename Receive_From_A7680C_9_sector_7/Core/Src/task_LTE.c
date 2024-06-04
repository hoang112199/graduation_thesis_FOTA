/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "task_LTE.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FTPServer "27.66.254.95"
#define FTPPort "21"
#define Username "user"
#define Password "mt112199"
#define FileName "Receive_From_A7680C.hex"
#define FilePath "C:/"
#define LengthOfEndLine 14 // Contain CR LF

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile int isLineCompleted = 0;

/*******************************************************************************
 * Function
 ******************************************************************************/
void display(void* data)																								
{
	HAL_UART_Transmit(&huart6,(uint8_t *)data,(uint16_t)strlen(data),1000);
}

void SendCommandToSim(char* command)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)command, (uint16_t)strlen(command), 1000);
	HAL_UART_Transmit(&huart1,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
}

void CheckSimConditionCommand(int index)
{
  char dynamicCommand[100];
  char* tmpPtr = dynamicCommand;
  switch(index)
  {
    case 0:
    {
      SendCommandToSim("AT");
      break;
    }
    case 1:
    {
      SendCommandToSim("AT+CPIN?");
      break;
    }
    case 2:
    {
      SendCommandToSim("AT+CSQ");
      break;
    }
    case 3:
    {
      SendCommandToSim("AT+CGREG?");
      break;
    }
    case 4:
    {
      SendCommandToSim("AT+COPS?");
      break;     
    }
    case 5:
    {
      //SendCommandToSim("AT+FSDEL=\"Receive_From_A7680C.hex\"");
      deleteBuffer(dynamicCommand);
      sprintf(dynamicCommand, "AT+FSDEL=\"%s\"\r\n", FileName);
      HAL_UART_Transmit(&huart1, (uint8_t *)dynamicCommand, (uint16_t)strlen(dynamicCommand), 1000);
      break;
    }
    case 6:
    {
      SendCommandToSim("AT+FSMEM");
      break;
    }
    case 7:
    {
      SendCommandToSim("AT+CFTPSSTART");
      break;
    }
    case 8:
    {
      SendCommandToSim("AT+CFTPSSINGLEIP=1");
      break;
    }
    case 9:
    {
      //SendCommandToSim("AT+CFTPSLOGIN=\"27.66.254.95\",21,\"user\", \"mt112199\",0");
      deleteBuffer(dynamicCommand);
      sprintf(dynamicCommand, "AT+CFTPSLOGIN=\"%s\",21,\"%s\",\"%s\",0\r\n", FTPServer, Username, Password);
      HAL_UART_Transmit(&huart1, (uint8_t *)dynamicCommand, (uint16_t)strlen(dynamicCommand), 1000);
      break;
    }
    case 10:
    {
      SendCommandToSim("AT+CFTPSLIST=\"/\"");
      break;
    }
    case 11:
    {
      //SendCommandToSim("AT+CFTPSGETFILE=\"Receive_From_A7680C.hex\"");
      deleteBuffer(dynamicCommand);
      sprintf(dynamicCommand, "AT+CFTPSGETFILE=\"%s\"\r\n", FileName);
      HAL_UART_Transmit(&huart1, (uint8_t *)dynamicCommand, (uint16_t)strlen(dynamicCommand), 1000);
      break;
    } 
    case 12:
    {
      SendCommandToSim("AT+FSLS");
      break;
    }
    case 13:
    {
      SendCommandToSim("AT+CFTPSLOGOUT");
      break;
    }
    case 14:
    {
      SendCommandToSim("AT+CFTPSSTOP");
      break;
    }
    default:
    {
      break;
    }
  }   
}

int processRespond(char* respond, int index)
{
  int respondStatus = 0; // 0 if error, 1 if not
  //process here
  switch(index)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 7:
    case 8:
    case 9:
    case 11:
    case 13:
    case 14:
    {     
      char expectRespond[] = "OK";
      if(strstr(respond, expectRespond) != NULL)
      {
        respondStatus = 1;
      }
      else
      {
        respondStatus = 0;
      }
      break;
    }    
    case 5:
    case 10:
    case 12:
    {
      respondStatus = 1;
      break;
    }
    default:
    {
      break;
    }
  }
  return respondStatus;
}

void SIM_Reset(void)
{
  SendCommandToSim("AT+CRESET");
}

void SIM_SendCommandAfterTimeout(void)
{
  SendCommandToSim("AT+CFTPSLOGOUT");
  HAL_Delay(1000);
  SendCommandToSim("AT+CFTPSSTOP");
}

void SIM_RequestFirmwareCommand(int beginLine)
{
  char dynamicCommand[70];
  deleteBuffer(dynamicCommand);
  //sprintf(dynamicCommand, "AT+CFTRANTX=\"C:/Receive_From_A7680C.hex\",%d,512\r\n", beginLine);
  sprintf(dynamicCommand, "AT+CFTRANTX=\"C:/%s\",%d,512\r\n", FileName, beginLine);
  HAL_UART_Transmit(&huart1, (uint8_t *)dynamicCommand, (uint16_t)strlen(dynamicCommand), 1000);
}

int ProcessFirmwareReceived(char* respond)
{
  int processFirmwareFlagError = 1; // 0: No Error, 1: Error, 2: End process
  char expectRespond[] = "+CFTRANTX: DATA,";
  if(strstr(respond, expectRespond) == NULL)
  {
    return 3; // received outside AT+CFTRANTX command 
  }
  char header[] = "+CFTRANTX: DATA,";
  char* dataAfterHeader = strstr(respond, header);
  char tmpLine[50]; // debug cho de xong xoa tranh hard fault
  
  int isATCommand = 0; // loai bo du thua sau OK
  int processFirstLine = 0; // loai bo dong dau
  char *token = strtok(dataAfterHeader, "\n");
  while (token != NULL) {
    deleteBuffer(tmpLine);
    strcpy(tmpLine, token);
    processFirmwareFlagError = processTmpLine(tmpLine);
    if(processFirmwareFlagError != 0)
    {
      break;
    }
    token = strtok(NULL, "\n");
  } 
  free(dataAfterHeader);
  free(token);
  return processFirmwareFlagError;
}

void clearDynamicCommand(char dynamicArray[])
{
  for(int i = 0; i < 99; i++)
	{
		dynamicArray[i] = '\0';
	}
}
