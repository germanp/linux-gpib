%{
#include <stdio.h>
#include <ib.h>
#undef EXTERN
#include <ibP.h>
#include <string.h>
#include <stdlib.h>

#define YYERROR_VERBOSE

ibConf_t ibFindConfigs[FIND_CONFIGS_LENGTH];
unsigned int findIndex = 0;
int bdid = 0;

%}

%union
{
int  ival;
char *sval;
char bval;
char cval;
}

%token T_INTERFACE T_DEVICE T_NAME T_MINOR T_BASE T_IRQ T_DMA
%token T_PAD T_SAD T_TIMO T_EOSBYTE
%token T_REOS T_BIN T_INIT_S T_DCL T_IFC
%token T_MASTER T_LLO T_DCL T_EXCL T_INIT_F T_AUTOPOLL

%token T_NUMBER T_STRING T_BOOL T_TIVAL
%type <ival> T_NUMBER
%type <ival> T_TIVAL
%type <sval> T_STRING
%type <bval> T_BOOL

%%

	input: /* empty */
		| device input
		| interface input
		| error
			{
				fprintf(stderr, "input error on line %i of %s\n", @1.first_line, DEFAULT_CONFIG_FILE);
				return -1;
			}
		;

	interface: T_INTERFACE '{' minor parameter '}'
			{
				ibFindConfigs[findIndex].is_interface = 1;
				if(++findIndex > FIND_CONFIGS_LENGTH)
				{
					fprintf(stderr, " too many devices in config file\n");
					return -1;
				}
			}
		;

	minor : T_MINOR '=' T_NUMBER {
				bdid = $3; ibFindConfigs[findIndex].board = $3;
				if(bdid < MAX_BOARDS)
					snprintf(ibBoard[bdid].device, sizeof(ibBoard[bdid].device), "/dev/gpib%i", bdid);
				else
					return -1;
			}
		;

	parameter: /* empty */
		| statement parameter
		| error
			{
				fprintf(stderr, "parameter error on line %i of %s\n", @1.first_line, DEFAULT_CONFIG_FILE);
				return -1;
			}
		;

	statement: T_PAD '=' T_NUMBER      { ibBoard[bdid].padsad |=  $3; ibFindConfigs[findIndex].padsad |= $3;}
		| T_SAD '=' T_NUMBER      { ibBoard[bdid].padsad |= ($3<<8); ibFindConfigs[findIndex].padsad |= ($3<<8);}
                 | T_EOSBYTE '=' T_NUMBER  { ibBoard[bdid].eos = $3; ibFindConfigs[findIndex].eos = $3;}
		| T_REOS T_BOOL           { ibBoard[bdid].eosflags |= $2 * REOS; ibFindConfigs[findIndex].eosflags |= $2 * REOS;}
                 | T_BIN  T_BOOL           { ibBoard[bdid].eosflags |= $2 * BIN; ibFindConfigs[findIndex].eosflags |= $2 * BIN;}
		| T_IFC  T_BOOL           { ibBoard[bdid].ifc = $2 ; ibFindConfigs[findIndex].flags |= CN_ISCNTL;}
		| T_TIMO '=' T_TIVAL      { ibBoard[bdid].timeout = $3; }
		| T_BASE '=' T_NUMBER     { ibBoard[bdid].base = $3; }
		| T_IRQ  '=' T_NUMBER     { ibBoard[bdid].irq = $3; }
		| T_DMA  '=' T_NUMBER     { ibBoard[bdid].dma = $3; }
		| T_NAME '=' T_STRING
			{
				strncpy(ibBoard[bdid].name, $3,
					sizeof(ibBoard[bdid].name));
				strncpy(ibFindConfigs[findIndex].name, $3,
					sizeof(ibFindConfigs[findIndex].name));
			}
		;

	device: T_DEVICE '{' option '}'
			{
				ibFindConfigs[findIndex].is_interface = 0;
				if(++findIndex > FIND_CONFIGS_LENGTH)
				{
					fprintf(stderr, "too many devices in config file\n");
					return -1;
				}
			}
		;

        option: /* empty */
	        | assign option
		| error
 			{
 				fprintf(stderr, "option error on line %i of %s\n", @1.first_line, DEFAULT_CONFIG_FILE);
				return -1;
			}
		;

	assign:
		T_PAD '=' T_NUMBER { ibFindConfigs[findIndex].padsad  |= $3; }
		| T_SAD '=' T_NUMBER { ibFindConfigs[findIndex].padsad |= ($3<<8); }
		| T_INIT_S '=' T_STRING { strncpy(ibFindConfigs[findIndex].init_string,$3,60); }
		| T_EOSBYTE '=' T_NUMBER  { ibFindConfigs[findIndex].eos = $3; }
		| T_REOS T_BOOL           { ibFindConfigs[findIndex].eosflags |= $2 * REOS;}
		| T_BIN  T_BOOL           { ibFindConfigs[findIndex].eosflags |= $2 * BIN; }
		| T_MASTER                { ibFindConfigs[findIndex].flags |= CN_ISCNTL; }
		| T_AUTOPOLL              { ibFindConfigs[findIndex].flags |= CN_AUTOPOLL; }
		| T_INIT_F '=' flags
		| T_NAME '=' T_STRING	{ strncpy(ibFindConfigs[findIndex].name,$3, sizeof(ibFindConfigs[findIndex].name));}
		| T_MINOR '=' T_NUMBER	{ ibFindConfigs[findIndex].board = $3;}
		;

	flags: /* empty */
		| ',' flags
	        | oneflag flags
		;

	oneflag: T_LLO       { ibFindConfigs[findIndex].flags |= CN_SLLO; }
		| T_DCL       { ibFindConfigs[findIndex].flags |= CN_SDCL; }
		| T_EXCL      { ibFindConfigs[findIndex].flags |= CN_EXCLUSIVE; }
		;

%%



void yyerror(char *s)
{
	fprintf(stderr, "%s\n", s);
}



