#ifndef BFP_H
#define BFP_H
/*
* 
* 
* CL_cli_type cli; <-- must be global
* 
* 
* 	CL_cli_init(&cli);
	cli.prompt = "eddie>";
	cli.delimeter = '\r';



	cli.registerCommand("ok", ' ', cmd_ok_handler, "Prints \"ok\" if cli is ok");
	cli.registerCommand("add", '+', cmd_add_handler, "Add numbers with a + delimeter");
	cli.registerCommand("sub", '-', cmd_sub_handler, "Subtracts numbers with a - delimeter");
	cli.registerCommand("modx", ' ', cmd_test_var_handler, "Under construction");
	cli.registerCommand("ledOn", ' ', cmd_ledOn_handler, "Turns on user LED");
	cli.registerCommand("ledOff", ' ', cmd_ledOff_handler, "Turns of user LED");
	cli.registerCommand("ledBlink", ';', cmd_ledBlink_handler, "Blinks user led x times with y delay\r\nusing the following format : ledBlink x;y");
	cli.registerCommand("getReg", ';', cmd_getreg_handler, "Returns the value of a given register\n\r[Format] getreg GPIOA;hex or (bin)(dec)");
	

	while (1)
	{
	
		
		if (cli.parsePending == true)
		{
			cli.parseCommand(&cli);		
		}

       
	}
void USART1_IRQHandler(void)
{
	if ((USART1->SR & USART_SR_RXNE)) //if data has arrived on the uart
	{
		USART1->SR &= ~(USART_SR_RXNE);//clear interrupt

		//fetch data
		cli.charReceived = USART1->DR;

		//if the character receieved is not the delimeter then echo the character
		if(cli.charReceived != cli.delimeter)
			USART1->DR = cli.charReceived;
		cli.parseChar(&cli);
	}


}//----------------------------------------------------------------
void cmd_ok_handler(uint8_t num, char *values[])
{

		CL_printMsg("System ok! \r\n");

}
void cmd_add_handler(uint8_t num, char *values[])
{
	
	 num will have the number of arguments passed
	 and values is an array of strings with those arguments
	 in the order they were passed
	 
uint32_t sum = 0;

for (int i = 0; i < num; i++)
{
	sum += atoi(values[i]);

}
CL_printMsg("Sum: %d\r\n", sum);



}




*/
#include "CL_CONFIG.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"


//---------------------| variables |-------------------
#define MAX_NUM_OF_COMMANDS 10
#define MESSAGE_MAX 50

//this function pointer will be used to point to the function handler
//for a given command
typedef  void (*cmd_handler)(uint8_t num, char *values[]);

//this used to hold a command "object" 
 typedef struct _newCMD 
{    
    char            *command;
    char            delimeter ;
    cmd_handler     cmdHandler;
	const char      *help;

}newCommand_type;
  
//will hold all commands and later be searched for a match
//when match is found the function in struct will be called
newCommand_type cmd_list[MAX_NUM_OF_COMMANDS]; 


//used internally to point to the registerCommand function so that it can be
//called from within the struct...this is because of my personal taste to
//do everything from within the struct, could easily just have the user call
//the function and remove the static attribute 
typedef  void (*registerCmd)(char *cmd, char delimeter, cmd_handler handler, char *help);



struct CL_cli_type;
typedef void(*parsecmd)(struct CL_cli_type *cli);
typedef void(*parsechar)(struct CL_cli_type *cli);
//control struct for the cli
typedef struct  
{
	char *prompt;					//user define cli prompt
	char delimeter;					//user defined delimeter for end of command usually \r (enter key)
	bool parsePending;				//used to let main app know a parse is pending
	char charReceived;				//stores received characters
	char cliMsg[MESSAGE_MAX];		//stores the complete received message
	uint8_t msgPtr;					//this keeps track of how much we have incremtned the cli.cliMsg index
	registerCmd registerCommand;	//function for user to register commands and handlers

	//void(*parsecmd)(struct cliType *cli);
	parsecmd parseCommand;
	parsechar parseChar;

}CL_cli_type;








//---------------------| Prototypes |-----------------------------------
static void registerCommand(char *cmd, char delimeter, cmd_handler handler, char *help);
void parseCMD(CL_cli_type *cli);
void parseChar(CL_cli_type *cli);
void CL_cli_init(CL_cli_type *cli);
void printRegister(uint32_t regVal);




#endif