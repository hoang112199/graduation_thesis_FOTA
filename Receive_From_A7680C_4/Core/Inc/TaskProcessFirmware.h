#ifndef TASKPROCESSFIRMWARE
#define TASKPROCESSFIRMWARE
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "flash.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MaxLineDisplay 25
#define MaxLengthLine 44 // 44 because add LF to test, when in actual change this to 43
#define MinLengthLine 11
#define NumberOfCheckSumBits 2
#define RecordTypeData '0'
#define RecordTypeEndOfFile '1'
#define RecordTypeExtendedLinearAddress '4'
#define BitOfRecordType 8
#define BitOfStartCode 0
#define StartBitOfByteCount 1
#define StartBitOfAddress 3
#define StopBitOfAddress 7
#define StartBitOfData 9
#define StartBitOfHighAddress 9
#define StartPage 128
/* 65536 * 2 because of 1 byte has 2 characters hex and each hex takes 1 cell */
#define SizeOfBuffer 2048
/* Convert A,B,C,D to ABCD */
#define Add4(A, B, C, D) (A << 12) | (B << 8) | (C << 4) | D

#define FLASH_SECTOR_FOR_FIRM_1_1 FLASH_SECTOR_5
#define FLASH_SECTOR_FOR_FIRM_1_2 FLASH_SECTOR_6

typedef enum ERROR_CODE
{
	NoError = 0, \
	ErrorStartBit = 1, \
	ErrorHex = 2, \
	ErrorCheckSum = 3, \
	ErrorByteCount = 4, \
	ErrorLength = 5, \
	ErrorAddress = 6, \
}ErrorCode;

typedef enum WRITE_PAGE_ERROR_CODE
{
	WritePageNoError = 0, \
	WritePageError = 1, \
	UnindentifiedError = 2
}WritePageErrorCode;

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern uint8_t bitHigh[4];
extern unsigned char buffer[SizeOfBuffer];
extern uint8_t RxData[MaxLengthLine];

extern uint16_t  uchar_array_length;

extern uint8_t hexArray[SizeOfBuffer];
extern uint8_t hexPair[SizeOfBuffer/2];

/*******************************************************************************
 * API
 ******************************************************************************/
extern uint8_t ConvertToDec(uint8_t HexNumber);
extern bool CheckStartBit(uint8_t fLine[]);
extern bool CheckHex(uint8_t fLine[], uint8_t RxLength);
extern bool CheckSum(uint8_t fLine[], uint8_t RxLength);
extern bool CheckLength(uint8_t RxLength);
extern bool CheckByteCount(uint8_t fLine[], uint8_t RxLength);
extern ErrorCode CheckLineError(uint8_t fLine[], uint8_t RxLength);
//extern void ErasePage(uint32_t addr);
extern uint16_t FRead(uint32_t addr);
extern WritePageErrorCode WritePage(uint32_t size,uint32_t page);
extern void ClearBuffer();
extern WritePageErrorCode WriteLineToFlash(uint8_t fLine[], uint8_t RxLength);
extern void convertCharsToHex(unsigned char *uchar_array, int uchar_array_length, uint8_t *uint8_array);
extern void convertHexPairs(uint8_t *input_array, uint8_t *output_array, int input_array_length);
extern void deleteBuffer(char* buf);

extern int processTmpLine(char tmpLine[50]);

#endif