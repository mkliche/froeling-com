/*############################################################################
 Copyright:      Markus Kotschenreuther
 Author:         
 Remarks:
 known Problems: none
 Version:
 Description:
############################################################################*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <Arduino.h>

#include "timer.h"
#include "froeling_com.h"
#include "Logging.h"
//#include "SoftwareSerial.h"

//SoftwareSerial swSer(RX_PIN, TX_PIN);

//******************************************
// local variables
//******************************************
T_FROE_COM_DATA tFroeComStmData;
int froeling_proc_value_array[NB_OF_PROCESS_VALUES];
unsigned int froeling_time_array[6];

//******************************************
//forward declarations
//******************************************
void Send_Msg(char* msg);
int  ComSend(char* SendBuffer, int bytes2send);
void CreateCommand(char* cmd, char* Param, int ParamLen, char* sendbuffer);
void CreateCheckSum(char* buf, int buffsize, char* checksum);
int  checkCheckSum(char* buf, int buffsize);
void sendAcq(char* buf, int buffsize);
void sendNAcq(char* buf, int buffsize);
int convert_FroelingTime(char x);
unsigned long myTime = 0;

//---------------------------------------
// functions to control statemachine
//---------------------------------------
void FroeCom_Init (void){

	// init variables

	// initialise state machine
	tFroeComStmData.tCurrentMainState = STM_INIT;
	tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_HEADER;

	Serial.begin(9600);

};

//******************************************
//    STATEMACHINE
//******************************************
//---------------------------------------
int FroeComSTM(void)
{
	myTime = millis() / 1000;

	// Message Format:
	// Byte:	 1| 2| 3| 4| 5| 6  ...
	// --------------------------------------------
	// HEX:		4d|32|07|57|50|20|25|01|03|12|01|88
	// --------------------------------------------
	// ASCII:	 M| 2| ...
	// --------------------------------------------
	// Content: Ident| l|   Datablock        | CHS |
	// Blocks : Header  |   Body             | CHS |
	//
	// Byte 1,2:	Message identifier (M1 = measurement values / M2 = time)
	// Byte 3		Message data block length in bytes (l)
	// Byte 4...	Datablock
	// Last 2 Bytes:Checksum (CHS)

	// main statemachine
	// --------------------
	switch(tFroeComStmData.tCurrentMainState)
	{
		case STM_INIT:
		{
			// init variables
			tFroeComStmData.glsUserID = FROELING_USER_ID;
			memset(froeling_proc_value_array, 0, NB_OF_PROCESS_VALUES*sizeof(int));
			memset(froeling_time_array, 0, 6*sizeof(unsigned int));
			memset(&tFroeComStmData,0, sizeof(T_FROE_COM_DATA));

			tFroeComStmData.ulLastSystemTimestamp = myTime;
			// init and reset uart
			//@TODO uart_init();

			//@TODO uart_flush();
      Serial.flush();
			//@TODO uart_fifo_flush();

			// start communication by sendig "Ra" to P3100
      Log.debug("Starting communication by sendig \"Ra\" to P3100\r\n");
			Send_Msg((char*)"Ra");

			// next states
			tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_HEADER;
			tFroeComStmData.tCurrentMainState = STM_RECEIVE_MSG;

			break;
		}

		case STM_RECEIVE_MSG:
		{
			int iTmp = -1;

			// read one character from the input buffer (not blocking)
			//@TODO iTmp = uart_getc_nowait();
      iTmp = Serial.read();
			if (iTmp  > -1) {
				tFroeComStmData.cReceiveBuffer[tFroeComStmData.iInBuffPointer] = (unsigned char)iTmp;
				tFroeComStmData.iInBuffPointer++; 
				Log.debug("Received char: %c\r\n", (unsigned char)iTmp);
			}      

			// sub statemachine
			// -----------------------
			switch(tFroeComStmData.tCurrentSubState)
			{
				case STM_SUB_RECEIVE_HEADER:
				{
					if(tFroeComStmData.iInBuffPointer >= 3){
						// get parameter length
						tFroeComStmData.iMsgLength = (unsigned int)tFroeComStmData.cReceiveBuffer[2] + 5;
						if ((tFroeComStmData.iMsgLength) > RECEIVE_BUF_SIZE){
							tFroeComStmData.iInBuffPointer = 0;
							tFroeComStmData.iMsgLength = 0;
						}

						if (tFroeComStmData.cReceiveBuffer[0] == 'R'){
							// set next state
							tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_REPLY_BODY;
						}
						else if (tFroeComStmData.cReceiveBuffer[0] == 'M'){
							// set next state
							tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_MESSAGE_BODY;
						}
						else{
							tFroeComStmData.iInBuffPointer = 0;
						}
					}
					break;
				}

				case STM_SUB_RECEIVE_REPLY_BODY:
				{
					// wait until 6 bytes have been received
					if(tFroeComStmData.iInBuffPointer >= 6){

						tFroeComStmData.iInBuffPointer = 0;
						tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_HEADER;
					}
					break;
				}
				case STM_SUB_RECEIVE_MESSAGE_BODY:
				{
					// is telegram complete?
					if(tFroeComStmData.iInBuffPointer >= tFroeComStmData.iMsgLength){

						// check received checksum
						if(checkCheckSum(tFroeComStmData.cReceiveBuffer, tFroeComStmData.iMsgLength)){
							sendAcq(tFroeComStmData.cReceiveBuffer, tFroeComStmData.iMsgLength);
						}
						else{
							sendNAcq(tFroeComStmData.cReceiveBuffer, tFroeComStmData.iMsgLength);
						}

						// check if measurement values have been received (M1 messages)
						if(tFroeComStmData.cReceiveBuffer[1] == '1'){
							tFroeComStmData.tCurrentMainState = STM_HANDLE_M1;
						}

						// check if date/time has been received
						else if(tFroeComStmData.cReceiveBuffer[1] == '2'){
							tFroeComStmData.tCurrentMainState = STM_HANDLE_M2;
						}
						else
						{
							// next states
							tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_HEADER;
							tFroeComStmData.tCurrentMainState = STM_RECEIVE_MSG;
						}
						tFroeComStmData.iInBuffPointer = 0;
					}
					break;
				}
			}
			break;
			// End of sub statemachine
			// -----------------------
		}
		case STM_HANDLE_M1:
		{
			int i;
			int startindex = 3;
			short sValue = 0;

			for(i = 0; i < NB_OF_PROCESS_VALUES; i++)
			{
				sValue = (short)(((char)(tFroeComStmData.cReceiveBuffer[startindex + (i*2)]) << 8) + (char)tFroeComStmData.cReceiveBuffer[startindex + (i*2 + 1)]);

				// scale some values
				switch(i){
					case 3:
					case 14:
          case 15:
					case 17:
          case 18:
					case 19:
					case 20:
					case 21:
					case 22:
					case 23:
					//case 22:
					  { sValue = sValue / 2; break; }
					//case 11:{sValue = sValue / 207;break;}
				}

				// write scaled values to process value array
				froeling_proc_value_array[i] = sValue;
			}

			// next states
			tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_HEADER;
			tFroeComStmData.tCurrentMainState = STM_RECEIVE_MSG;
			break;
		}
		case STM_HANDLE_M2:
		{
			if(tFroeComStmData.cRbSendFlag == 0){
				// init measurement value transfer by sending "Rb" to P3100
				Send_Msg((char*)"Rb");
				tFroeComStmData.cRbSendFlag = 1;
			}

			// day
			froeling_time_array[0] = convert_FroelingTime(tFroeComStmData.cReceiveBuffer[6]);
			// month
			froeling_time_array[1] = convert_FroelingTime(tFroeComStmData.cReceiveBuffer[7]);
			// year
			froeling_time_array[2] = convert_FroelingTime(tFroeComStmData.cReceiveBuffer[9]);
			// min
			froeling_time_array[3] = convert_FroelingTime(tFroeComStmData.cReceiveBuffer[5]);
			// hour
			froeling_time_array[4] = convert_FroelingTime(tFroeComStmData.cReceiveBuffer[4]);
			// second
			froeling_time_array[5] = convert_FroelingTime(tFroeComStmData.cReceiveBuffer[3]);

			// next states
			tFroeComStmData.tCurrentSubState = STM_SUB_RECEIVE_HEADER;
			tFroeComStmData.tCurrentMainState = STM_RECEIVE_MSG;
			break;
		}
	}

	// communication watchdog
	if(myTime < tFroeComStmData.ulLastSystemTimestamp){
		// happends only at midnight...
		tFroeComStmData.ulLastSystemTimestamp = myTime;
	}
	else
	{
		if((myTime - tFroeComStmData.ulLastSystemTimestamp) >= ALIVE_CHECK_TIMESPAN){

			int iActFroelingTimestamp = (froeling_time_array[4] * 60) + froeling_time_array[5];

			// check if time sent by p3100 has changed since last check
			if(iActFroelingTimestamp == tFroeComStmData.iLastFroelingTimestamp){
				tFroeComStmData.tCurrentMainState = STM_INIT;
			}
			tFroeComStmData.iLastFroelingTimestamp = iActFroelingTimestamp;
			tFroeComStmData.ulLastSystemTimestamp = myTime;
		}
	}
	return(0);
}

/* ---------------------------------------------------
 * Construct and send a message in Frï¿½ling-Format
 * ---------------------------------------------------
 */
void Send_Msg(char* msg)
{
	char sendbuffer[8];
	char arrayUserID[3];
	char b[8];
	char cs[2];

	arrayUserID[0] = 0;
	arrayUserID[1] = (char)((FROELING_USER_ID >> 8) & 0xFF);
	arrayUserID[2] = (char)(FROELING_USER_ID & 0xFF);

	memset(b, 0,(8)*sizeof(char));

	b[0] = msg[0];
	b[1] = msg[1];
	b[2] = 3;
//  b[3] = 0;
//  b[4] = 0xff;
//  b[5] = 0xf9;

	memcpy(&b[3], arrayUserID, 3);
	CreateCheckSum(b, 6, cs);
	memcpy(&b[6], cs, 2);
	memcpy(sendbuffer, b, 8);

	ComSend(sendbuffer, 8);
	return;
}

/* ---------------------------------------------------
 * Create a Frï¿½ling Checksum
 * ---------------------------------------------------
 */
void CreateCheckSum(char* buf, int buffsize, char* checksum)
{
    unsigned short cs = 0;
    int i;

    for (i = 0; i < buffsize; i++){
        cs += (unsigned char)buf[i];
    }
    checksum[0] = (unsigned char)(cs >> 8);
    checksum[1] = (unsigned char) cs;
    return;
}

/* ---------------------------------------------------
 * Check if a checksum is correct
 * ---------------------------------------------------
 */
int checkCheckSum(char* buf, int buffsize)
{
	int i;
	int result = 0;
    unsigned short cs1 = 0;
    unsigned short cs2 = 0;

    // take last 2 bytes from received string (received checksum)
    cs1 = (unsigned short)(((unsigned char)buf[buffsize - 2] * 256) + (unsigned char)buf[buffsize - 1]);

    // build own checksum from received string
    for (i = 0; i < (buffsize - 2); i++){
        cs2 += (unsigned char)buf[i];
    }

    // compare both values
    if(cs1 == cs2){
    	result = 1;
    }
    return result;
}

/* ---------------------------------------------------
 * Send a Frï¿½ling acknowledge
 * ---------------------------------------------------
 */
void sendAcq(char* buf, int buffsize)
{
	char b[6];
	char cs[2];

    if (buffsize > 1)
    {
        b[0] = buf[0];
        b[1] = buf[1];
        b[2] = 1;
        b[3] = 1;
        b[4] = 0;
        b[5] = 0;
        CreateCheckSum(b, 6, cs);
        memcpy(&b[4], cs, 2);
    }
    ComSend(b, 6);
    return;
}

/* ---------------------------------------------------
 * Send a "Not acknowledge" to Frï¿½ling
 * ---------------------------------------------------
 */
void sendNAcq(char* buf, int buffsize)
{
	char b[6];
	char cs[2];

    if (buffsize > 1)
    {
        b[0] = buf[0];
        b[1] = buf[1];
        b[2] = 1;
        b[3] = 0;
        b[4] = 0;
        b[5] = 0;
        CreateCheckSum(b, 6, cs);
        memcpy(&b[4], cs, 2);
    }

    ComSend(b, 6);
    return;
}

/* ---------------------------------------------------
 * Send something via ComPort
 * ---------------------------------------------------
 */
int  ComSend(char* SendBuffer, int bytes2send)
{
	int i;

	for(i = 0; i < bytes2send; i++){
		//@TODO uart_putc( SendBuffer[i]);
    Serial.write((byte)SendBuffer[i]);
	}
	return(0);
}

/* ---------------------------------------------------
 * convert Frï¿½ling Date/Time segment to an integer value
 * Frï¿½ling interprets a Hex value directly "as it is written" as integer...
 * e.g. hour = 0x12 means hour = 12; minutes = 0x55 means minutes = 55
 * ---------------------------------------------------
 */
int convert_FroelingTime(char x)
{
	int y = 0;

	y = (int)(x>>4);
	y = (y*10) + (x-(y<<4));
	return(y);
}

