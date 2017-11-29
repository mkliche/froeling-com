/*############################################################################
 Copyright:      Markus Kot.
 Author:         
 Remarks:
 known Problems: none
 Version:
 Description:
############################################################################*/

#ifndef _HANDY_H_
 #define _HANDY_H_

//******************************************
// defines
//******************************************
#define FROELING_USER_ID 		-7
#define NB_OF_PROCESS_VALUES	27  // number of process values contained in M1 frames
#define ALIVE_CHECK_TIMESPAN   	30 // in seconds
#define RECEIVE_BUF_SIZE 		128

#define RX_PIN     5
#define TX_PIN     4

//******************************************
// typedefs
//******************************************
typedef enum {
	STM_INIT,
	STM_RECEIVE_MSG,
	STM_HANDLE_M1,
	STM_HANDLE_M2
}T_FROE_COM_STM;

typedef enum {
	STM_SUB_RECEIVE_HEADER,
	STM_SUB_RECEIVE_REPLY_BODY,
	STM_SUB_RECEIVE_MESSAGE_BODY,
}T_FROE_COM_SUBSTM;

typedef struct
{
	T_FROE_COM_STM   tCurrentMainState;
	T_FROE_COM_SUBSTM tCurrentSubState;
	char cReceiveBuffer[RECEIVE_BUF_SIZE];
	int iInBuffPointer;
	char cRbSendFlag;
	unsigned int  iMsgLength;
	int glsUserID;
	int iLastFroelingTimestamp;
	unsigned long ulLastSystemTimestamp;
}T_FROE_COM_DATA;

// variables
// ---------------------------------------
extern T_FROE_COM_DATA tFroeComStmData;

// defines
// ---------------------------------------
#define USR UCSRA
#define UCR UCSRB
#define UBRR UBRRL
#define EICR EICRB

// function prototypes
// ---------------------------------------
extern void FroeCom_Init(void);
extern int FroeComSTM(void);

// variables
// ---------------------------------------
extern int froeling_proc_value_array[];
extern unsigned int froeling_time_array[];

#endif //_HANDY_H_


