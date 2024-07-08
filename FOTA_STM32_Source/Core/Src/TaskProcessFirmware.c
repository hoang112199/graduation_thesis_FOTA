/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "TaskProcessFirmware.h"
/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Start Page */
int page_write = 0;
/* Process firmware variables */
volatile int indexLine = 0;
char fLineTmp[45];
volatile int isBetweenPlus = 0;
volatile int isEndFirmware = 0;
/* Hex pair buffer variables */
uint16_t currentBufferIndex = 0;
unsigned char buffer[SizeOfBuffer];
uint8_t hexArray[SizeOfBuffer];
uint8_t hexPair[SizeOfBuffer/2];
uint16_t  uchar_array_length = SizeOfBuffer;

/*******************************************************************************
 * Function
 ******************************************************************************/
uint8_t ConvertToDec(uint8_t HexNumber)
{
    if (HexNumber >= '0' && HexNumber <= '9')
    {
        HexNumber = HexNumber - '0';
    }
    else if (HexNumber >= 'A' && HexNumber <= 'F')
    {
        HexNumber = HexNumber - 'A' + 10;
    }
    else
    {
        HexNumber = HexNumber - 'a' + 10;
    }
    return HexNumber;
}

bool CheckStartBit(uint8_t fLine[])
{
    bool result = false;
    if (fLine[BitOfStartCode] == ':')
    {
        result = true;
    }
    else
    {
        /* Do nothing */
    }
    return result;
}

bool CheckHex(uint8_t fLine[], uint8_t RxLength)
{
    bool result = true;
    uint32_t index = 0;
    uint8_t length = RxLength;
    for (index = 1; index < length; index++)
    {
        if (ConvertToDec(fLine[index]) < 0 || ConvertToDec(fLine[index]) > 15)
        {
            result = false;
        }
        else
        {
            /* Do nothing */
        }
    }
    return result;
}

bool CheckSum(uint8_t fLine[], uint8_t RxLength)
{
    bool result = false;
    uint8_t index = 0;
    uint8_t length = RxLength;
    uint8_t value = 0;
    for (index = 1; index < length; index += 2)
    {
        if (index + 1 < length)
        {
            char cHexPair[3] = {fLine[index], fLine[index + 1], '\0'};
            value = value + strtol(cHexPair, NULL, 16);
        }
    }
    if (value % 256 == 0)
    {
        result = true;
    }
    else
    {
        /* Do nothing */
    }
    return result;
}

bool CheckLength(uint8_t RxLength)
{
	bool result = false;
	if(RxLength < MinLengthLine || RxLength > (MaxLengthLine - 1))
	{
		result = false;
	}
	else
	{
		result = true;
	}
	return result;
}

bool CheckByteCount(uint8_t fLine[], uint8_t RxLength)
{
	bool result = false;
	uint8_t byteCountArrayACSII[2] = {fLine[StartBitOfByteCount], fLine[StartBitOfByteCount + 1]};
	uint8_t byteCountArrayHex[2];
	uint8_t byteCountValue[1];
	convertCharsToHex(byteCountArrayACSII, 2, byteCountArrayHex);
  convertHexPairs(byteCountArrayHex, byteCountValue, 1);
	if(((RxLength - MinLengthLine) / 2) == byteCountValue[0])
	{
		result = true;
	}
	else
	{
		// Do nothing
	}
	return result;
}

ErrorCode CheckLineError(uint8_t fLine[], uint8_t RxLength)
{
    ErrorCode errorCode = NoError;
    if (CheckStartBit(fLine) == false)
    {
        errorCode = ErrorStartBit;
    }
		else if (CheckLength(RxLength) == false)
		{
				errorCode = ErrorLength;
		}
    else if (CheckHex(fLine, RxLength) == false)
    {
        errorCode = ErrorHex;
    }
    else if (CheckSum(fLine, RxLength) == false)
    {
        errorCode = ErrorCheckSum;
    }
		else if (CheckByteCount(fLine, RxLength) == false)
		{
				errorCode = ErrorByteCount;
		}
    else
    {
        errorCode = NoError;
    }
    return errorCode;
}

uint16_t FRead(uint32_t addr)
{
	uint16_t* val = (uint16_t *)addr;
	return *val;
}

uint8_t charToHexValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return 0;
    }
}

void convertCharsToHex(unsigned char *uchar_array, int uchar_array_length, uint8_t *uint8_array) {
    const uint8_t char_to_hex_map[256] = {
        ['0'] = 0x00, ['1'] = 0x01, ['2'] = 0x02, ['3'] = 0x03, ['4'] = 0x04, ['5'] = 0x05, ['6'] = 0x06, ['7'] = 0x07,
        ['8'] = 0x08, ['9'] = 0x09, ['A'] = 0x0A, ['B'] = 0x0B, ['C'] = 0x0C, ['D'] = 0x0D, ['E'] = 0x0E, ['F'] = 0x0F,
        ['a'] = 0x0A, ['b'] = 0x0B, ['c'] = 0x0C, ['d'] = 0x0D, ['e'] = 0x0E, ['f'] = 0x0F,
    };

    for (int i = 0; i < uchar_array_length; ++i) {
        uint8_array[i] = char_to_hex_map[uchar_array[i]];
    }
}

void convertHexPairs(uint8_t *input_array, uint8_t *output_array, int input_array_length) {
    for (int i = 0; i < input_array_length; i += 2) {
        output_array[i / 2] = (input_array[i] << 4) | (input_array[i + 1] & 0x0F);
    }
}

WritePageErrorCode WritePage(uint32_t size,uint32_t page)
{
  convertCharsToHex(buffer, uchar_array_length, hexArray);
  convertHexPairs(hexArray, hexPair, SizeOfBuffer);
	WritePageErrorCode errorWrite = WritePageNoError;
	uint32_t ADDRESS_PAGE = ((uint32_t)0x08000000 + (page*1024)); //calculate page address
	uint32_t addr = 0x00000000;
	uint16_t data = 0x00000000;

	// Must clear all flash flag to avoid error when processing first command
  // For example, in this case the first byte can't be write
	HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
  
	for(uint32_t i = 0; i < size / 2; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ADDRESS_PAGE + i, hexPair[i]);
	}
	HAL_FLASH_Lock();
	return WritePageNoError;
}

void ClearBuffer()
{
	for(int i=0;i<1024;i++)buffer[i]=255;
}

WritePageErrorCode WriteLineToFlash(uint8_t fLine[], uint8_t RxLength)
{
	ErrorCode error = NoError;
	WritePageErrorCode writeError = WritePageNoError;
	int index = 0;
	if(CheckLineError(fLine, RxLength) == NoError)
	{
		if(fLine[BitOfRecordType] == RecordTypeData)
		{
			// When receive uart - 1 for <LF>, using module sim don't have <LF>, remove  "-1"
			for(index = StartBitOfData; index < MaxLengthLine - NumberOfCheckSumBits - 1; index++)
			{
				buffer[currentBufferIndex] = fLine[index];
				
				if(currentBufferIndex == (SizeOfBuffer - 1))
				{
					if(WritePage(SizeOfBuffer,page_write) == WritePageNoError)
					{
						page_write++;
						ClearBuffer();
						currentBufferIndex = 0;
					}
					else
					{
						return WritePageError;
					}
				}
				else
				{
					currentBufferIndex++;
				}					
			}
		}
		if(fLine[BitOfRecordType] == RecordTypeEndOfFile)
		{
			if(WritePage(SizeOfBuffer,page_write) == WritePageNoError)
			{
				page_write++;
				ClearBuffer();
				currentBufferIndex = 0;
        isEndFirmware = 1;
			}
			else
			{
				return WritePageError;
			}
		}
	}
	else
	{
		return UnindentifiedError;
	}
	return writeError;
}

int processTmpLine(char tmpLine[50])
{
  int processFirmwareFlagError = 0;
  int index = 0;  
  if(tmpLine[0] == '+')
  {
    if(isBetweenPlus == 0)
    {
      isBetweenPlus = 1;
    }
    else
    {
      isBetweenPlus = 0;
    }
  }
  if(isBetweenPlus == 1)
  {
    if(tmpLine[0] == '+')
    {

    }
    else
    {
      
      if(tmpLine[0] ==  0x3A && indexLine > 0)
      {
        if(WriteLineToFlash((uint8_t*)fLineTmp, indexLine) == WritePageNoError)
        {                     
          
          deleteBuffer(fLineTmp);
          indexLine = 0;
        }
        else
        {
          processFirmwareFlagError = 1;
        }
      }
      while(tmpLine[index] != 0x0D)
      {
        fLineTmp[indexLine] = tmpLine[index];
        indexLine++;
        index++;
      }
      
      if(fLineTmp[BitOfRecordType] == RecordTypeEndOfFile)
      {
        processFirmwareFlagError = 2;
        uint8_t endCommand[] = {0x3A, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x46, 0x46}; // equal to ":00000001FF"
        if(WriteLineToFlash(endCommand, 11) == WritePageNoError)
        {                               
          deleteBuffer(fLineTmp);
          indexLine = 0;
        }
        else
        {
          processFirmwareFlagError = 1;
        }
        isBetweenPlus = 0;
      }
    }
  }
  if(processFirmwareFlagError == 1)
  {
    isBetweenPlus = 0;
  }
  return processFirmwareFlagError;
}

void deleteBuffer(char* buf)
{
	int len = strlen(buf);
	for(int i = 0; i < len; i++)
	{
		buf[i] = 0;
	}
}

void setStartPage(int startPage)
{
  page_write = startPage;
}