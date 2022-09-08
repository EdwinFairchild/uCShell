#include "uCShell.h"

#if USE_COLORS
#define _uCShell_print_prompt() uCShell.print("\033[33m%s\033[m", ptr_ucShell->prompt)
#define _print_prompt() ptr_ucShell->print("\033[33m%s\033[m", ptr_ucShell->prompt)
#else
#define _uCShell_print_prompt() uCShell.print("%s", ptr_ucShell->prompt)
#define _print_prompt() ptr_ucShell->print("%s", ptr_ucShell->prompt)
#endif
// will hold all commands and later be searched for a match
// when match is found the function in struct will be called
newCommand_type cmd_list[MAX_NUM_OF_COMMANDS];

// internal struct to hold things I dont want exposed to the user
typedef struct
{
    char ucshellMsg[MESSAGE_MAX]; // stores the complete received message
    print_f print;
    uint8_t msgPtr; // this keeps track of how much we have increment the
                    // cli.ucshellMsg index
    bool stream;
} prv_uCShell_type;

prv_uCShell_type uCShell;

uint8_t CURRENT_NUM_OF_COMMANDS = 0;
bool CLI_ACTIVE = true;
char BACKSPACE = 0x08;

// internal helpers
static void _internal_cmd_command_list_handler(uint8_t num, char *values[]);
static void cleanUp(uCShell_type *ptr_ucShell);
void printBanner(void);
cmd_handler stream_Handler_ptr = NULL;

static void registerCommand(char *cmd, char delimeter, cmd_handler handler, char *help, bool stream)
{
    // register a command at index matching current number of commands
    // this helps keep track of where to put the next registered command
    cmd_list[CURRENT_NUM_OF_COMMANDS].command = cmd;
    cmd_list[CURRENT_NUM_OF_COMMANDS].delimeter = delimeter;
    cmd_list[CURRENT_NUM_OF_COMMANDS].cmdHandler = handler;
    cmd_list[CURRENT_NUM_OF_COMMANDS].help = help;
    cmd_list[CURRENT_NUM_OF_COMMANDS].streamCommand = stream;
    CURRENT_NUM_OF_COMMANDS++;
}
void CL_cli_init(uCShell_type *ptr_ucShell, char *prompt, print_f print_function)
{
    // since structs
    ptr_ucShell->registerCommand = registerCommand;
    ptr_ucShell->parseCommand = parseCMD;
    ptr_ucShell->parseChar = parseChar;
    ptr_ucShell->delimeter = '\r';
    ptr_ucShell->print = print_function;
    ptr_ucShell->prompt = prompt;
    uCShell.msgPtr = 0;

    uCShell.print = print_function;
    uCShell.stream = false;
    // register command to show supported commands
    ptr_ucShell->registerCommand("?", ' ', _internal_cmd_command_list_handler, "Lists supported commands", false);
    printBanner();
    _uCShell_print_prompt();
}
void parseChar(uCShell_type *ptr_ucShell)
{
    /*	If ptr_ucShell.parsePending is already true then it means a message is
       still parsing , this current data will be ginored */
    if (ptr_ucShell->parsePending == false)
    {
        /*
         *	if the delimeter has been reached:
         *		--stop assembling the message
         *		--reset the cli pointer to point back to
         *		  beggining of uCShell.ucshellMsg
         *		--reset the pointer counter
         *		--set message received to true
         *
         */

        if (ptr_ucShell->charReceived == ptr_ucShell->delimeter || ptr_ucShell->charReceived == '\n')
        {
            // reset temp pointer countr
            uCShell.msgPtr = 0;

            // this flag is used to let the application know we have a command to
            // parse do not parse anything in ISR
            ptr_ucShell->parsePending = true;
        }
        // start stop delimeters for stream commands
        else if (ptr_ucShell->charReceived == '[' && stream_Handler_ptr != NULL)
        {
            uCShell.stream = true;
        }
        else if (ptr_ucShell->charReceived == ']')
        {
            uCShell.stream = false;
            uCShell.print("\r\n");
            cleanUp(ptr_ucShell);
            _uCShell_print_prompt();
        }
        // if backspace is received the user wants to delete previous char receveid
        // so decrement the pointer so it points to previous char and set that char
        // to null and do not increment pointer, stay at that location
        else if (ptr_ucShell->charReceived == BACKSPACE)
        {
            if (uCShell.msgPtr > 0) // make sure we can even decrement in the first place
            {
                uCShell.ucshellMsg[uCShell.msgPtr] = NULL;

                uCShell.msgPtr--;
                uCShell.ucshellMsg[uCShell.msgPtr] = NULL;
                uCShell.print("\x08 \x08");
            }
        }
/* tab , note this doesnt work in desktop environment */
#if USING_DESKTOP == 0
        else if (ptr_ucShell->charReceived == 0x09)
        {
            /* TOOD: show tab hints */
            printHint(ptr_ucShell);
        }
#endif

        /*	if we have NOT reached the delimiter or received a backspace then
         *increment the pointer
         *	so the next byte is stored at the next location hence assembling the
         *message\
         *	*/
        else
        {
            if (uCShell.msgPtr < MESSAGE_MAX)
            {
                uCShell.ucshellMsg[uCShell.msgPtr] = ptr_ucShell->charReceived;
                uCShell.msgPtr++;
                uCShell.print("%c", ptr_ucShell->charReceived);
            }
        }
    }
}

void printHint(uCShell_type *ptr_ucShell)
{

    bool matchFound = false;
    for (int i = 0; i < CURRENT_NUM_OF_COMMANDS; i++)
    {
        if ((memcmp(uCShell.ucshellMsg, cmd_list[i].command, strlen(uCShell.ucshellMsg))) == 0)
        {
            ptr_ucShell->print("\r\n>%s", cmd_list[i].command);
            matchFound = true;
        }
    }
    if (matchFound)
    {
        ptr_ucShell->print("\r\n");
        _print_prompt();
        ptr_ucShell->print("%s", uCShell.ucshellMsg);
    }
}

void parseCMD(uCShell_type *ptr_ucShell)
{
    char *token;
    // used retreive the command and tokens via strtok

    // will store the delimeter for a matching command
    char delimeter;
    // store found arguemtns
    char *tokens_found[10] = {NULL};

    bool matchFound = false;
    // retreive just the command by setting the first delimeter of strtok to
    // new line / enter / line feed etc...
    token = strtok(uCShell.ucshellMsg, " \n \r");
    if (token == NULL)
    {
        cleanUp(ptr_ucShell);
        _print_prompt();
        return;
    }
    //--------------------------------------| Strcmp based search
    //|---------------------------------

    // search for the command in cmd_lists of registered commands
    int i;
    for (i = 0; i < CURRENT_NUM_OF_COMMANDS; i++)
    {
        // if a command match occurs (strcmp returns zero on string match)
        if (!(strcmp(token, cmd_list[i].command)))
        {
            // so we can break out of the for loop
            matchFound = true;

            // now go into the struct of matching command and use the given
            // delimeter to extract the rest of the parameters if any
            delimeter = cmd_list[i].delimeter;

            // this is used to keep track of how many arguments were passed so we
            // can tell the handler function
            uint8_t argumentCount = 0;

            // start going through the string tokeninzing and using the delimeter
            while (token != NULL)
            {
                // get the next token
                token = strtok(NULL, &delimeter);

                // check if the first token is a "?"

                if (token == NULL) // if result is NULL "endof string" then we are done
                {

                    break; // outof while loop
                }
                else
                {
                    // add the token to an array so we can send it to the handler
                    // remember these are all pointers
                    // to the original ucshellMsg
                    tokens_found[argumentCount] = token;
                    // increment argument counter so we can also tell the handler
                    // how many arguments to expect
                    argumentCount++;
                }
            }

            // check if this is a request for the help msg
            // only if arguments is non-zero
            if (argumentCount && *tokens_found[0] == '?')
            {
                ptr_ucShell->print("\r\n[HELP: %s] %s\r\n", cmd_list[i].command, cmd_list[i].help);
                _print_prompt();
                ptr_ucShell->parsePending = false;
                break;
            }

            // call the command handler for the specific command that was matched
            // pass the number of tokens found as well as a list of the tokens
#if !USING_DESKTOP
            ptr_ucShell->print("\r\n");
#endif
            // check if command found is a stream command
            if (cmd_list[i].streamCommand == true)
            {
                uCShell.stream = true;
                stream_Handler_ptr = cmd_list[i].cmdHandler;
                stream_Handler_ptr(0, NULL);

                // check if the first token is a "?"

                if (token == NULL) // if result is NULL "endof string" then we are done
                {
                    break; // outof while loop
                }
                else
                {
                    // add the token to an array so we can send it to the handler
                    // remember these are all pointers
                    // to the original ucshellMsg
                    tokens_found[argumentCount] = token;
                    // increment argument counter so we can also tell the handler
                    // how many arguments to expect
                    argumentCount++;
                }
            }

            // check if this is a request for the help msg
            if (argumentCount && *tokens_found[0] == '?')
            {
                ptr_ucShell->print("\r\n[HELP: %s] %s\r\n", cmd_list[i].command, cmd_list[i].help);
                _print_prompt();
                ptr_ucShell->parsePending = false;
                break;
            }

            // call the command handler for the specific command that was matched
            // pass the number of tokens found as well as a list of the tokens
            ptr_ucShell->print("\r\n");
            // check if command found is a stream command
            if (cmd_list[i].streamCommand == true)
            {
                uCShell.stream = true;
                stream_Handler_ptr = cmd_list[i].cmdHandler;
                stream_Handler_ptr(0, NULL);
            }
            else // all other commands
            {
                cmd_list[i].cmdHandler(argumentCount, tokens_found);
            }
            _print_prompt();
        }
        // TODO: think of a cleaner way to handle this instead of this matchFound
        // flag
        if (matchFound == true)
        {
            // break out of for loop, no need to cycle through rest of commands
            ptr_ucShell->parsePending = false;
            break;
        }

    } // end of cycling through commands looking for match

    if (matchFound == false)
    {
        ptr_ucShell->print("\r\n\"%s\" not found!\r\n", uCShell.ucshellMsg);
        _print_prompt();
    }

    cleanUp(ptr_ucShell);

    // return pointer to handler function
}

static void cleanUp(uCShell_type *ptr_ucShell)
{
    ptr_ucShell->parsePending = false;
    // clear buffer  to receive new messages and not have old text in there
    for (int i = 0; i < MESSAGE_MAX; i++)
        uCShell.ucshellMsg[i] = NULL;
    uCShell.msgPtr = 0;
}
void uCShell_run(uCShell_type *ptr_ucShell)
{
    if (uCShell.stream == true)
    {
        // call stream function handler
        stream_Handler_ptr(0, NULL);
    }
    else if (ptr_ucShell->parsePending == true)
    {
        ptr_ucShell->parseCommand(ptr_ucShell);
    }
}
static void _internal_cmd_command_list_handler(uint8_t num, char *values[])
{
    // get length of the longest command: used to add padding dashes so that
    // output is aligned
    uint8_t len = 1;
    for (int i = 0; i < CURRENT_NUM_OF_COMMANDS; i++)
    {
        if (strlen(cmd_list[i].command) > len)
            len = strlen(cmd_list[i].command);
    }
    // print all commands
    for (int i = 0; i < CURRENT_NUM_OF_COMMANDS; i++)
    {
        // print just the command
#if USE_COLORS
        uCShell.print("[\033[91m%s\033[97m]", cmd_list[i].command);
#else
        uCShell.print("[%s]", cmd_list[i].command);
#endif
        // add padding dashes if the len of this command is shorter than len
        for (int j = 0; j < (len - strlen(cmd_list[i].command)); j++)
            uCShell.print("-");
        // print the help message
        uCShell.print("| %s\r\n", cmd_list[i].help);
    }
}

bool is_uCShell_streaming(void)
{
    return uCShell.stream;
}

void printBanner(void)
{
    uCShell.print("\r\n\r\n");
    uCShell.print("               .cxOOxc'.                              "
                  ".'lxOOxc.   \r\n");
    uCShell.print("              .kWMMMMMN0d;.                        "
                  ".;d0NMMMMMWk.  \r\n");
    uCShell.print("              ;XMMMMMMMMMWKxc'.                 "
                  "'ckKWMMMMMMMWKd.  \r\n");
    uCShell.print("              ;XMMMMMMMMMMMMMNOo;.          "
                  ".;o0NMMMMMMMMNOo;.    \r\n");
    uCShell.print("              ;XMMMMW0kKWMMMMMMMWKxc'     ;xKWMMMMMMMWKxc'  "
                  "  .   \r\n");
    uCShell.print("              ;XMMMMNl .;o0NMMMMMMMMNOoc;.,cxKWMMMN0d;.    "
                  ".cxk,  \r\n");
    uCShell.print("              ;XMMMMWd.   .'ckXWMMMMMMMMWKxc,':odc'.   "
                  ".,lOXWMX;  \r\n");
    uCShell.print("              ;XMMMMMWKd:.    .:d0NMMMMMMMMMNOo;.   "
                  ".;d0WMMMMMX;  \r\n");
    uCShell.print("              ;XMMMMMMMMWXkl,.   "
                  ".,lkXWMMMMMMMMWKx:'.;d0NMMMMNO'  \r\n");
    uCShell.print("              ;XMMMMMMMMMMMMN0d:.    "
                  ".:ld0NMMMMMMMWXOl;',lxkl,.   \r\n");
    uCShell.print("              ;XMMMMWXKWMMMMMMMWXkl,.    "
                  ".,lkXWMMMMMMMWKd:'.      \r\n");
    uCShell.print("              ;XMMMMWo.,oOXWMMMMMMMN0d:,.    "
                  ".:d0NMMMMMMMWXkl,.   \r\n");
    uCShell.print("              ;XMMMMWd.   'cxKWMMMMMMMWWXkl'.   "
                  ".,lOXWMMMMMMMNk'  \r\n");
    uCShell.print("              ;XMMMMMNOo;.   .;oONMMMMMMMMWN0d;.    "
                  ".cxKWMMMMMX;  \r\n");
    uCShell.print("              ,0WMMMMMMMWKxc'    'cxKWMMMMMMMMWKkc'.   "
                  ".;oONWW0,  \r\n");
    uCShell.print("               .:d0NMMMMMMMWXOo,.   .;oxONMMMMMMMMNOo;.    "
                  "':;.   \r\n");
    uCShell.print("                  .,lkXWMMMMMMMWKd:.     'cxKWMMMMMMMWKl.   "
                  "      \r\n");
    uCShell.print("                      .:d0NMMMMMMMWXkl,..   .;d0NMMN0d:.    "
                  "      \r\n");
    uCShell.print("                         .,lkXWMMMMMMMNX0d:.   .'cc,.       "
                  "      \r\n");
    uCShell.print("                             .:d0NMMMMMMMMWXx,              "
                  "      \r\n");
    uCShell.print("                                .,oOXWMMWXOl,.              "
                  "      \r\n");
    uCShell.print("                                    .;::;.                        "
                  "\r\n\r\n");
    uCShell.print("                        uCShell : Edwin Fairchild 2022\r\n\r\n");
}
