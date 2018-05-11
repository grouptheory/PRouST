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

/*
 * File: ExtInitDB.cc
 * Author: mountcas
 * Version: $Id: ExtInitDB.cc,v 1.35 1998/08/06 18:32:02 marsh Exp $
 * Purpose: Initialize a Database from a file.
 */

#ifndef LINT
static char const _ExtInitDB_cc_rcsid_[] =
"$Id: ExtInitDB.cc,v 1.35 1998/08/06 18:32:02 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include "ExtInitDB.h"
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Protocol.h>

#include <fsm/database/Database.h>

#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/internal_reachable_atm_addr.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/uni_ie/addr.h>

#include <iostream.h>


int EXT_INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME = 0;
int linenum = 0;

#define LONG_LIFE MAXSHORT

// void skip_ws(char ** string)
// { for (; **string && isspace(**string); (*string)++); }

void skip_ws(char * string)
{ 
  int i = 0, j = 0;

  while (isspace(string[i]) && string[i] != '\0')
    i++;

  if (i) {
    for (j = 0; string[i] != '\0'; i++, j++)
      string[j] = string[i];
    string[j] = '\0';
  }
}

void _get_token(const char * const line, char * token)
{
  int i = 0, j = 0;
  while (line[i] != '\0' && !isalpha(line[i]))
    i++;
  while (line[i] != '\0' && !isspace(line[i]) && line[i] != '\n')
    token[j++] = line[i++];
  token[j] = '\0';
}

// --------------------------- INTERNAL REACHABLE ADDRESS BLOCK -------------------------
static ig_resrc_avail_info * _parse_raig(FILE * fp)
{
  int   ret;
  char  linebuf[512];
  u_int ig_type;

  linenum++;
  if (fgets(linebuf, 512, fp) > 0) {
    skip_ws(linebuf);
    if (!*linebuf)
      return 0;
    if ((ret = sscanf(linebuf, "(type %u)", &ig_type)) != 1) {
      cerr << "ERROR in reading IG type of the Nodal IG line " << linenum << endl;
      exit(1);
    }
    if (ig_type != 128 && ig_type != 129) {
      cerr << "ERROR invalid RAIG type " << ig_type << endl;
      exit(1);
    }
    u_int flags;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(flags 0x%x)", &flags)) != 1) {
      cerr << "ERROR in reading RAIG flags: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int admin_weight;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(admin_weight %u)", &admin_weight)) != 1) {
      cerr << "ERROR in reading admin_weight: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int max_cell_rate;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(max_cell_rate %u)", &max_cell_rate)) != 1) {
      cerr << "ERROR in reading max_cell_rate: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int avail_cell_rate;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(avail_cell_rate %u)", &avail_cell_rate)) != 1) {
      cerr << "ERROR in reading avail_cell_rate: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int cell_transfer_delay;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(cell_transfer_delay %u)", &cell_transfer_delay)) != 1) {
      cerr << "ERROR in reading cell_transfer_delay: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int cell_delay_var;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(cell_delay_var %u)", &cell_delay_var)) != 1) {
      cerr << "ERROR in reading cell_delay_var: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int clr0;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(clr0 %u)", &clr0)) != 1) {
      cerr << "ERROR in reading clr0: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int clr01;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(clr0+1 %u)", &clr01)) != 1) {
      cerr << "ERROR in reading clr01: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int cell_rate_margin;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(cell_rate_margin %u)", &cell_rate_margin)) != 1) {
      cerr << "ERROR in reading cell_rate_margin: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int var_factor;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(var_factor %u)", &var_factor)) != 1) {
      cerr << "ERROR in reading var_factor: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    ig_resrc_avail_info * raig =
      new ig_resrc_avail_info(((ig_type == 128) ? 
			       ig_resrc_avail_info::outgoing :
			       ig_resrc_avail_info::incoming), 
			      (short) flags, (int) admin_weight,
			      (int) max_cell_rate, (int) avail_cell_rate,
			      (int) cell_transfer_delay,
			      (int) cell_delay_var, (short) clr0, 
			      (short) clr01);
    return (raig);
  } else
    cerr << "ERROR unable to continue reading from the file." << endl;

  return 0;
}

// ------------------------- Nodal Hierarchy List ----------------------------------
static ig_nodal_hierarchy_list * _parse_nhl(FILE * fp)
{
  int   ret;
  char  linebuf[512];
  u_int ig_type;

  linenum++;
  if (fgets(linebuf, 512, fp) > 0) {
    skip_ws(linebuf);
    if (!*linebuf)
      return 0;
    if ((ret = sscanf(linebuf, "(type %u)", &ig_type)) != 1) {
      cerr << "ERROR in reading IG type in nodal hierarchy list on line " << linenum << endl;
      exit(1);
    }
    if (ig_type != 33) {
      cerr << "ERROR, wrong IG type for address." << endl;
      exit(1);
    }
    long seq_num;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(seq %ld", &seq_num)) != 1) {
      cerr << "ERROR in reading sequence number: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    ig_nodal_hierarchy_list * nhl = new ig_nodal_hierarchy_list(seq_num);

    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    while (linebuf[0] != ')') {
      unsigned int rnid[22];
      u_char uc_rnid[22];
      if ((ret = sscanf(linebuf, "(NHL node %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", 
			rnid+0, rnid+1, rnid+2, rnid+3, rnid+4, rnid+5, rnid+6, rnid+7, rnid+8, 
			rnid+9, rnid+10, rnid+11, rnid+12, rnid+13, rnid+14, rnid+15, rnid+16, 
			rnid+17, rnid+18, rnid+19, rnid+20, rnid+21)) != 22) {
	cerr << "ERROR in reading node id: " << ret << " on line " << linenum << endl;
	exit(1);
      }
      for (int i = 0; i < 22; i++) 
	uc_rnid[i] = (u_char)rnid[i];
      NodeID nid(uc_rnid);
      PeerID *pid = nid.GetPeerGroup();
      pid->SetLevel((int)uc_rnid[0]);

      Addr * addy = nid.GetAddr();
      nhl->AddLevel(&nid, addy, pid);
      delete pid;
      // Repeat until closing ) is reached.
      fgets(linebuf, 512, fp); linenum++;
      skip_ws(linebuf);
      delete addy;
    }
    return nhl;
  } else
    cerr << "ERROR reading from file.  Current line is " << linenum << endl;

  return 0;
}

static ig_internal_reachable_atm_addr * _parse_iaddr_ig(FILE * fp)
{
  int ret;
  char linebuf[512];
  u_int ig_type;

  linenum++;
  if (fgets(linebuf, 512, fp) > 0) {
    skip_ws(linebuf);
    if (!*linebuf)
      return 0;
    if ((ret = sscanf(linebuf, "(type %u)", &ig_type)) != 1) {
      cerr << "ERROR in reading IG type in internal addr on line " << linenum << endl;
      exit(1);
    }
    if (ig_type != 224) {
      cerr << "ERROR, wrong IG type for address." << endl;
      exit(1);
    }
    u_int flags;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(flags 0x%x)", &flags)) != 1) {
      cerr << "ERROR in reading address flags: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int port;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(local_port %u)", &port)) != 1) {
      cerr << "ERROR in reading address port: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int scope;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(scope %u)", &scope)) != 1) {
      cerr << "ERROR in reading address scope: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int ail;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(addr_info_len %u)", &ail)) != 1) {
      cerr << "ERROR in reading address ail: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int aic;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(addr_info_cnt %u)", &aic)) != 1) {
      cerr << "ERROR in reading address aic: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    ig_internal_reachable_atm_addr * iaddr = new ig_internal_reachable_atm_addr(port, scope, ail);
    
    for (int k = 0; k < aic; k++) {
      u_int plen;
      fgets(linebuf, 512, fp); linenum++; 
      skip_ws(linebuf);
      if ((ret = sscanf(linebuf, "(prefix_len %u)", &plen)) != 1) {
	cerr << "ERROR in reading address prefix length: " << ret << " on line " << linenum << endl;
	exit(1);
      }
      unsigned int prefix[13];
      u_char uc_prefix[13];
      fgets(linebuf, 512, fp); linenum++; 
      skip_ws(linebuf);
      if ((ret = sscanf(linebuf, "(prefix 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", 
			prefix+0, prefix+1, prefix+2, prefix+3, prefix+4, prefix+5, prefix+6, prefix+7, 
			prefix+8, prefix+9, prefix+10, prefix+11, prefix+12)) != 13) {
	cerr << "ERROR in reading iaddr prefix: " << ret << " on line " << linenum << endl;
	exit(1);
      }
      for (int i = 0; i < 13; i++) 
	uc_prefix[i] = (u_char)prefix[i];
      iaddr->AddPrefix(plen,(u_char *)prefix);
    }
    u_int num_raigs;
    linenum++; 
    if (fgets(linebuf, 512, fp)) {
      skip_ws(linebuf);
      if (!*linebuf)
	return (iaddr);
      if ((ret = sscanf(linebuf, "(raig %u", &num_raigs)) != 1) {
	cerr << "ERROR in reading internal addr num raigs: " << ret << " on line " << linenum << endl;
	exit(1);
      }
      for (int r = 0; r < num_raigs; r++)
	iaddr->AddOutGoingRAIG(_parse_raig(fp));
      // end of RAIG token.
      fgets(linebuf, 512, fp); linenum++; 
    } 
    // End of iaddr
    return (iaddr);
  } else
    cerr << "ERROR reading from file.  Current line is " << linenum << endl;

  return 0;
}


// --------------------------- HORIZONTAL LINKS BLOCK ---------------------
static ig_horizontal_links * _parse_hl_ig(FILE * fp)
{
  int ret, i;
  char linebuf[512];
  u_int ig_type;

  linenum++;
  if (fgets(linebuf, 512, fp) > 0) {
    skip_ws(linebuf);
    if (!*linebuf)
      return 0;
    if ((ret = sscanf(linebuf, "(type %u)", &ig_type)) != 1) {
      cerr << "ERROR in reading IG type in horizontal links on line " << linenum << endl;
      exit(1);
    }
    if (ig_type != 288) {
      cerr << "ERROR, wrong IG type for a horizontal link." << endl;
      exit(1);
    }
    u_int flags;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(flags 0x%x)", &flags)) != 1) {
      cerr << "ERROR in reading Hor. Link flags: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    unsigned int rnid[22];
    u_char uc_rnid[22];
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(remote_node %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", 
		      rnid+0, rnid+1, rnid+2, rnid+3, rnid+4, rnid+5, rnid+6, rnid+7, rnid+8, 
		      rnid+9, rnid+10, rnid+11, rnid+12, rnid+13, rnid+14, rnid+15, rnid+16, 
		      rnid+17, rnid+18, rnid+19, rnid+20, rnid+21)) != 22) {
      cerr << "ERROR in reading node id: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    for (i = 0; i < 22; i++) 
      uc_rnid[i] = (u_char)rnid[i];

    u_int remote_port;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(remote_port %u)", &remote_port)) != 1) {
      cerr << "ERROR in reading Hor. Link remote port: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int local_port;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(local_port %u)", &local_port)) != 1) {
      cerr << "ERROR in reading Hor. Link local port: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int aggr_token;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(aggr_token %u)", &aggr_token)) != 1) {
      cerr << "ERROR in reading Hor. Link aggr_token: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int num_raigs;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(raig %u)", &num_raigs)) != 1) {
      cerr << "ERROR in reading Hor. Link num raigs: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    ig_horizontal_links * hl = new ig_horizontal_links(flags, (u_char *)uc_rnid, 
						       remote_port, local_port, aggr_token);

    for (i = 0; i < num_raigs; i++)
      hl->AddRAIG(_parse_raig(fp));

    // Skip the closing parens for the raig number
    fgets(linebuf, 512, fp); linenum++; 
    return (hl);
  } else
    cerr << "ERROR reading from file.  Current line is " << linenum << endl;

  return 0;
}
  
// --------------------------- NODAL IGs BLOCK ---------------------
static ig_nodal_info_group * _parse_nodal_ig(FILE *fp, u_char *nid, NodeID *& myPeerGroupLeader)
{
  int ret = 0, i, j;
  char linebuf[512];
  u_char nodeid[80];
  u_int ig_type;

  bcopy(nid, nodeid, 20);

  linenum++;
  if (fgets(linebuf, 512, fp) > 0) {
    skip_ws(linebuf);
    if (!*linebuf)
      return 0;
    if ((ret = sscanf(linebuf, "(type %u)", &ig_type)) != 1) {
      cerr << "ERROR in reading IG type in nodal ig on line " << linenum << endl;
      exit(1);
    }
    unsigned int esi[6];
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(esi 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", 
		      esi+0,esi+1,esi+2,esi+3,esi+4,esi+5)) != 6) {
      cerr << "ERROR in reading node esi: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    for (i = 0, j = 13; i < 6 && j < 19; i++, j++)
      nodeid[j] = esi[i];
    u_int leadership_pri;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(leadership_priority %u)", &leadership_pri)) != 1) {
      cerr << "ERROR in reading node leadership priority: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    u_int flags;
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(flags 0x%x)", &flags)) != 1) {
      cerr << "ERROR in reading node flags: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    unsigned int ppgl[22];
    u_char uc_ppgl[22];
    fgets(linebuf, 512, fp); linenum++; 
    skip_ws(linebuf);
    if ((ret = sscanf(linebuf, "(preferred_pgl %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", 
		      ppgl+0,ppgl+1,ppgl+2,ppgl+3,ppgl+4, ppgl+5,ppgl+6,ppgl+7,ppgl+8,
		      ppgl+9,ppgl+10,ppgl+11,ppgl+12,ppgl+13,ppgl+14,ppgl+15, ppgl+16,
		      ppgl+17, ppgl+18,ppgl+19,ppgl+20,ppgl+21)) != 22) {
      cerr << "ERROR in reading node PGL ID: " << ret << " on line " << linenum << endl;
      exit(1);
    }
    for (i = 0; i < 22; i++) 
      uc_ppgl[i] = (u_char)ppgl[i];
    if(myPeerGroupLeader == 0)
      myPeerGroupLeader = new NodeID((u_char *)uc_ppgl);

    return new ig_nodal_info_group(nodeid, (u_char)leadership_pri, (u_char)flags, (u_char *)ppgl);
  } else
    cerr << "ERROR reading from file.  Current line is " << linenum << endl;

  return 0;
}


// Most of the parsing is controlled through this function here ...
void _parse_origin_block(char * linebuf, Database * db, NodeID *& myPrefPGL, FILE * fp)
{
  int    ret = 0, num_links;
  u_int  nid[22], idnum, seqnum;
  u_char uc_nid[22];
  char   buf[512];
  PTSPPkt * ptsp = 0;

  skip_ws(linebuf);
  // The line passed to us contains the origin id and seq number
  if ((ret = sscanf(linebuf, "(origin %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x id %u seq %u", 
		    nid+0, nid+1, nid+2, nid+3, nid+4, nid+5, nid+6, nid+7, nid+8, nid+9, nid+10,
		    nid+11, nid+12, nid+13, nid+14, nid+15, nid+16, nid+17, nid+18, nid+19, nid+20, 
		    nid+21, &idnum, &seqnum)) != 24) {
    cerr << "ERROR in reading node id: " << ret << " on line " << linenum << endl;
    exit(1);
  }
  for (int i = 0; i < 22; i++)
    uc_nid[i] = nid[i];

  ptsp = new PTSPPkt(uc_nid, uc_nid+1);

  linenum++;
  if (fgets(buf, 512, fp) > 0) {
    skip_ws(buf);

    if (!strncmp(buf, "(nodal_ig", 9)) {
      ig_ptse * nodal_ptse = 0;
      if ((ret = sscanf(buf, "(nodal_ig %u", &num_links)) == 1) {
	nodal_ptse  = 
	  new ig_ptse(InfoGroup::ig_nodal_info_group_id, idnum, seqnum, 0, 
		      (EXT_INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME ?
		       LONG_LIFE : db->PTSELifetime()));
	ig_nodal_info_group * nodal_ig;
	for (int j = 0; j < num_links; j++) {
	  if (nodal_ig = _parse_nodal_ig(fp, (u_char *)(uc_nid+2), myPrefPGL))
	    nodal_ptse->AddIG(nodal_ig);
	}
      }
      if (nodal_ptse)
	ptsp->AddPTSE(nodal_ptse);
      // end of nodal_ig block
      fgets(linebuf, 512, fp); linenum++; 
    } else if (!strncmp(buf, "(horlink_ig", 11)) {
      ig_ptse * horiz_ptse = 0;
      if ((ret = sscanf(buf, "(horlink_ig %u", &num_links)) == 1) {
	horiz_ptse  = 
	  new ig_ptse(InfoGroup::ig_horizontal_links_id, idnum, seqnum, 0,
		      (EXT_INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME?
		       LONG_LIFE : db->PTSELifetime()));
	ig_horizontal_links * horiz_ig;
	for (int j = 0; j < num_links; j++) {
	  if (horiz_ig = _parse_hl_ig(fp))
	    horiz_ptse->AddIG(horiz_ig);
	}
      }
      if (horiz_ptse)
	ptsp->AddPTSE(horiz_ptse);
      // end of horiz_ig block
      fgets(linebuf, 512, fp); linenum++; 
    } else if (!strncmp(buf, "(addresses", 10)) {
      ig_ptse * addr_ptse = 0;
      if ((ret = sscanf(buf, "(addresses %u", &num_links)) == 1) {
	addr_ptse  = new ig_ptse(InfoGroup::ig_internal_reachable_atm_addr_id,
				 idnum, seqnum, 0,
				 (EXT_INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME?
				  LONG_LIFE : db->PTSELifetime()));
	ig_internal_reachable_atm_addr * addr_ig;
	for (int j = 0; j < num_links; j++) {
	  if (addr_ig = _parse_iaddr_ig(fp))
	    addr_ptse->AddIG(addr_ig);
	}
      }
      if (addr_ptse)
	ptsp->AddPTSE(addr_ptse);
      // end of iaddr_ig block
      fgets(linebuf, 512, fp); linenum++; 
    } else if (!strncmp(buf, "(lists", 6)) {
      ig_ptse * nhl_ptse = 0;
      if ((ret = sscanf(buf, "(lists %u", &num_links)) == 1) {
	nhl_ptse  = new ig_ptse(InfoGroup::ig_nodal_hierarchy_list_id,
				idnum, seqnum, 0,
				(EXT_INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME?
				 LONG_LIFE : db->PTSELifetime()));
	ig_nodal_hierarchy_list	* nhl_ig;
	for (int j = 0; j < num_links; j++) {
	  if (nhl_ig = _parse_nhl(fp))
	    nhl_ptse->AddIG(nhl_ig);
	}
      }
      if (nhl_ptse)
	ptsp->AddPTSE(nhl_ptse);
      // end of iaddr_ig block
      fgets(linebuf, 512, fp); linenum++; 
    } else {
      cerr << "ERROR parsing line " << linenum << " -- unknown token '" << buf << "' exiting." << endl;
      exit(1);
    }
  } else
    cerr << "ERROR parsing line " << linenum << " exiting." << endl;

  // end of origin block
  fgets(linebuf, 512, fp); linenum++; 

  if (((list<ig_ptse *>)ptsp->GetElements()).size() > 0)
    db->Insert(ptsp);
  else
    delete ptsp;
}

// Main entry point for parsing database files.
Conduit * ExtInitDB(FILE * fp, const char * dbConduitName, 
		    Database *& db, NodeID *& myNodeID, 
		    NodeID *& myParentNodeID,
		    NodeID *& myPreferredPeerGroupLeader)
{
  int    ret;
  u_int  nid[22];
  char   linebuf[512];
  Conduit * rval = 0;
  //  PeerID  * myPeerGroupID = 0;

  // parse the whole file
  while (fgets(linebuf, 512, fp) > 0) {
    linenum++;
    char token[80];

    skip_ws(linebuf);
    _get_token(linebuf, token);

    switch (token[0]) {
      case 'n': // node_params
	if (strcmp(token, "node_params"))
	  continue;
        // This is a special case since the database cannot be built until the NodeID is
        //  extracted from the node params
        linenum++;
        if (!fgets(linebuf, 512, fp)) {
	  cerr << "ERROR reading the node id on line " << linenum << endl;
	  exit(1);
        } else {
	  // read in node id
	  skip_ws(linebuf);
	  if ((ret = sscanf(linebuf, "(id %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", 
			    nid+0, nid+1, nid+2, nid+3, nid+4, nid+5, nid+6, nid+7, nid+8, 
			    nid+9, nid+10, nid+11, nid+12, nid+13, nid+14, nid+15, nid+16, 
			    nid+17, nid+18, nid+19, nid+20, nid+21)) != 22) {
	    cerr << "ERROR in reading node id: " << ret << " on line " << linenum << endl;
	    exit(1);
	  } else {
	    u_char uc_nid[22];
	    for (int i = 0; i < 22; i++)
	      uc_nid[i] = nid[i];
	    myNodeID = new NodeID(uc_nid);
	    //	    myPeerGroupID = new PeerID(uc_nid+1);
	    db = new Database(myNodeID);
	    Protocol * tmp = new Protocol(db);
	    rval = new Conduit(dbConduitName, tmp);
	  }
	}
	linenum++;
        if (!fgets(linebuf, 512, fp)) {
	  cerr << "ERROR reading the parent node id on line " << linenum << endl;
	  exit(1);
        } else {
	  // read in parent node id
	  skip_ws(linebuf);
	  if ((ret = sscanf(linebuf, "(parent_id %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", 
			    nid+0,  nid+1,  nid+2,  nid+3,  nid+4,  nid+5,  nid+6, 
			    nid+7,  nid+8,  nid+9,  nid+10, nid+11, nid+12, nid+13,
			    nid+14, nid+15, nid+16, nid+17, nid+18, nid+19, 
			    nid+20, nid+21)) != 22) {
	    cerr << "ERROR in reading parent node id: " << ret << " on line " << linenum << endl;
	    exit(1);
	  } else {
	    u_char uc_pnid[22];
	    for (int i = 0; i < 22; i++)
	      uc_pnid[i] = nid[i];
	    myParentNodeID = new NodeID((u_char *)uc_pnid);
	  }
	}
	// continue on from here to the origin case
        break;
      case 'o': // origin -- special case for nodal_igs, hor_links, and reachable addys
	_parse_origin_block(linebuf, db, myPreferredPeerGroupLeader, fp);
	break;
      case 'l': // link_params
      case 'i': // internal_addr
      default:
	// These get skipped, the next line should be origin ...
	break;
    }
    // continue;
  }
  // EOF
  return rval;
}


#if 0 // To be finished at a later date.

Conduit * ExtInitDB(const char filename, const char * dbConduitName, 
		 Database *& db, NodeID *& myNodeID, 
		 NodeID *& myParentNodeID,
		 NodeID *& myPreferredPeerGroupLeader)
{
}

Conduit * ExtInitDB(ifstream infile, const char * dbConduitName, 
		 Database *& db, NodeID *& myNodeID, 
		 NodeID *& myParentNodeID,
		 NodeID *& myPreferredPeerGroupLeader)
{
}

#endif

