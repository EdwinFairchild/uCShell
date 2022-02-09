#ifndef uCSHELL_H
#define uCSHELL_H
/*
* Author:  Edwin Fairchild 2022
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
    cli.registerCommand("ledBlink", ';', cmd_ledBlink_handler, "Blinks user led x times with y delay\r\nusing the
following format : ledBlink x;y"); cli.registerCommand("getReg", ';', cmd_getreg_handler, "Returns the value of a given
register\n\r[Format] getreg GPIOA;hex or (bin)(dec)");


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

#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "uCShell_Config.h"
//---------------------| variables |-------------------
#define MAX_NUM_OF_COMMANDS 10
// maximum allowed characters in internal buffer
#define MESSAGE_MAX 50

// this function pointer will be used to point to the function handler
// for a given command
typedef void (*cmd_handler)(uint8_t num, char *values[]);

// this used to hold a command "object"
typedef struct _newCMD
{
    char *command;
    char delimeter;
    cmd_handler cmdHandler;
    const char *help;
    bool streamCommand;

} newCommand_type;

// used internally to point to the registerCommand function so that it can be
// called from within the struct...this is because of my personal taste to
// do everything from within the struct, could easily just have the user call
// the function and remove the static attribute
typedef void (*registerCmd)(char *cmd, char delimeter, cmd_handler handler, char *help, bool stream);

struct CL_cli_type;
typedef void (*parsecmd)(struct CL_cli_type *cli);
typedef void (*parsechar)(struct CL_cli_type *cli);
#if USING_WINDOWS
typedef int (*print_f)(const char *__restrict __fmt, ...);
#else
typedef void (*print_f)(char *msg, ...);
#endif
// control struct for the cli
typedef struct
{
    char *prompt;                // user define cli prompt
    char delimeter;              // user defined delimeter for end of command usually \r (enter key)
    registerCmd registerCommand; // function for user to register commands and handlers
    bool parsePending;           // used to let main app know a parse is pending
    parsecmd parseCommand;
    char charReceived; // stores received characters
    parsechar parseChar;

    char cliMsg[MESSAGE_MAX]; // stores the complete received message
    print_f print;
    uint8_t msgPtr; // this keeps track of how much we have increment the cli.cliMsg index

    // void(*parsecmd)(struct cliType *cli);
} CL_cli_type;

// internal struct to hold things I dont want exposed to the user
typedef struct
{
    char cliMsg[MESSAGE_MAX]; // stores the complete received message
    print_f print;
    uint8_t msgPtr; // this keeps track of how much we have increment the cli.cliMsg index
    bool stream;
} uCShell_type;

//---------------------| Prototypes |-----------------------------------

void parseCMD(CL_cli_type *cli);
void parseChar(CL_cli_type *cli);
void CL_cli_init(CL_cli_type *cli, char *prompt, print_f print_function);
void printRegister(uint32_t regVal);
void uCShell_run(CL_cli_type *cli);
bool is_uCShell_streaming(void);

#endif
