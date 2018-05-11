// -*- C++ -*-
// +++++++++++++++
//   P R o u S T     ---  PNNI Routing Simulation Toolkit  ---
// +++++++++++++++
// Version: 1.0 (Fri Mar  5 14:35:33 EST 1999)
// 
// 			  Copyright (c) 1998
// 		 Naval Research Laboratory (NRL/CCS)
// 			       and the
// 	  Defense Advanced Research Projects Agency (DARPA)
// 
// 			 All Rights Reserved.
// 
// Permission to use, copy, and modify this software and its
// documentation is hereby granted, provided that both the copyright notice and
// this permission notice appear in all copies of the software, derivative
// works or modified versions, and any portions thereof, and that both notices
// appear in supporting documentation.
// 
// NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
// DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM
// THE USE OF THIS SOFTWARE.
// 
// NRL and DARPA request users of this software to return modifications,
// improvements or extensions that they make to:
// 
//                 proust-dev@cmf.nrl.navy.mil
//                         -or-
//                Naval Research Laboratory, Code 5590
//                Center for Computation Science
//                Washington, D.C.  20375
// 
// and grant NRL and DARPA the rights to redistribute these changes in
// future upgrades.
//

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#ifndef LINT
static char const _functions_h_rcsid_[] =
"$Id: functions.h,v 1.53 1999/02/17 21:24:23 mountcas Exp $";
#endif

extern "C" {
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
};

//-----------------------------------------------------------------------------
void bind_name_globally(char* s, int code)
{
  string str(s);
  if (v_global_binding.lookup(str)){
    sprintf(v_temp,"There are two global declarations of variable '%s'",s);
    yyerror(v_temp);
  } else {
    v_global_binding.insert(strdup(s), code);
  }
}

//-----------------------------------------------------------------------------
void bind_name_locally(char* s, int code)
{
  string str(s);
  if (v_local_binding.lookup(str)){
    sprintf(v_temp,"There are two local declarations of variable '%s' in procedure %s",s,v_proc_name);
    yyerror(v_temp);
  } else {
    v_local_binding.insert(strdup(s),code);
  }
}

//-----------------------------------------------------------------------------
int is_globally_defined(char* s)
{
  string str(s);
  dic_item di;
  if (di = v_global_binding.lookup(str))
    return (v_global_binding.inf(di));
  else
    return 0;
}

//---------------------------------------------------------------------------
int is_locally_defined(char* s)
{
  string str(s);
  dic_item di;
  if (di = v_local_binding.lookup(str))
    return (v_local_binding.inf(di));
  else
    return 0;
}


//-----------------------------------------------------------------------------
void dump_initial(void)
{
  // is there some way to get a ptr to the clock?
  time_t clk;
  time(&clk);

  fprintf(v_output, 
	  "// -----------------------------------------------------------------------------\n"
	  "// PRouST Simulation Scenario\n"
	  "// Created: %s"
	  "// -----------------------------------------------------------------------------\n\n"
	  "#include <common/cprototypes.h>\n"
	  "#include <iostream.h>\n"
	  "#include <FW/kernel/Kernel.h>\n"
	  "#include <FW/basics/diag.h>\n"
	  "#include <fsm/config/Configurator.h>\n"
	  "#include <sim/fate/cronos.h>\n"
	  "#include <sim/plugin/Plugin.h>\n"
	  "#include <fsm/omni/OmniProxy.h>\n\n"
	  "CompMaker internal_maker;\n\n",
	  ctime(&clk), v_configFileName);

  fprintf(v_output, "int internal_argc;\n");
  fprintf(v_output, "char **internal_argv;\n");
}

//-----------------------------------------------------------------------------
void dump_final(void)
{
  fprintf(v_output, "//-----------------------------------------------------------------------------\n"
	  "void cronos_global_init(void) {\n"
	  "%s}\n", v_global_init);
  fprintf(v_output, "\n\n\n//-----------------------------------------------------------------------------\n"
	  "int main(int argc, char ** argv)\n{\n"
	  "  char filename[80];\n"
	  "  internal_argc = argc;\n"
	  "  internal_argv = argv;\n"
	  "  strcpy(filename, \"/dev/null\");\n\n"
	  "  if (argc < 2) {\n"
	  "    cerr << \"Usage: \" << argv[0] << \" [filename [config_file]]\" << endl;\n"
	  "  } else {\n"
	  "    strcpy(filename, argv[1]);\n"
	  "    cout << \"Output will be logged in \" << argv[1] << endl;\n"
	  "  }\n\n"
	  "  loadDiagPrefs();\n\n"
	  "  char nsc_buf[64];\n"
	  "  sprintf(nsc_buf, \"%%s.OMNI\", argv[0]);\n"
	  "  theOmniProxy().setOutput( nsc_buf );\n\n"
	  "  cronos_global_init();\n"
	  "  Kernel & globalKernel = theKernel();\n"
	  "  const Configurator &globalConfigurator ="
	  " (argc < 3) ? theConfigurator(\"%s\") : theConfigurator(argv[2]);\n"
	  "  DefaultLinkConfig();\n"
	  "  VisPipe(filename);\n"
	  "  globalKernel.Run();\n\n"
	  "  return 0;\n}\n", v_configFileName);
}

//-----------------------------------------------------------------------------
void dump_timeseq(void)
{
  switch(v_ts_type) {
  case c_PERIODIC:
    fprintf(v_output, "periodic_ts internal_%s(",v_ts_name);
    if (v_ts_file != 0) fprintf(v_output, "\"%s\"", v_ts_file);
    else fprintf(v_output, "0");
    fprintf(v_output, ");\n");
    sprintf(v_temp,"  // --- Initialize periodic timeseq: internal_%s\n",v_ts_name);
    strcat(v_global_init,v_temp);
    break;
  case c_UNIFORM:
    fprintf(v_output, "uniform_ts internal_%s(",v_ts_name);
    if (v_ts_file != 0) fprintf(v_output, "\"%s\"", v_ts_file);
    else fprintf(v_output, "0");
    fprintf(v_output, ");\n");
    sprintf(v_temp,"  // --- Initialize uniform timeseq: internal_%s\n",v_ts_name);
    strcat(v_global_init,v_temp);
    break;
  case c_POISSON:
    fprintf(v_output, "poisson_ts internal_%s(",v_ts_name);
    if (v_ts_file != 0) fprintf(v_output, "\"%s\"", v_ts_file);
    else fprintf(v_output, "0");
    fprintf(v_output, ");\n");
    sprintf(v_temp,"  // --- Initialize poisson timeseq: internal_%s\n",v_ts_name);
    strcat(v_global_init,v_temp);
    break;
  case c_EXPLICIT:
    fprintf(v_output, "explicit_ts internal_%s(",v_ts_name);
    if (v_ts_file != 0) fprintf(v_output, "\"%s\"", v_ts_file);
    else fprintf(v_output, "0");
    fprintf(v_output, ");\n");
    sprintf(v_temp,"  // --- Initialize explicit timeseq: internal_%s\n",v_ts_name);
    strcat(v_global_init,v_temp);
    break;
  default:
    break;
  }

  switch(v_ts_type) {
    case c_UNIFORM:
    case c_PERIODIC:
    case c_POISSON:
      sprintf(v_temp,"  internal_%s.SetStart(%lf);\n",v_ts_name,v_ts_parm_val[0]);
      strcat(v_global_init,v_temp);
      sprintf(v_temp,"  internal_%s.SetEnd(%lf);\n"  ,v_ts_name,v_ts_parm_val[1]);
      strcat(v_global_init,v_temp);
      sprintf(v_temp,"  internal_%s.SetParam(%lf);\n",v_ts_name,v_ts_parm_val[2]);
      strcat(v_global_init,v_temp);
      sprintf(v_temp,"  internal_%s.Boot();\n",v_ts_name);
      strcat(v_global_init,v_temp);
      break;
    case c_EXPLICIT:
      for (int i=0; i<v_arg_loop; i++) {
	sprintf(v_temp,"  internal_%s.AddTick(%lf);\n",v_ts_name,v_ts_parm_val[i]);
	strcat(v_global_init,v_temp);
      }
      sprintf(v_temp,"  internal_%s.Boot();\n",v_ts_name);
      strcat(v_global_init,v_temp);
      break;
    default:
      break;
  }

  sprintf(v_temp,"\n");
  strcat(v_global_init,v_temp);
}

//-----------------------------------------------------------------------------
void dump_group(void)
{
  switch(v_grp_type) {
  case c_SWITCH_GROUP:
    fprintf(v_output, "set<SwitchComp *> internal_%s;\n",v_grp_name);
    break;
  case c_LINK_GROUP:
    fprintf(v_output, "set<LinkComp *> internal_%s;\n",v_grp_name);
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------
void dump_selector(void)
{
  int i;
  int j;

  fprintf(v_output, "//-----------------------------------------------------------------------------\n");
  fprintf(v_output, "// This procedure selects\n");
  fprintf(v_output, "// FROM: (");
  int code = 0, allcode = 0;

  char gname[c_TXTLEN];
  bool neg;

  for (i=0;i<v_num_clauses;i++) {
    if (i > 0) 
      fprintf(v_output, "// OR:   (");
    for (j=0;j<v_num_literals[i];j++) {
      if (j > 0) 
	fprintf(v_output, "INTERSECT");

      if (v_sel_comp[i][j][0]=='!') {
	strcpy(gname,1+v_sel_comp[i][j]);
	neg=true;
      } else {
	strcpy(gname,v_sel_comp[i][j]);
	neg=false;
      }

      if (code = is_globally_defined(gname)) {
	if (neg) fprintf(v_output, " NOT(internal_%s) ",gname);
	else fprintf(v_output, " internal_%s ",gname);
	allcode = (allcode | code);
      } else {
	sprintf(v_temp,"Reference to unknown group '%s'",gname);
	yyerror(v_temp);
      }
    }
    fprintf(v_output, ")\n");
  }

  switch (allcode) {
  case c_SWITCH_GROUP:
    fprintf(v_output, "SwitchComp * internal_%s(int index = -1) {\n", v_sel_name);
    fprintf(v_output, "  set<SwitchComp*> _finalset;\n");
    fprintf(v_output, "  set<SwitchComp*> _tempset;\n");
    fprintf(v_output, "  static SwitchComp* internal_last_choice;\n");
    break;
  case c_LINK_GROUP:
    fprintf(v_output, "LinkComp * internal_%s(int index = -1) {\n", v_sel_name);
    fprintf(v_output, "  set<LinkComp*> _finalset;\n");
    fprintf(v_output, "  set<LinkComp*> _tempset;\n");
    fprintf(v_output, "  static LinkComp* internal_last_choice;\n");
    break;
  default:
    sprintf(v_temp,"Heterogenous group types in definition of selector '%s'", v_sel_name);
    yyerror(v_temp);
    break;
  }
  fprintf(v_output, "  static double internal_locality;\n");

  fprintf(v_output, "\n  _finalset.clear();\n\n");

  for (i = 0; i < v_num_clauses; i++) {
    switch (allcode) {
    case c_SWITCH_GROUP:
      fprintf(v_output, "  internal_maker.UniverseOfSwitches(_tempset);\n");
      break;
    case c_LINK_GROUP:
      fprintf(v_output, "  internal_maker.UniverseOfLinks(_tempset);\n");
      break;
    default:
      break;
    }

    for (j=0;j<v_num_literals[i];j++) {
      if (v_sel_comp[i][j][0]=='!') {
      } else {
	strcpy(gname,v_sel_comp[i][j]);
	neg=false;
	fprintf(v_output, "  _tempset &= internal_%s;\n",gname);
      }
    }

    for (j=0;j<v_num_literals[i];j++) {
      if (v_sel_comp[i][j][0]=='!') {
	strcpy(gname,1+v_sel_comp[i][j]);
	neg=true;
	fprintf(v_output, "  _tempset -= internal_%s;\n",gname);
      } else {
      }
    }
    fprintf(v_output, "  _finalset += _tempset;\n\n");
  }

  fprintf(v_output, "  if (_finalset.empty()) { internal_last_choice = 0; }\n");
  fprintf(v_output, "  if (index >= 0 && index < _finalset.size()) {\n"
	            "    internal_last_choice = _finalset[index];\n"
	            "  } else {\n");
  switch (allcode) {
  case c_SWITCH_GROUP:
    fprintf(v_output, "    internal_last_choice = internal_maker.ChooseSwitch(&_finalset);\n");
    bind_name_globally(v_sel_name,c_SWITCH_SELECTOR);
    break;
  case c_LINK_GROUP:
    fprintf(v_output, "    internal_last_choice = internal_maker.ChooseLink(&_finalset);\n");
    bind_name_globally(v_sel_name,c_LINK_SELECTOR);
    break;
  default:
    break;
  }

  fprintf(v_output, "  }\n  if ( internal_last_choice == 0 ) cerr "
	  " << \"WARNING:  Selector failed to locate switch in group.\" << endl;\n" );
  fprintf(v_output, "  return internal_last_choice;\n");
  fprintf(v_output, "}\n\n");
}

//-----------------------------------------------------------------------------
void dump_indent(char* s) 
{
  for (int i = 0; i < v_indent_level; i++)
    strcat(s,"  ");
}

#define NUM_VIS 1

int is_special_vis(char * s)
{
  char * special_visitors[] = {
    "FastUNIVisitor"
  };

  int i = 0;

  for (i = 0; i < NUM_VIS; i++)
    if (!strcmp(s, special_visitors[i]))
      return 1;
  return 0;
}

#endif
