#include "task_LTE.h"

#define FTPServer "27.66.240.9"
#define FTPPort "21"
#define Username "user"
#define Password "mt112199"
#define FileName "bootloader address 2.txt"
#define FilePath "C:/"

#define LengthOfEndLine 14 // Contain CR LF



volatile int isLineCompleted = 0;


/* FTP variables */
/*
char APNCommand[100] = {0};
char FTPCommand[100] = {0};
char FTPServer[50] = "27.66.240.9";
char FTPPort[6] = "21";
char Username[32] = "user";
char Pass[32] = "mt112199";
*/
/* Sim variables */
/*
char Sim_response[500] = {0};
char Sim_Rxdata[2] = {0};
int eof = 0;
*/
/* File variables */
/*
char fileName[30] = "bootloader address 2.txt";
char filePath[50] = "/public_html/DATA/";
*/
// volatile int commandIndex = 0;
/*
int8_t Sim_sendCommand(char* command,char* response,uint32_t timeout)
{
	uint8_t answer = 0, count  = 0;
	deleteBuffer(Sim_response);
	uint32_t time = HAL_GetTick();
	uint32_t time1 = HAL_GetTick();
	HAL_UART_Transmit(&huart1, (uint8_t *)command, (uint16_t)strlen(command), 1000);
	HAL_UART_Transmit(&huart1,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
	do
	{
    
		while(HAL_UART_Receive(&huart1, (uint8_t *)Sim_Rxdata, 1, 1000) != HAL_OK)
		{
			if(HAL_GetTick() - time > timeout) 
			{
				return 0;
			}
		}
    
    //HAL_UART_Receive_IT(&huart1, (uint8_t *)Sim_Rxdata, 1);
    time = HAL_GetTick();
		Sim_response[count++] = Sim_Rxdata[0];
		while((HAL_GetTick() - time < timeout))
		{
			if(HAL_UART_Receive(&huart1, (uint8_t *)Sim_Rxdata, 1, 1000) == HAL_OK)
			{
				Sim_response[count++] = Sim_Rxdata[0];
				time1 = HAL_GetTick();
			}
			else 
			{
				if(HAL_GetTick() - time1 > 100)
				{
					if(strstr(Sim_response,response) != NULL) 
					{
						answer = 1;
					}
					else 
					{
						answer = 0;
					}
					break;
				}
			}
		}
	}
	while(answer == 0);
	display(Sim_response);
	return answer;
}

void Sim_configGPRSnFTP(void)
{
	if(Sim_sendCommand("AT","OK",5000))
	{
		HAL_Delay(10);
		if(Sim_sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",5000))	//Configure GPRS
		{
			HAL_Delay(10);
			if(Sim_sendCommand("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",5000))			
			{
				deleteBuffer(APNCommand);
				HAL_Delay(10);
				strcpy(APNCommand,"AT+SAPBR=3,1,\"USER\",\"");	//Set username for APN
				strcat(APNCommand,Username);
				strcat(APNCommand,"\"");
				if(Sim_sendCommand(APNCommand,"OK",5000))
				{
					deleteBuffer(APNCommand);
					HAL_Delay(10);
					strcpy(APNCommand,"AT+SAPBR=3,1,\"PWD\",\"");		//Set password for APN
					strcat(APNCommand,Pass);
					strcat(APNCommand,"\"");
					if(Sim_sendCommand(APNCommand,"OK",5000))
					{
						deleteBuffer(APNCommand);
						HAL_Delay(10);
						while(Sim_sendCommand("AT+SAPBR=1,1","OK",5000) != 1);	//Open bearer									+ BO SUNG TIME OUT
						if(Sim_sendCommand("AT+FTPCID=1","OK",5000))		//Set FTP bearer profile identifier
						{
							strcpy(FTPCommand,"AT+FTPSERV=\"");	//Set FTP server address
							strcat(FTPCommand,FTPServer);
							strcat(FTPCommand,"\"");
							if(Sim_sendCommand(FTPCommand,"OK",5000))
							{
								deleteBuffer(FTPCommand);
								HAL_Delay(10);
								strcpy(FTPCommand,"AT+FTPPORT=");	//Set FTP server port
								strcat(FTPCommand,FTPPort);
								if(Sim_sendCommand(FTPCommand,"OK",5000))
								{
									deleteBuffer(FTPCommand);
									HAL_Delay(10);
									strcpy(FTPCommand,"AT+FTPUN=\"");	//Set FTP username
									strcat(FTPCommand,Username);
									strcat(FTPCommand,"\"");
									if(Sim_sendCommand(FTPCommand,"OK",5000))
									{
										deleteBuffer(FTPCommand);
										HAL_Delay(10);
										strcpy(FTPCommand,"AT+FTPPW=\"");	//Set FTP password
										strcat(FTPCommand,Pass);
										strcat(FTPCommand,"\"");
										if(Sim_sendCommand(FTPCommand,"OK",5000))
										{
											deleteBuffer(FTPCommand);
											HAL_Delay(10);
											display("Configuration done!\r\n");
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Sim_downloadFileFromFTP(void)
{
	deleteBuffer(FTPCommand);
	HAL_Delay(10);
	strcpy(FTPCommand,"AT+FTPGETNAME=\"");	//Get file name
	strcat(FTPCommand,fileName);
	strcat(FTPCommand,"\"");
	if(Sim_sendCommand(FTPCommand,"OK",5000))
	{
		deleteBuffer(FTPCommand);
		HAL_Delay(10);
		strcpy(FTPCommand,"AT+FTPGETPATH=\"");	//Get file path
		strcat(FTPCommand,filePath);
		strcat(FTPCommand,"\"");
		if(Sim_sendCommand(FTPCommand,"OK",5000))
		{
			deleteBuffer(FTPCommand);
			HAL_Delay(10);
			if(Sim_sendCommand("AT+FTPGET=1","+FTPGET: 1,1",30000))
			{
				eof = 0;
				while(eof == 0)
				{
					Sim_sendCommand("AT+FTPGET=2,20","+FTPGET: 2,",30000);
					if(strstr(Sim_response,"+FTPGET: 2,0") != NULL)
					{
						eof = 1;
					}
				}
			}
		}
	}
}
*/
void display(void* data)																								
{
	HAL_UART_Transmit(&huart6,(uint8_t *)data,(uint16_t)strlen(data),1000);
}

/*
void Sim_Config(void)
{
  if(Sim_sendCommand("AT","OK",1) == 1)
  {
    display("Configuration done!\r\n");
  }
  else
  {
    display("FAIL!\r\n");
  }
}
*/
void SendCommandToSim(char* command)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)command, (uint16_t)strlen(command), 1000);
	HAL_UART_Transmit(&huart1,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
  //HAL_TIM_Base_Start(&htim2);
}

void CheckSimConditionCommand(int index)
{
  //commandIndex = 1;
  char dynamicCommand[100];
  char* tmpPtr = dynamicCommand;
  switch(index)
  {
    case 0:
    {
      //SendCommandToSim("AT+CFTPSLIST=\"/\"");
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
      /*
      strcpy(dynamicCommand, "AT+FSDEL=\"");
      strcat(dynamicCommand, FilePath);
      strcat(dynamicCommand, FileName);
      strcat(dynamicCommand, "\"");
      SendCommandToSim(dynamicCommand);
      deleteBuffer(dynamicCommand);
      */
      SendCommandToSim("AT+FSDEL=\"C:/ProcessFirmwareF407.hex\"");
      //SendCommandToSim("AT");
      break;
    }
    case 6:
    {
      SendCommandToSim("AT+FSMEM");
      break;
    }
    case 7:
    {
      //SendCommandToSim("AT");
      SendCommandToSim("AT+CFTPSSTART");
      break;
    }
    case 8:
    {
      //SendCommandToSim("AT");
      SendCommandToSim("AT+CFTPSSINGLEIP=1");
      break;
    }
    case 9:
    {
      /*
      strcpy(dynamicCommand, "AT+CFTPSLOGIN=\"");
      strcat(dynamicCommand, FTPServer);
      strcat(dynamicCommand, "\"");
      strcat(dynamicCommand, ",");
      strcat(dynamicCommand, FTPPort);
      strcat(dynamicCommand, ",\"");
      strcat(dynamicCommand, Username);
      strcat(dynamicCommand, "\",");
      strcat(dynamicCommand, Password);
      strcat(dynamicCommand, "\",0");
      SendCommandToSim(dynamicCommand);
      deleteBuffer(dynamicCommand);
      */
      //SendCommandToSim("AT");
      SendCommandToSim("AT+CFTPSLOGIN=\"27.66.240.9\",21,\"user\", \"mt112199\",0");
      break;
    }
    case 10:
    {
      SendCommandToSim("AT+CFTPSLIST=\"/\"");
      break;
    }
    case 11:
    {
      /*
      strcpy(dynamicCommand, "AT+CFTPSGETFILE=\"");
      strcat(dynamicCommand, FileName);
      strcat(dynamicCommand, "\"");
      SendCommandToSim(dynamicCommand);
      deleteBuffer(dynamicCommand);
      */
      SendCommandToSim("AT+CFTPSGETFILE=\"C:/ProcessFirmwareF407.hex\"");
      
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
      //int length = strlen(respond);
    
    //char array[length + 1];
    
    //strcpy(array, respond);
      if(strstr(respond, expectRespond) != NULL)
      {
        respondStatus = 1;
      }
      else
      {
        respondStatus = 0;
      }
      
      //respondStatus = 1;
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
  char dynamicCommand[55];
  deleteBuffer(dynamicCommand);
  sprintf(dynamicCommand, "AT+CFTRANTX=\"C:/ProcessFirmwareF407.hex\",%d,512\r\n", beginLine);
  /*
  strcpy(dynamicCommand, "AT+CFTRANTX=\"C:/ProcessFirmwareF407.hex\",");
  strcat(dynamicCommand, (char*)beginLine);
  strcat(dynamicCommand, ",512");
  */
  HAL_UART_Transmit(&huart1, (uint8_t *)dynamicCommand, (uint16_t)strlen(dynamicCommand), 1000);
  //SendCommandToSim(dynamicCommand);
  //SendCommandToSim("AT+CFTRANTX=\"C:/ProcessFirmwareF407.hex\",0,512");
  //clearDynamicCommand(dynamicCommand);
}

int ProcessFirmwareReceived(char* respond)
{
  int processFirmwareFlagError = 1; // 0: No Error, 1: Error, 2: End process
  
  //char expectRespond[] = "AT+CFTRANTX";
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
