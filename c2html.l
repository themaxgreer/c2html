H               [a-fA-F0-9]
D		[0-9]
L		[a-zA-Z_]
%{
#include <stdio.h>
#include <string.h>

void printString(char* string);
enum {
  NUM = 257, IDENT,INT_CONST,FLOAT_CONST,
  STRING_LIT,CHAR_CONST,OP, KEYWORD, DIRECT, 
  NL, COMM
};
typedef union {
  float f;
  int i;
  char *s;
} YYSTYPE;

YYSTYPE yylval;
%}

%%
auto				{yylval.s = strdup(yytext);return KEYWORD;}
break				{yylval.s = strdup(yytext);return KEYWORD;}
case				{yylval.s = strdup(yytext);return KEYWORD;}
char				{yylval.s = strdup(yytext);return KEYWORD;}
const				{yylval.s = strdup(yytext);return KEYWORD;}
continue			{yylval.s = strdup(yytext);return KEYWORD;}
default				{yylval.s = strdup(yytext);return KEYWORD;}
do				{yylval.s = strdup(yytext);return KEYWORD;}
double				{yylval.s = strdup(yytext);return KEYWORD;}
else				{yylval.s = strdup(yytext);return KEYWORD;}
enum				{yylval.s = strdup(yytext);return KEYWORD;}
extern				{yylval.s = strdup(yytext);return KEYWORD;}
float				{yylval.s = strdup(yytext);return KEYWORD;}
for				{yylval.s = strdup(yytext);return KEYWORD;}
goto				{yylval.s = strdup(yytext);return KEYWORD;}
if				{yylval.s = strdup(yytext);return KEYWORD;}
int				{yylval.s = strdup(yytext);return KEYWORD;}
long				{yylval.s = strdup(yytext);return KEYWORD;}
register			{yylval.s = strdup(yytext);return KEYWORD;}
return				{yylval.s = strdup(yytext);return KEYWORD;}
short				{yylval.s = strdup(yytext);return KEYWORD;}
signed				{yylval.s = strdup(yytext);return KEYWORD;}
sizeof				{yylval.s = strdup(yytext);return KEYWORD;}
static				{yylval.s = strdup(yytext);return KEYWORD;}
struct				{yylval.s = strdup(yytext);return KEYWORD;}
switch				{yylval.s = strdup(yytext);return KEYWORD;}
typedef				{yylval.s = strdup(yytext);return KEYWORD;}
union				{yylval.s = strdup(yytext);return KEYWORD;}
unsigned			{yylval.s = strdup(yytext);return KEYWORD;}
void				{yylval.s = strdup(yytext);return KEYWORD;}
volatile			{yylval.s = strdup(yytext);return KEYWORD;}
while				{yylval.s = strdup(yytext);return KEYWORD;}

\"(\\.|[^\\"])*\"		{yylval.s = strdup(yytext);return STRING_LIT;}

'(\\.|[^\\'])+'			{yylval.s = strdup(yytext);return CHAR_CONST;}

0[0-7]*				{yylval.s = strdup(yytext);return INT_CONST;}
0[xX]{H}+[u|U|l|L]?		{yylval.s = strdup(yytext);return INT_CONST;}
{D}+(\.{D}*)?			{yylval.s = strdup(yytext);return INT_CONST;}	

{L}[a-zA-Z0-9_]* 		{yylval.s = strdup(yytext);return IDENT;}

{D}*"."{D}+([eE][+-]?{D}+)?(f|F|l|L)?	{yylval.s = strdup(yytext); return FLOAT_CONST;}
{D}+"."{D}*([eE][+-]?{D}+)?(f|F|l|L)?	{yylval.s = strdup(yytext); return FLOAT_CONST;}

[{}()[];,=?:|\^&<+-*/%!]	{yylval.s = strdup(yytext); return OP;}

[ \t\r\f]+             		{printf("%s", yytext); } 

#.*                    		{yylval.s = strdup(yytext); return DIRECT;}

\n                     		{yylineno++;return NL;}

"//".*				{yylval.s = strdup(yytext); return COMM;}
"/*"([^/]|[^*]\/)*"*/" 		{yylval.s = strdup(yytext); return COMM; }
%%

int main(int argc, char *argv[]) {
	int tok;
  	//test, it works
  	FILE *in = fopen( argv[1], "r" );

    	if (in == 0) printf("Could not open file\n");
  	yyin = in;  
  
	//initial html code
	printf("<html>\n");
  	printf("<head><title>%s</title></head>\n", argv[1]);
  	printf("<html>\n");
	printf("<body bgcolor=#FFFFFF>\n");
	printf("<pre>\n");
 	printf("<font color=#000000>\n%d\t</font>",yylineno);
	
	//switch statement for printing
  	while ((tok = yylex()) != 0) {
  	 switch(tok) {
   		case NUM: printf("%f", yylval.f); break;		
		case KEYWORD:		printf("<font color=#0000FF><b>");
					printString(yylval.s); printf("</b></font>"); 
					break;
              	case IDENT:		printf("<font color=#0000FF>");
					printString(yylval.s);printf("</font>"); 
					break;
   		case NL:		printf("<font color=#000000>\n%d\t</font>", yylineno); break;
		case OP:
		case DIRECT:		printf("<font color=#000000>");
					printString(yylval.s);printf("</font>"); break;
		case COMM:		printf("<font color=#00FF00>"); printString(yylval.s);
					printf("</font>"); break;
		case INT_CONST:
		case FLOAT_CONST:	printf("<font color=#00FFFF><i>"); printString(yylval.s);
					printf("</i></font>"); break;
		case STRING_LIT:	printf("<font color=#FF0000><i><u>");
					printString(yylval.s);printf("</u></i></font>"); break;
		case CHAR_CONST:	printf("<font color=#FF0000><i>"); 
					printString(yylval.s);printf("</i></font>"); break;
		default:printf("<font color=#FF0000><blink>%c</blink</font>",tok); break;
   	 }
  	}
	printf("</pre>\n");
	printf("</body>\n");
	printf("</html>\n");
  	return 0;
}
void printString(char* string){
	/*quick and dirty way to print out metacharacters
	* for html purposes, also handling newlines 
	* for multi-line comments  */ 
	int i;
	for(i=0;i<strlen(string);i++){
		if(string[i] == '<'){ printf("%s","&lt;");}
		else if(string[i] == '>') {printf("%s","&gt;");}
		else if(string[i] == '"') {printf("%s","&quot;");}
		else if(string[i] == '&') {printf("%s","&amp;");}
		else if(string[i] == '\n'){
			printf("<font color=#000000>\n%d\t</font>",++yylineno);	
		} else printf("%c",string[i]);
		
	}
}

