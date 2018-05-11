/* -*- C++ -*- */
/* File: syntax.lex
 * Author: mountcas, bilal, talmage
 * Version: $Id: Configurator.lex,v 1.6 1999/02/24 20:37:32 talmage Exp $
 * Purpose: lexical analyzer for the ONE language.
 */

%x STRING

%{

#include <DS/containers/sortseq.h>
#include <DS/util/String.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/UNI40_e2e_trans_delay.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/UNI40_xqos.h>
#include <codec/uni_ie/qos_param.h>
#include <ctype.h>
#include <iostream.h>

#include <SwitchConfiguration.h>
#include "Configurator.yac.tab.h"

int PNNIConfiguratorlineno=1;
#if 0
int recentlineno=0;

char * recentline = NULL;
#endif

ds_String *theString = 0;

%}

%%

"{"		{ return _lc; }
"}"		{ return _rc; }

"a"		{ return _bbc_class_a; }
"abr"		{ return _abr; }
"acac"		{ return _acac; }
"acr"		{ return _acr; }
"aggregationtoken"	{ return _aggregationtoken; }
"aggregator"	{ return _aggregator; }
"atc"		{ return _bbc_atc; }
"backward"	{ return _backward; }
"basenodeid" 	{ return _basenodeid; }
"bbc"		{ return _uni_bbc; }
"c"		{ return _bbc_class_c; }
"call"		{ return _callconfiguration; }
"cbr"		{ return _cbr; }
"cbr_4"		{ return _cbr_4; }
"cbr_6"		{ return _cbr_6; }
"cbr_clr"	{ return _cbr_clr; }
"ccdv"		{ return _ccdv; }
"cdv"		{ return _cdv; }
"class" 	{ return _bbc_class; }
"clipping"	{ return _clipping; }
"clr"		{ return _clr; }
"clr_0"		{ return _clr_0; }
"clr_01"	{ return _clr_01; }
"connection" 	{ return _connection; }
"ctd"		{ return _ctd; }
"cumulative"	{ return _e2e_cumulative; }
"e2etd"		{ return _e2etd; }
"end"		{ return _end;}
"extqos"	{ return _extqos; }
"forward"	{ return _forward; }
"gcac_clp"	{ return _gcac_clp; }
"levels" 	{ return _switch_levels; }
"link"		{ return _link; }
"logos"		{ return _logos; }
"maximum"	{ return _e2e_maximum; }
"mbs_0"		{ return _mbs_0; }
"mbs_01"	{ return _mbs_01; }
"mcr"		{ return _mcr; }
"mcr_01"	{ return _mcr_01; }
"net_gen_ind"	{ return _e2e_net_gen_ind; }
"nrtvbr"	{ return _nrtvbr; }
"nrtvbr_0"	{ return _nrtvbr_0; }
"nrtvbr_2"	{ return _nrtvbr_2; }
"nrtvbr_8"	{ return _nrtvbr_8; }
"nrtvbr_clr"	{ return _nrtvbr_clr; }
"omni"		{ return _omni; }
"p2mp"		{ return _p2mp; }
"p2p"		{ return _p2p; }
"pcr_0"		{ return _pcr_0; }
"pcr_01"	{ return _pcr_01; }
"ports" 	{ return _switch_ports; }
"qos"		{ return _uni_qos; }
"rtvbr"		{ return _rtvbr; }
"rtvbr_1"	{ return _rtvbr_1; }
"rtvbr_clr"	{ return _rtvbr_clr; }
"scr_0"		{ return _scr_0; }
"scr_01"	{ return _scr_01; }
"switch"	{ return _switch; }
"tpc1"		{ return _tpc1; }
"tpc2"		{ return _tpc2; }
"tpc3"		{ return _tpc3; }
"tpc4"		{ return _tpc4; }
"tpc5"		{ return _tpc5; }
"tpc6"		{ return _tpc6; }
"tpc7"		{ return _tpc7; }
"tpc8"		{ return _tpc8; }
"trafficdescriptor"	{ return _trafficdescriptor; }
"ubr"		{ return _ubr; }
"vp"		{ return _bbc_class_vp; }
"weight"	{ return _weight; }
"x"		{ return _bbc_class_x; }

"true"		{ PNNIConfiguratorlval.boolean = true;
		  return _true;
		}

"false"		{ PNNIConfiguratorlval.boolean = false;
		  return _false;
		}




\"		theString = new ds_String(); BEGIN(STRING); /* quoted string code is from the flex(1) manual page. */

<STRING>\"	{ /* saw closing quote - all done */
  		  BEGIN(INITIAL);
                  /* return string constant token type and
                   * value to parser
                   */
		   PNNIConfiguratorlval.string = theString;
		   return _string_obj; 
		}

<STRING>\n        {
                 /* error - unterminated string constant */
                 /* generate error message */
                 }

<STRING>\\[0-7]{1,3} {
                 /* octal escape sequence */
                 int result;

                 (void) sscanf( PNNIConfiguratortext + 1, "%o", &result );

                 if ( result > 0xff )
                         /* error, constant is out-of-bounds */

		   *theString += (char)result;
                 }

<STRING>\\[0-9]+ {
                 /* generate error - bad escape sequence; something
                  * like '\48' or '\0777777'
                  */
                 }

<STRING>\\n  { static char *newline="\n"; *theString += newline; }
<STRING>\\t  { static char *tab="\t"; *theString += tab; }
<STRING>\\r  { static char *rtn="\r"; *theString += rtn; }
<STRING>\\b  { static char *backspace="\b"; *theString += backspace; }
<STRING>\\f  { static char *formfeed="\f"; *theString += formfeed; }

<STRING>\\(.|\n)  *theString += PNNIConfiguratortext[1];

<STRING>[^\\\n\"]+        {
                 char *yptr = PNNIConfiguratortext;

                 while ( *yptr )
                         *theString  += *yptr++;
                 }



[0-9]*:[0-9]*:[0-9a-fA-F]*      {
		  PNNIConfiguratorlval.nodeid = new NodeID(PNNIConfiguratortext);
		  return _nodeid;
		 }

[0-9]*:[0-9a-fA-F]*             {
		  PNNIConfiguratorlval.nodeid = new NodeID(PNNIConfiguratortext);
		  return _nodeid;
		}

[0-9]*		{ PNNIConfiguratorlval.integer = atoi(PNNIConfiguratortext);
		  return _integer; 
		}

\/\/.*$		{ ; }

\n              { PNNIConfiguratorlineno++; }
[ \t]+\n	{ PNNIConfiguratorlineno++; }

[ ] { ; }

[\t] { ; }

%%
