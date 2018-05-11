/* -*- C++ -*- */
/* File: syntax.lex
 * Author: mountcas, bilal, talmage
 * Version: $Id: syntax.lex,v 1.33 1999/02/19 21:22:48 marsh Exp $
 * Purpose: lexical analyzer for the FATE language.
 */

%x STRING VERBATIM

%{
#include <string.h>
#include <ctype.h>
#include <iostream.h>
#include <DS/util/String.h>

int yylineno = 1;
int recentlineno = 0;

char * recentline = NULL;

#define MAX_STR_CONST 4096
char string_buf[MAX_STR_CONST];
char *string_buf_ptr;

ds_String *verbatim = 0;

int get_id(char *input)
{
  char *in = input;
  /* format dec_num:dec_num:hex.hex.hex... .'s are optional */
  int next_pos = 2; /* advance after each pair of hex chars */
  bool right_nibble=false; /* begins with left */
  int hexx, expected, val = 0;
  char id_length = 0;
  char first_byte = 0;

  while (*input != ':') {
    val = val *10 + *input - '0';
    input++;
  }
  if (val > 160) {
    cout << "FATAL:  on line "<< yylineno
	 << ", nodeid peergroup len ["<< val <<"] exceeds 160"<<endl;
    exit (1);
  }
  id_length = val;
  input++; /* step over first : */
  
  val = 0;
  if (strchr(input, ':')) {
    expected = 22;	/* 22 zeros */
    yylval.stringval = strdup("0000000000000000000000");
    while (*input != ':') {
      val = val * 10 + *input - '0';
      input ++;
    }
    if (val > 160) {
      cout << "FATAL:  on line "<< yylineno
	   <<", nodeid address len ["<< val<<"] exceeds 160" << endl;
      exit (1);
    }
    first_byte = val;
    input++; /* step over second : */

#if 0
    yylval.stringval = new char[23];
    memset(yylval.stringval, 0, 23);
#endif
  } else { /* PeerID -- get two hex digits */
    expected = 14;	/* 14 zeros */
    yylval.stringval = strdup("00000000000000");
    val = isdigit(*input) ? *input - '0' : tolower(*input) - 'a' + 10;
    first_byte = (val << 4);
    input++;
    val = isdigit(*input) ? *input - '0' : tolower(*input) - 'a' + 10;
    first_byte |= val;
    input++;

#if 0
    yylval.stringval = new char[15];
    memset(yylval.stringval, 0, 15);
#endif
  }
  

#if 0
  yylval.stringval[0] = id_length;
  yylval.stringval[1] = first_byte;
#endif

  while (*input && next_pos < expected) {
    hexx = isdigit(*input) ? *input - '0' : tolower(*input) - 'a' + 10;
    if (right_nibble == false) {
#if 0
      yylval.stringval[next_pos] = (hexx << 4);
#endif
      right_nibble=true;
    } else {
#if 0
      yylval.stringval[next_pos++] |=  hexx;
#else
      next_pos++;
#endif
      right_nibble = false;
    }
    input++;
  }

  if ((next_pos < expected) || *input) {
    cout << "FATAL:  on line "<< yylineno
	 <<", insufficent hex data in id: "<< in << endl;
    exit(1);
  }

  yylval.stringval = strdup(in);

  if (expected == 22)
    return _nid;
  else
    return _Pid;
}

%}

%%

.*\n		{
			if (yylineno != recentlineno) {
			  if (recentline != NULL) free(recentline);
			  recentline = strdup(yytext);
			  recentlineno = yylineno;
			}
			REJECT;
		}

"configuration"	{ return _configuration; }
"begin"		{ return _begin; }
"end"		{ return _end; }
"call"		{ return _call; }

"("		{ return _lp; }
")"		{ return _rp; }
"{"		{ return _lc; }
"}"		{ return _rc; }
"["		{ return _lb; }
"]"		{ return _rb; }
"<"		{ return _lq; }
">"		{ return _rq; }

","		{ return _comma; }
"="		{ return _eq; }
";"		{ return _eos; }
"'"		{ return _quote; }
"\$"		{ return _dollar; }

"#include"	{ return _include; }
"/"		{ return _slash; }

"timeseq"	{ return _timeseq; }
"periodic"	{ return _periodic; }
"uniform"	{ return _uniform; }
"poisson"	{ return _poisson; }
"explicit"	{ return _explicit; }

"selector"	{ return _selector; }
"choose"	{ return _choose; }
"&&"		{ return _and; }
"||"		{ return _or; }
"!"		{ return _not; }

"group"		{ return _group; }
"+="		{ return _group_add; }
"-="		{ return _group_rem; }
"clear"		{ return _group_clr; }

"proc"		{ return _proc_; }
"procedure"	{ return _proc_; }

"forall"	{ return _forall; }
"in"		{ return _in; }
"not-in"	{ return _not_in; }
"if"		{ return _if; }
"else"		{ return _else; }

"switch"	{ return _switch; }
"link"		{ return _link; }
"birth"		{ return _birth; }
"death"		{ return _death; }
"suspend"	{ return _suspend; }
"resume"	{ return _resume; }

"inject"	{ return _inject; }
"<-"		{ return _with; }
"with"		{ return _with; }
"setup"         { return _setup; }

"snapshot"	{ return _snapshot; }
"snap"	        { return _snapshot; }

"exec"          { return _exec; }

"diag"          { return _diag; }

"ofstream"      { return _ofstream; }
"ifstream"      { return _ifstream; }

"plugin"	{ return _plugin; }
"library" 	{ return _library; }


"%{"		verbatim = new ds_String(); BEGIN(VERBATIM);

<VERBATIM>. 	{ *verbatim += yytext[0]; }
<VERBATIM>\n 	{ yylineno++;
		  *verbatim += yytext[0]; }

<VERBATIM>"%}" 		{ BEGIN(INITIAL); 
		  yylval.stringval = strdup(verbatim->chars());
		  delete verbatim;
		  return _verbatim;
		}

\"      string_buf_ptr = string_buf; BEGIN(STRING); /* quoted string code is from the flex(1) manual page. */

<STRING>\"	{ /* saw closing quote - all done */
                 BEGIN(INITIAL);
                 *string_buf_ptr = '\0';
                 /* return string constant token type and
                  * value to parser
                  */
		  yylval.stringval = strdup(string_buf);
		  return _quotedstring; 
                 }

<STRING>\n        {
                 /* error - unterminated string constant */
                 /* generate error message */
                 }

<STRING>\\[0-7]{1,3} {
                 /* octal escape sequence */
                 int result;

                 (void) sscanf( yytext + 1, "%o", &result );

                 if ( result > 0xff )
                         /* error, constant is out-of-bounds */

                 *string_buf_ptr++ = result;
                 }

<STRING>\\[0-9]+ {
                 /* generate error - bad escape sequence; something
                  * like '\48' or '\0777777'
                  */
                 }

<STRING>\\n  *string_buf_ptr++ = '\n';
<STRING>\\t  *string_buf_ptr++ = '\t';
<STRING>\\r  *string_buf_ptr++ = '\r';
<STRING>\\b  *string_buf_ptr++ = '\b';
<STRING>\\f  *string_buf_ptr++ = '\f';

<STRING>\\(.|\n)  *string_buf_ptr++ = yytext[1];

<STRING>[^\\\n\"]+        {
                 char *yptr = yytext;

                 while ( *yptr )
                         *string_buf_ptr++ = *yptr++;
                 }


[A-Za-z\/\.][A-Za-z0-9\_\-\.\/:]*	{ yylval.stringval = strdup(yytext);
					  return _string; }

[0-9]*:[0-9]*:[0-9a-fA-F]*      { return get_id(yytext); }

[0-9]*:[0-9a-fA-F]*             { return get_id(yytext); }

[0-9]*		{ yylval.numberval.digits = strdup(yytext);
		  yylval.numberval.intval = atoi(yytext);
		  yylval.numberval.doubleval = atof(yytext);
		  return _posint; 
		}

\-[0-9]* 	{ yylval.numberval.digits = strdup(yytext);
		  yylval.numberval.intval = atoi(yytext);
		  yylval.numberval.doubleval = atof(yytext);
		  return _negint; 
		}

\.[0-9]*	{ yylval.numberval.digits = strdup(yytext);
		  yylval.numberval.intval = atoi(yytext);
		  yylval.numberval.doubleval = atof(yytext);
		  return _posfp; 
		}

[0-9]*\.[0-9]*	{ yylval.numberval.digits = strdup(yytext);
		  yylval.numberval.intval = atoi(yytext);
		  yylval.numberval.doubleval = atof(yytext);
		  return _posfp;  
		}

\-\.[0-9]*	{ yylval.numberval.digits = strdup(yytext);
		  yylval.numberval.intval = atoi(yytext);
		  yylval.numberval.doubleval = atof(yytext);
		  return _negfp;  
		}

\-[0-9]*\.[0-9]*	{ yylval.numberval.digits = strdup(yytext);
			  yylval.numberval.intval = atoi(yytext);
			  yylval.numberval.doubleval = atof(yytext);
			  return _negfp;  
			}

\/\/.*$		{ ; }

\n              { yylineno++; }
[ \t]+\n	{ yylineno++; }

[ ] { ; }

[\t] { ; }

%%
