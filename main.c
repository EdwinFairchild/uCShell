/**
 * @file main.c
 * @author Eddie (eddie@edwinfairchild.com)
 * @brief This is a windows example of uCShell
 * @version 0.0.1
 * @date 2022-01-12
 * 
 * 
 * 
 */
#include "uCShell.h"
#include "uCShell_Config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

CL_cli_type cli;
//protoypes
void cmd_ok_handler(uint8_t num, char *values[]);

int main(void)
{
    cli.prompt = "eddie>";
    cli.delimeter = '\r';
    cli.print = printf;
    CL_cli_init(&cli);
    cli.registerCommand("ok", ' ', cmd_ok_handler, "Prints \"ok\" if cli is ok",false);
    cli.registerCommand("edwina", ' ', cmd_ok_handler, "Prints \"ok\" if cli is ok",true);
    char char_received ;
    while(1)
    {
        //TODO: some of the bugs could be scanf related
        //streaming on windows does not work
        if(!is_uCShell_streaming())
        scanf("%c",&char_received);
        
       
        //if the character receieved is not the delimeter then echo the character
        // if(cli.charReceived != cli.delimeter || cli.charReceived != '[' || cli.charReceived != ']')
        // {
        //     printf("%c", char_received);
        // }
        cli.charReceived = char_received;
        cli.parseChar(&cli);
        uCShell_run(&cli);
    }
   
    return 0;
}
void cmd_ok_handler(uint8_t num, char *values[])
{

		printf("System ok! \r\n");

}