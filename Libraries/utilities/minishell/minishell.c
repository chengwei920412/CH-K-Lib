#include "minishell.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define CB_SIZE			64
#define MAX_ARGS		5
#define MAX_FUNCTION_NUM  (64)
char console_buffer[CB_SIZE];		/* console I/O buffer	*/
static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
static const char erase_seq[] = "\b \b";		/* erase sequence	*/
static const char   tab_seq[] = "        ";		/* used to expand TABs	*/
static MINISHELL_InstallTypeDef gInstall;
static MINISHELL_CommandTableTypeDef* gpSHELL_ExCmdTable[MAX_FUNCTION_NUM];
static uint32_t gNumOfFunctions = 0;

//SHELL Internal function
static int CommandFun_Help(int argc, char *argv[]);

static MINISHELL_CommandTableTypeDef SHELL_InFunTable[] =
{
    { "help", 1, CommandFun_Help ,"help" },
    { "list", 1, CommandFun_Help ,"help" },
};

static void SHELL_InsertFunction(void* pAddress)
{
    if(gNumOfFunctions >= MAX_FUNCTION_NUM)
    {
        return;
    }
    gpSHELL_ExCmdTable[gNumOfFunctions] = (MINISHELL_CommandTableTypeDef*) pAddress;
    gNumOfFunctions++;
}

void MINISHELL_Init(void)
{
    uint8_t i;
    for(i = 0;i < ARRAY_SIZE(SHELL_InFunTable); i++)
    {
        SHELL_InsertFunction((void*) &SHELL_InFunTable[i]);
    }
}

void MINISHELL_Install(MINISHELL_InstallTypeDef* pInstall)
{
    if((pInstall->ctrl_getchar == NULL) || (pInstall->ctrl_putchar == NULL))
		{
        return;
		}
    gInstall = *pInstall;
}

static uint8_t SHELL_Getc(void)
{
    return gInstall.ctrl_getchar();
}

static void SHELL_Putc(uint8_t ch)
{
    gInstall.ctrl_putchar(ch);
}

int MINISHELL_printf(const char *format,...)
{
    int chars;
    int i;
    va_list ap;
    char printbuffer[CB_SIZE];
    va_start(ap, format);
    chars = vsprintf(printbuffer, format, ap);
    va_end(ap);
    for(i=0;i<chars;i++)
    {
        SHELL_Putc(printbuffer[i]);
    }
    return chars ;
}

static int CommandFun_Help(int argc, char *argv[])
{
    uint32_t i = 0;
	  MINISHELL_printf("Available Commands:\r\n");
    //dispaly internal functions
    for(i = 0; i < gNumOfFunctions; i++)
    {
        MINISHELL_printf("name:%s |",     gpSHELL_ExCmdTable[i]->name);
        MINISHELL_printf("usage:%s \r\n", gpSHELL_ExCmdTable[i]->usage); 
    }
		return 0;
}

//! @brief MiniShell register user defined functions
void MINISHELL_Register(MINISHELL_CommandTableTypeDef* SHELL_CommandTableStruct, uint16_t NumberOfFunction)
{
    uint8_t i;
    if((SHELL_CommandTableStruct != NULL) && (NumberOfFunction != 0))
		{
        for(i = 0; i < NumberOfFunction; i++)
        {
            SHELL_InsertFunction(&SHELL_CommandTableStruct[i]);
        }
		}
}




static int readline_into_buffer(const char *const prompt, char * buffer)
{
    char *p = buffer;
    char * p_buf = p;
    int	n = 0;				/* buffer index		*/
    int	plen = 0;			/* prompt length	*/
    int	col;				/* output column cnt	*/
    char	c;

    //printf prompt
    if (prompt)
    {
        plen = strlen(prompt);
        MINISHELL_printf(prompt);
    }
    col = plen;
    for (;;) 
    {
        c = SHELL_Getc();
        //Special character handling
        switch (c) 
        {
            case '\r':				/* Enter		*/
            case '\n':
                *p = '\0';
                MINISHELL_printf("\r\n");
                return (p - p_buf);

            case '\0':				/* nul			*/
                continue;

            case 0x03:				/* ^C - break		*/
                p_buf[0] = '\0';	/* discard input */
                return (-1);

            case 0x15:				/* ^U - erase line	*/
                while (col > plen)
                {
                    MINISHELL_printf(erase_seq);
                    --col;
                }
                p = p_buf;
                n = 0;
                continue;

            case 0x17:				/* ^W - erase word	*/
                p = delete_char(p_buf, p, &col, &n, plen);
                while ((n > 0) && (*p != ' '))
                {
                    p = delete_char(p_buf, p, &col, &n, plen);
                }
                continue;

            case 0x08:				/* ^H  - backspace	*/
            case 0x7F:				/* DEL - backspace	*/
                p = delete_char(p_buf, p, &col, &n, plen);
                continue;

            default:
            //must be a normal character then 
            if (n < CB_SIZE - 2)
            {
                if (c == '\t') 
                {	/* expand TABs		*/
                    MINISHELL_printf(tab_seq + (col & 07));
                    col += 8 - (col & 07);
                }
					    	else 
					    	{
                    ++col;		/* echo input		*/
                    SHELL_Putc(c);
                }
                *p++ = c;
                ++n;
            } 
						else 
						{			/* Buffer full		*/
                SHELL_Putc('\a');
            }
        }
    }
}

static char *delete_char(char *buffer, char *p, int *colp, int *np, int plen)
{
    char *s;

    if (*np == 0) 
    {
        return (p);
    }

    if (*(--p) == '\t') 
    {			/* will retype the whole line	*/
        while (*colp > plen) 
        {
            MINISHELL_printf(erase_seq);
            (*colp)--;
        }
        for (s = buffer; s < p; ++s) 
				{
            if (*s == '\t')
            {
                MINISHELL_printf(tab_seq+((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            } 
            else
            {
                ++(*colp);
                SHELL_Putc(*s);
            }
        }
    } 
    else 
    {
        MINISHELL_printf(erase_seq);
        (*colp)--;
    }
    (*np)--;

    return (p);
}

static int parse_line(char *line, char *argv[])
{
    int nargs = 0;

#ifdef DEBUG_PARSER
	MINISHELL_printf ("parse_line: \"%s\"\n", line);
#endif
    while (nargs < MAX_ARGS)
    {
    
        /* skip any white space */
        while ((*line == ' ') || (*line == '\t')) 
        {
            ++line;
        }

        if (*line == '\0')
        {	/* end of line, no more args	*/
            argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		MINISHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
            return (nargs);
        }
     
        argv[nargs++] = line;	/* begin of argument string	*/

        /* find end of string */
        while (*line && (*line != ' ') && (*line != '\t'))
        {
            ++line;
        }

        if (*line == '\0')
        {	/* end of line, no more args	*/
            argv[nargs] = NULL;
#ifdef DEBUG_PARSER
    MINISHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
            return (nargs);
        }
        *line++ = '\0';		/* terminate current arg	 */
    }
    MINISHELL_printf ("** Too many args (max. %d) **\n", MAX_ARGS);
#ifdef DEBUG_PARSER
    MINISHELL_printf ("parse_line: nargs=%d\n", nargs);
#endif
    return (nargs);
}

static  MINISHELL_CommandTableTypeDef *find_cmd(const char *cmd, MINISHELL_CommandTableTypeDef *table, int table_len)
{
    MINISHELL_CommandTableTypeDef *cmdtp;
    MINISHELL_CommandTableTypeDef *cmdtp_temp = table;	/*Init value */
    int n_found = 0;

    for (cmdtp = table; cmdtp != table + table_len; cmdtp++)
    {
        if (strncmp(cmd, cmdtp->name, strlen(cmd)) == 0)
        {
            cmdtp_temp = cmdtp;	/* abbreviated command ? */
            n_found++;
        }
    }
    if (n_found == 1)			/* exactly one match */
    return cmdtp_temp;
    return NULL;	/* not found or ambiguous command */
}

/****************************************************************************
 * returns:
 *	1  - command executed, repeatable
 *	0  - command executed but not repeatable, interrupted commands are
 *	     always considered not repeatable
 *	-1 - not executed (unrecognized, bootd recursion or too many args)
 *           (If cmd is NULL or "" or longer than CONFIG_SYS_CBSIZE-1 it is
 *           considered unrecognized)
 *
 * WARNING:
 *
 * We must create a temporary copy of the command since the command we get
 * may be the result from getenv(), which returns a pointer directly to
 * the environment data, which may change magicly when the command we run
 * creates or modifies environment variables (like "bootp" does).
 */
int run_command(const char *cmd, int flag)
{
    uint8_t i;
	  uint8_t IsMatch = 0;
    MINISHELL_CommandTableTypeDef *cmdtp;
    char *argv[MAX_ARGS + 1];	/* NULL terminated	*/
    int argc;
    if (!cmd || !*cmd)
    {
        return -1;	/* empty command */
    }
    if (strlen(cmd) >= CB_SIZE)
    {
        MINISHELL_printf("## Command too long!\n");
        return -1;
    }
    /* Extract arguments */
    if ((argc = parse_line((char *)cmd, argv)) == 0)
    {
        return -1;
    }
    /* Look up command in command table */
    for(i=0;i<gNumOfFunctions;i++)
		{
        if(find_cmd(argv[0], gpSHELL_ExCmdTable[i], 1) != NULL)
        {
            cmdtp = gpSHELL_ExCmdTable[i];
            IsMatch = 1;
            break;
        }
		}
    if(IsMatch == 0)
    {
        MINISHELL_printf("Unknown command '%s' - try 'help'\r\n", argv[0]);
        return -1;	
		}
    /* found - check max args */
    if (argc > cmdtp->maxargs) 
    {
        return -1;
    }
    /* OK - call function to do the command */
    if ((cmdtp->cmd)(argc, argv) != 0)
    {
        return -1;
    }
    return 0;
}

void MINISHELL_CmdHandleLoop(char *name)
{
    static char lastcommand[CB_SIZE] = { 0 };
    int len;
    int rc = 1;
    int flag;

    for (;;) 
    {
        len = readline_into_buffer(name, console_buffer);
        flag = 0;	/* assume no special flags for now */
        if (len > 0)
        strcpy(lastcommand, console_buffer);
        if (len == -1)
            MINISHELL_printf("<INTERRUPT>\n");
        else
            rc = run_command(lastcommand, flag);
        if (rc <= 0)
        {
            /* invalid command or not repeatable, forget it */
            lastcommand[0] = 0;
        }
    }
}


