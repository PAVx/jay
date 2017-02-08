//Justin Lee: Feb 2nd, 2017
//Tianyi / Jason Zhu: Feb 3rd, 2017

#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include <string.h>
#include "softuart.h"
#include "xbee.h"
#include <util/delay.h>

// FILE *xbee_out = stdout;
// fprintf(xbee_out, "%d", x);

//char[pos]

char atniClose[] = "ATNI, CN";

char atnd[] = "ATND";

char cr = '\r';

int i;

//Enter into Command Mode
int AT_Init(void){
	UART_SendString("+++");
	_delay_ms(2000);
	// for(i = 0,i < 3, i++){
	// 	UART_SendString("+");
	// 	//fprintf(xbee_out, "+");
	// }
	return(1);
}

int getSignalStrength(void){
	AT_Init();
	hard_to_soft();

	UART_SendString("ATDB, CN\r");
	_delay_ms(1000);
	hard_to_soft();

	return(1);
}

int discoverMode(void){
	char availableDrone[256];
	char signalStrength[256];
	memset(signalStrength, '\0', sizeof(signalStrength));
	memset(availableDrone, '\0', sizeof(availableDrone));

	AT_Init();

	UART_SendString("ATND, CN\r");

	char serialBuff[256];
	
	int bufferItter = 0;
	int lineNum = 0;
	int nameItter = 0;
	int strengthItter = 0;

	softuart_puts("reset\n");

	for(bufferItter = 0; !(UART_IsEmpty()); bufferItter++){
		softuart_puts("for loop\n");
	    serialBuff[bufferItter] = UART_GetByte();
	    if(serialBuff[bufferItter] == 0x0D){
	    	softuart_puts("newln fnd\n");
	    	lineNum++;
	    }
	    if(lineNum == 3){
	    	softuart_puts("line 3\n");
	    	signalStrength[strengthItter] = serialBuff[bufferItter];
	    	strengthItter++;
	    }
	    if(lineNum == 4){
	    	softuart_puts("line 4\n");
	    	availableDrone[nameItter] = serialBuff[bufferItter];
	    	nameItter++;
	    }
	}

	softuart_puts(serialBuff);

	softuart_puts("NAME: ");
	//softuart_putchar('R');
	softuart_puts(availableDrone);
	softuart_puts("\nStrength: ");
	softuart_puts(signalStrength);

	// hard_to_soft();
	_delay_ms(2000);
	return(1);
}

//Return module name
int getModuleName(void){
	char moduleName[256];
	memset(moduleName, '\0', sizeof(moduleName));

	AT_Init();
	hard_to_soft();

	UART_SendString("ATNI, CN\r");

	_delay_ms(3000);

	hard_to_soft();	

	return(1);
}

int setModuleName(void){
	char okayResponse[2];

	AT_Init();

	memset(okayResponse, '\0', sizeof(okayResponse));
	okayResponse[0] = UART_GetByte();
	okayResponse[1] = UART_GetByte();

	UART_SendString("ATNI bagOfDicks, WR, CN\r");

	_delay_ms(300);

	hard_to_soft();
	// int i = 0;
	// for(i = 0; !(UART_IsEmpty()); i++){
	//     serialBuff[i] = UART_GetByte();
	// }

	// softuart_puts(serialBuff);

  	return(1);
}

int sendPacket(char *packet){
	UART_SendString(packet);
	return(1);
}

//--------------------HELPER FUNCTIONS---------------------------------------------


int charLength(char *input){
    int length = 0; 
    printf("%s", input);
    while(input[length++]!='\0');
    printf("%i", length);
    return(length);
 }

int xbeefprintf(char *xbeePrint){
	for(i = 0; i < charLength(xbeePrint);i++){
		// fprintf(xbee_out, "%c", xbeePrint[i]);

		UART_SendString(xbeePrint);
	}
	charReturn();
	return(1);
}

int charReturn(void){
	// fprintf(xbee_out, "\r");
	UART_SendByte('\r');
	return(1);
}

int hard_to_soft(void){
	char serialBuff[256];
	
	int i = 0;
	for(i = 0; !(UART_IsEmpty()); i++){
	    serialBuff[i] = UART_GetByte();
	}

	softuart_puts(serialBuff);
	return(1);

}

char * hard_to_memory(void){
	char *serialBuff = (char *) malloc(sizeof(char) * 256);
	
	int i = 0;
	for(i = 0; !(UART_IsEmpty()); i++){
	    serialBuff[i] = UART_GetByte();
	}

	return(serialBuff);
}

