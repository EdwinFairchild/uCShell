#include "CL_bfp.h"
#include "CL_printMsg.h"


uint8_t CURRENT_NUM_OF_COMMANDS = 0; 
bool CLI_ACTIVE = true ; 
char BACKSPACE = 127;

//TODO: this external dependency on my own printMsg should be losely coupled 
//and made so the driver for this CLI is passed a print function with this
//prototype

//TODO: Make a good struct based driver and completely fix this parser

//TODO: verify how delete/backspace works and is it universal?
extern void CL_printMsg(char *msg, ...);

//internal helpers
static void _internal_cmd_command_list_handler(uint8_t num, char *values[]);


static void registerCommand(char *cmd, char delimeter, cmd_handler handler, char *help)
{
    //register a command at index matching current number of commands
    //this helps keep track of where to put the next registered command
    cmd_list[CURRENT_NUM_OF_COMMANDS].command = cmd;
    cmd_list[CURRENT_NUM_OF_COMMANDS].delimeter = delimeter;
    
    cmd_list[CURRENT_NUM_OF_COMMANDS].cmdHandler = handler;
	cmd_list[CURRENT_NUM_OF_COMMANDS].help = help;
    CURRENT_NUM_OF_COMMANDS++;

}
void CL_cli_init(CL_cli_type *cli)
{
//since structs 
	cli->registerCommand = registerCommand;
	cli->parseCommand = parseCMD;
	cli->parseChar = parseChar;
	cli->msgPtr = 0;

	//register command to show supported commands
	cli->registerCommand("?", ' ', _internal_cmd_command_list_handler, "Lists supported commands");


}
void parseChar(CL_cli_type *cli)
{
	/*	If cli.parsePending is already true then it means a message is still
		parsing , this current data will be ginored */
	if (cli->parsePending == false) 
	{
		/*
			 *	if the delimeter has been reached: 
			 *		--stop assembling the message 
			 *		--reset the cli pointer to point back to 
			 *		  beggining of cli.cliMsg 
			 *		--reset the pointer counter 
			 *		--set message received to true 
			 *	
			 */
 
		if (cli->charReceived == cli->delimeter || cli->charReceived == '\n') 
		{	
						
			//reset temp pointer countr
			cli->msgPtr = 0;    
				
			//this flag is used to let the application know we have a command to parse
			//do not parse anything in ISR
			cli->parsePending = true; 
								
		}	
		//if backspace is received the user wants to delete previous char receveid
		//so decrement the pointer so it points to previous char and set that char to null
		//and do not increment pointer, stay at that location
		else if(cli->charReceived == BACKSPACE) 
		{
			if (cli->msgPtr > 0)//make sure we can even decrement in the first place
			{
				cli->msgPtr--;
				cli->cliMsg[cli->msgPtr] = NULL; 
			}
		}
			
		/*	if we have NOT reached the delimiter or recevied a backspace then increment the pointer
		 *	so the next byte is stored at the next location hence assembling the message\
		 *	*/
		else 
		{
			cli->cliMsg[cli->msgPtr] = cli->charReceived; 
				
				
			if (cli->msgPtr < MESSAGE_MAX)
			{
				 
				cli->msgPtr++;  
			}			
		}
	}
}

void parseCMD(CL_cli_type *cli)
{ 

	
	
    char *token; 
    //used retreive the command and tokens via strtok 

    //will store the delimeter for a matching command
    char delimeter;
	//store found arguemtns
	char *tokens_found[10] = { NULL };

    bool matchFound = false; 
    //retreive just the command by setting the first delimeter of strtok to 
	//new line / enter / line feed etc...
    token = strtok(cli->cliMsg," \n \r");
   


//--------------------------------------| Strcmp based search |---------------------------------
	
    //search for the command in cmd_lists of registered commands
    int i ;
    for( i = 0 ; i < CURRENT_NUM_OF_COMMANDS; i++)
    {
       
        //if a command match occurs (strcmp returns zero on string match)
        if(! (strcmp(token,cmd_list[i].command) )  ) 
        {   
            //so we can break out of the for loop
            matchFound = true;
             
            //now go into the struct of matching command and use the given delimeter
            //to extract the rest of the parameters if any
            delimeter = cmd_list[i].delimeter ;

            //this is used to keep track of how many arguments were passed so we can tell the handler function
            uint8_t argumentCount = 0;

            //start going through the string tokeninzing and using the delimeter
            while(token != NULL)
            {
	            //get the next token
                token = strtok(NULL,&delimeter);
	            
	            //check if the first token is a "?" 
	            
	            
	            if (token == NULL) // if result is NULL "endof string" then we are done
		            {
                   
			            break;//outof while loop
		            }
	            else
	            {
		            //add the token to an array so we can send it to the handler
					//remember these are all pointers
					//to the original cliMsg
					tokens_found[argumentCount] = token;
					//increment argument counter so we can also tell the handler 
					//how many arguments to expect
					argumentCount++;
				}
	        }

	        //check if this is a request for the help msg
	        if (*tokens_found[0] == '?')
	        {
		        CL_printMsg("\r\n[HELP: %s] %s\r\n",cmd_list[i].command,  cmd_list[i].help);
		        CL_printMsg("%s", cli->prompt);
		        cli->parsePending = false;
		        break; 
	        } 
    
            //call the command handler for the specific command that was matched
	        //pass the number of tokens found as well as a list of the tokens
			CL_printMsg("\r\n");
            cmd_list[i].cmdHandler(argumentCount,tokens_found);
	        CL_printMsg("%s", cli->prompt);
	        
	       
	         
        }
	    if (matchFound == true)
	    {
		    //break out of for loop, no need to cycle through rest of commands
			 cli->parsePending = false;
		    break; 
	    }
	   
    }

	
	
	if (matchFound == false)
	{
		
		CL_printMsg("\r\n\"%s\" not found!\r\n", cli->cliMsg);
		CL_printMsg("%s", cli->prompt);
	}
		

	cli->parsePending = false;
    //clear buffer  to receive new messages and not have old text in there
    for (int i = 0; i < 50; i++)
        cli->cliMsg[i] =  NULL;
	
	//return pointer to handler function


}

static void _internal_cmd_command_list_handler(uint8_t num, char *values[])
{

	//get length of the longest command: used to add padding dashes so that output is aligned
	uint8_t len = 1;
	for(int i = 0 ; i < CURRENT_NUM_OF_COMMANDS; i++)
		{
			if(strlen(cmd_list[i].command) > len)
				len = strlen(cmd_list[i].command);
		}
	//print all commands
	for(int i = 0 ; i < CURRENT_NUM_OF_COMMANDS; i++)
	{

		    //print just the command
			CL_printMsg("[\033[91m%s\033[97m]",cmd_list[i].command);
			//add padding dashes if the len of this command is shorter than len
			for(int j = 0 ; j < (len-strlen(cmd_list[i].command)) ; j++)
				CL_printMsg("-");
			//print the help message
			CL_printMsg("| %s\r\n", cmd_list[i].help);
	}


}



void printRegister(uint32_t regVal)
{
	CL_printMsg("Register Value: 0x%X\n\r", regVal);
	for (int i = 31; i >=10; i--)
	{	
		CL_printMsg("| %d ",i);
	

	}
	for (int i = 9; i >= 0; i--)
	{	
		CL_printMsg("|  %d ", i);
	

	}

	CL_printMsg("\n\r");
	for(int i = 0 ; i < 160 ; i++)
		CL_printMsg("-");

	CL_printMsg("\n\r");
	for (int i = 31; i >= 0; i--)
	{	
	
		((regVal & (1 << i)) ? CL_printMsg("|  X ") : CL_printMsg("|    "));

	}
	CL_printMsg("\n\r");


}
