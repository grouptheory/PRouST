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
 * File: InitDB.cc
 * Author: battou
 * Version: $Id: InitDB.cc,v 1.35 1998/08/06 18:33:06 marsh Exp $
 * Purpose: Initialize a Database from a file.
 */

#ifndef LINT
static char const _InitDB_cc_rcsid_[] =
"$Id: InitDB.cc,v 1.35 1998/08/06 18:33:06 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include "InitDB.h"
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Protocol.h>

#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/internal_reachable_atm_addr.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/pnni_pkt/ptsp.h>
#include <fsm/database/Database.h>
#include <iostream.h>


int INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME = 0;
#define LONG_LIFE MAXSHORT

void codec_test(InfoGroup* x, int ct);

static char node_params[] =  "\t(node_params\n";
static char node_params_end[] =  "\t)\n";
static char id[] =  "\t(id %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";
static char pid[] =  "\t(parent_id %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";

static char nodal_ig[] = "\t\t(nodal_ig %u\n";
static char nodal_ig_end[] = "\t\t)\n";

static char nig_type[]  = "\t\t\t(type %u)\n";
static char nig_esi[]   =  "\t\t\t(esi 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";
static char nig_lp[]    = "\t\t\t(leadership_priority %u)\n";
static char nig_flags[] = "\t\t\t(flags 0x%x)\n";
static char nig_ppgl[]  = "\t\t\t(preferred_pgl %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n\t\t)\n\t)\n";

static char link_params[] = "\t(link_params\n";
static char link_params_end[] = "\t)\n";

static char hl_ig[] = "\t\t(horlink_ig %u\n";
static char hlig_end[] = "\t\t)\n";

// UPLINK_IG
static char upl_params[] = "\t(uplink_params\n";
static char upl_ig[] = "\t\t(uplink_ig %u\n";
static char upl_flags[] = "\t\t\t(flags 0x%x)\n";
static char upl_rnid[] = "\t\t\t(remote_node %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";
static char upl_pgid[] = "\t\t\t(common_pgid %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";
static  char upl_lpid[] = "\t\t\t(local_port %u)\n";
static char upl_aggr[] = "\t\t\t(aggr_token %u)\n";
static char upl_addr[] = "\t\t\t(upnod_addr 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";
static char upl_id_end[] = "\t\t)\n";
static char upl_params_end[] = "\t)";
// ULIA
static char ulia_ig[] = "\t\t\t(ulia\n";
static char tab4_type[] = "\t\t\t\t(type %u)\n";
static char ulia_seq[] = "\t\t\t\t(seq_num %u)\n";
static char ulia_end[] = "\t\t\t)\n";

static char tab3_type[] = "\t\t\t(type %u)\n";
static char tab3_flags[]   = "\t\t\t(flags 0x%x)\n";
static char hlig_rnid[]    = "\t\t\t(remote_node %d %d 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";
static char hlig_rpid[]    = "\t\t\t(remote_port %u)\n";
static char hlig_lpid[]    = "\t\t\t(local_port %u)\n";
static char hlig_aggr[]    = "\t\t\t(aggr_token %u)\n";

static char raig[]    = "\t\t\t(raig %u\n";
static char raig_type[]  = "\t\t\t\t(type %u)\n";
static char raig_flags[] = "\t\t\t\t(flags 0x%x)\n";
static char raig_admin_weight[]  = "\t\t\t\t(admin_weight %u)\n";
static char raig_mcr[]  = "\t\t\t\t(max_cell_rate %u)\n";
static char raig_acr[]  = "\t\t\t\t(avail_cell_rate %u)\n";
static char raig_ctd[]  = "\t\t\t\t(cell_transfer_delay %u)\n";
static char raig_cdv[]  = "\t\t\t\t(cell_delay_var %u)\n";
static char raig_clr0[] = "\t\t\t\t(clr0 %u)\n";
char raig_clr01[]= "\t\t\t\t(clr0+1 %u)\n";
static char raig_crm[]  = "\t\t\t\t(cell_rate_margin %u)\n";
static char raig_vf[]   = "\t\t\t\t(var_factor %u)\n";
static char raig_end[]  = "\t\t\t)\n";

static char iaddr[] = "\t(internal_addr\n";
static char iaddr_end[] = "\t)\n";
static char xaddr[] = "\t(external_addr\n)";
static char xaddr_end[] = "\t)\n";

static char addr[]  = "\t\t(addresses %u\n";
static char addr_scope[] = "\t\t\t(scope %u)\n";
static char addr_ail[]   = "\t\t\t(addr_info_len %u)\n";
static char addr_aic[]   = "\t\t\t(addr_info_cnt %u)\n";
static char addr_pl[]    = "\t\t\t\t(prefix_len %u)\n";
static char addr_p[]     = "\t\t\t\t(prefix 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)\n";
static char addr_end[]   = "\t\t)\n";


static ig_resrc_avail_info * parse_raig(FILE * fp)
{
  int ret;
#ifdef DEBUG_READ  
  cout << "###parse_raig\n";
#endif
  u_int ig_type;
  if ((ret = fscanf(fp, raig_type, &ig_type)) != 1) {
    cout << "error in reading IG type of the Nodal IG \n";
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "Type: " <<ig_type << endl;
#endif
  if (ig_type != 128 && ig_type != 129) {
    cout << "wrong  RAIG type \n";
    exit(1);
  }
  u_int flags;
  if ((ret = fscanf(fp, raig_flags, &flags)) != 1) {
    cout << "error in reading RAIG flags: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "flags: " << flags << endl;
#endif
  u_int admin_weight;
  if ((ret = fscanf(fp, raig_admin_weight, &admin_weight)) != 1) {
    cout << "error in reading admin_weight: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "admin_weight: " << admin_weight << endl;
#endif
  u_int max_cell_rate;
  if ((ret = fscanf(fp, raig_mcr, &max_cell_rate)) != 1) {
    cout << "error in reading max_cell_rate: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "max_cell_rate: " << max_cell_rate << endl;
#endif
  u_int avail_cell_rate;
  if ((ret = fscanf(fp, raig_acr, &avail_cell_rate)) != 1) {
    cout << "error in reading avail_cell_rate: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "avail_cell_rate: " << avail_cell_rate << endl;
#endif
  u_int cell_transfer_delay;
  if ((ret = fscanf(fp, raig_ctd, &cell_transfer_delay)) != 1) {
    cout << "error in reading cell_transfer_delay: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "cell_transfer_delay: " << cell_transfer_delay << endl;
#endif
  u_int cell_delay_var;
  if ((ret = fscanf(fp, raig_cdv, &cell_delay_var)) != 1) {
    cout << "error in reading cell_delay_var: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "cell_delay_var: " << cell_delay_var << endl;
#endif
  u_int clr0;
  if ((ret = fscanf(fp, raig_clr0, &clr0)) != 1) {
    cout << "error in reading clr0: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "clr0: " << clr0 << endl;
#endif
  u_int clr01;
  if ((ret = fscanf(fp, raig_clr01, &clr01)) != 1) {
    cout << "error in reading clr01: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "clr01: " << clr01 << endl;
#endif
  u_int cell_rate_margin;
  if ((ret = fscanf(fp, raig_crm, &cell_rate_margin)) != 1) {
    cout << "error in reading cell_rate_margin: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "cell_rate_margin: " << cell_rate_margin << endl;
#endif
  u_int var_factor;
  if ((ret = fscanf(fp, raig_vf, &var_factor)) != 1) {
    cout << "error in reading var_factor: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "var_factor: " << var_factor << endl;
#endif
  ig_resrc_avail_info::resrc_type x;
  if (ig_type == 128)
    x = ig_resrc_avail_info::outgoing;
  else
    x = ig_resrc_avail_info::incoming;
      
  ig_resrc_avail_info *raig = 
    new ig_resrc_avail_info(x, (short )flags, (int )admin_weight,
			    (int )max_cell_rate, (int )avail_cell_rate,
			    (int )cell_transfer_delay,
			    (int )cell_delay_var, (short )clr0, (short )clr01);
  return(raig);
}


static ig_horizontal_links * parse_hl(FILE *fp)
{
  int ret, i;
  
#ifdef DEBUG_READ  
  cout << "###parse_hl" << endl;
#endif
  u_int ig_type;
  if ((ret = fscanf(fp, tab3_type, &ig_type)) != 1) {
    cout << "error in reading IG type of the Nodal IG" << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "type = " << ig_type << endl;
#endif
  if (ig_type != 288) {
    cout << "wrong IG type for an Hor. Link\n";
    exit(1);
  }
  u_int flags;
  if ((ret = fscanf(fp, tab3_flags, &flags)) != 1) {
    cout << "error in reading Hor. Link flags: ret = " << ret << endl;
      exit(1);
  }
#ifdef DEBUG_READ  
  cout << "flags = " << flags << endl;
#endif
  unsigned int rnid[22];
  u_char uc_rnid[22];
  if ((ret = fscanf(fp, hlig_rnid, rnid+0, rnid+1, rnid+2, rnid+3, rnid+4,
		    rnid+5, rnid+6, rnid+7, rnid+8, rnid+9, rnid+10, rnid+11,
		    rnid+12, rnid+13, rnid+14, rnid+15, rnid+16, rnid+17,
		    rnid+18, rnid+19, rnid+20, rnid+21)) != 22) {
    cout << "error in reading node id: ret = " << ret << endl;
    exit(1);
  }
  
#ifdef DEBUG_READ  
  cout << "RemNodeID: " << rnid[0] << rnid[1] << endl;
  for (i = 2; i < 22; i++)
    cout << hex << (int)rnid[i] << " ";
  cout << dec << endl;
#endif
  for (i = 0; i < 22; i++) 
    uc_rnid[i] = (u_char)rnid[i];
  NodeID *rnode = new NodeID((u_char *)uc_rnid);
  u_int remote_port;
  if ((ret = fscanf(fp, hlig_rpid, &remote_port)) != 1) {
    cout << "error in reading Hor. Link remote port: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "remote port: " << remote_port << endl;
#endif
  u_int local_port;
  if ((ret = fscanf(fp, hlig_lpid, &local_port)) != 1) {
    cout << "error in reading Hor. Link local port: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "local port: " << local_port << endl;
#endif
  u_int aggr_token;
  if ((ret = fscanf(fp, hlig_aggr, &aggr_token)) != 1) {
    cout << "error in reading Hor. Link aggr_token: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "aggr_token: " << aggr_token << endl;
#endif
  ig_horizontal_links  *hl = new ig_horizontal_links(flags,rnode, remote_port,
						     local_port, aggr_token);
  return(hl);
}

// ------------------- ULIA ----------------------
static ig_uplink_info_attr * parse_ulia(FILE * fp)
{
  int ret;
  u_int num_raigs;
  u_int trash[80];
  // type
  u_int ig_type;
  if((ret = fscanf(fp, tab4_type, &ig_type)) != 1){
    cout << "error in reading type :ret = " << ret << endl;
    exit(1);
  }

  u_int seq;
  if((ret = fscanf(fp, ulia_seq, &seq)) != 1){
    cout << "error in reading ulia seq: ret = " << ret << endl;
    exit(1);
  }

  // raig_count
  if((ret = fscanf(fp, raig, &num_raigs) ) != 1){
    cout << "error in reading num of raigs : ret =" << ret << endl;
    exit(1);
  }

  ig_uplink_info_attr * ulia = new ig_uplink_info_attr(seq);
  for(int i = 0; i < num_raigs ; i++){
    ig_resrc_avail_info * ra = parse_raig(fp);
    ulia->AddIG(ra);
  }
  ret = fscanf(fp, raig_end, trash);
  ret = fscanf(fp, ulia_end, trash);
  return ulia;
}

// ------------------ Uplink -------------
static ig_uplinks * parse_upl(FILE * fp)
{
  int ret, i;
  int num_raigs;
  int trash[80];
#ifdef DEBUG_READ
  cout << "###parse_uplinks" << endl;
#endif
  u_int ig_type;
  if ((ret = fscanf(fp, tab3_type, &ig_type)) != 1) {
    cout << "error in reading IG type of the Uplinks IG" << endl;
    exit(1);
  }
  
#ifdef DEBUG_READ
  cout << "type = " << ig_type << endl;
#endif
  
  u_int flags;
  if ((ret = fscanf(fp, tab3_flags, &flags)) != 1) {
    cout << "error in reading uplinks flags ret = " << ret << endl;
      exit(1);
  }
#ifdef DEBUG_READ
  cout << "flags = " << flags << endl;
#endif
  
  unsigned int rnid[22];
  u_char uc_rnid[22];
  if ((ret = fscanf(fp, upl_rnid, rnid+0, rnid+1, rnid+2, rnid+3, rnid+4,
                    rnid+5, rnid+6, rnid+7, rnid+8, rnid+9, rnid+10, rnid+11,
                    rnid+12, rnid+13, rnid+14, rnid+15, rnid+16, rnid+17,
                    rnid+18, rnid+19, rnid+20, rnid+21)) != 22){
    cout << "error in reading node id: ret = " << ret << endl;
    exit(1);
  }
  
#ifdef DEBUG_READ
  cout << "UpNodeID: " << rnid[0] << rnid[1] << endl;
  for (i = 2; i < 22; i++)
    printf("0x%02x ",rnid[i]);
  printf("\n");
#endif
  for (i = 0; i < 22; i++)
    uc_rnid[i] = (u_char)rnid[i];
  NodeID *rnode = new NodeID((u_char *)uc_rnid);
  unsigned int cpgid[14];
  u_char uc_cpgid[14];

  if ((ret = fscanf(fp, upl_pgid, cpgid+0, cpgid+1, cpgid+2, cpgid+3, cpgid+4,
                    cpgid+5, cpgid+6, cpgid+7, cpgid+8, cpgid+9, cpgid+10,
                    cpgid+11, cpgid+12, cpgid+13)) != 14){
   cout << "error in reading CPGID : ret = " << ret << endl;
   exit(1);
  }

  for (i = 0; i < 14; i++)
    uc_cpgid[i] = (u_char)cpgid[i];

  PeerGroupID * CPGID = new PeerGroupID(uc_cpgid);

  u_int local_port;
  if ((ret = fscanf(fp, upl_lpid, &local_port)) != 1) {
    cout << "error in reading  local port: ret = " << ret << endl;
    exit(1);
  }

#ifdef DEBUG_READ
  cout << "local port: " << local_port << endl;
#endif

  u_int aggr_token;
  if ((ret = fscanf(fp, upl_aggr, &aggr_token)) != 1) {
    cout << "error in reading Hor. Link aggr_token: ret = " << ret << endl;
    exit(1);
  }

#ifdef DEBUG_READ
  cout << "aggr_token: " << aggr_token << endl;
#endif

  unsigned int upnodeaddr[20];
  u_char uc_upnodeaddr[20];
  if ((ret = fscanf(fp, upl_addr, upnodeaddr+0, upnodeaddr+1, upnodeaddr+2,
                    upnodeaddr+3, upnodeaddr+4, upnodeaddr+5, upnodeaddr+6,
                    upnodeaddr+7, upnodeaddr+8, upnodeaddr+9, upnodeaddr+10,
                    upnodeaddr+11, upnodeaddr+12, upnodeaddr+13, upnodeaddr+14,
                    upnodeaddr+15, upnodeaddr+16, upnodeaddr+17, upnodeaddr+18,
                    upnodeaddr+19)) != 20){
    cout << "error in reading up node addr: ret = " << ret << endl;
    exit(1);
  }
  for (i = 0; i < 20 ; i++)
    uc_upnodeaddr[i] = (u_char)upnodeaddr[i];
  ig_uplinks *ul = new ig_uplinks(flags, uc_rnid, uc_cpgid, local_port, aggr_token,  uc_upnodeaddr);
  if ((ret = fscanf(fp, raig, &num_raigs)) != 1){
    cout << "error in reading raig :ret = " << ret << endl;
    exit(1);
  }
  for (i = 0; i < num_raigs ; i ++){
   ig_resrc_avail_info * ra = parse_raig(fp);
   fscanf(fp, raig_end, trash);
   ul->AddIG(ra);
  }
  fscanf(fp, ulia_ig, trash);
  ig_uplink_info_attr * ulia = parse_ulia(fp);
  ul->AddIG(ulia);
  fscanf(fp, upl_id_end, trash);
  return ul;
}

// ------------------------- Internal Reachable Addr -------------------------
static ig_internal_reachable_atm_addr * parse_iaddr(FILE *fp)
{
  int ret, i;
#ifdef DEBUG_READ  
  cout << "###parse_iaddr\n";
#endif
  u_int ig_type;
  if ((ret = fscanf(fp, tab3_type, &ig_type)) != 1) {
    cout << "error in reading IG type in internal addr\n";
      exit(1);
  }
#ifdef DEBUG_READ  
  cout << "Type: " << ig_type << endl;
#endif
  if (ig_type != 224) {
    cout << "ERROR: wrong IG type for Addr" << endl;
    exit(1);
  }
  u_int flags;
  if ((ret = fscanf(fp, tab3_flags, &flags)) != 1) {
    cout << "ERROR in reading Addr flags: ret " << ret << endl;
    exit(1);
  }
  
#ifdef DEBUG_READ  
  cout << "flags: " << hex << (long)flags << dec << endl;
#endif
  u_int port;
  if ((ret = fscanf(fp, hlig_lpid, &port)) != 1) {
    cout << "ERROR in reading Addr port: ret is " << ret << endl;
    exit(1);
  }
  
#ifdef DEBUG_READ  
  cout << "Port: " << port << endl;
#endif
  u_int scope;
  if ((ret = fscanf(fp,addr_scope, &scope)) != 1) {
    cout << "ERROR in reading Addr scope: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "scope: " << scope << endl;
#endif
  u_int ail;
  if ((ret = fscanf(fp,addr_ail, &ail)) != 1) {
    cout << "ERROR in reading Addr ail: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "ail: " << ail << endl;
#endif
  u_int aic;
  if ((ret = fscanf(fp,addr_aic, &aic)) != 1) {
    cout << "ERROR in reading Addr aic: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "aic: " << aic << endl;
#endif
  ig_internal_reachable_atm_addr *iaddr = 
    new ig_internal_reachable_atm_addr(port,scope,ail);
  for (int k = 0; k < aic; k++) {
    u_int plen;
    if ((ret = fscanf(fp,addr_pl, &plen)) != 1) {
      cout << "ERROR in reading Addr prefix length: ret = " << ret << endl;
      exit(1);
    }
#ifdef DEBUG_READ  
    cout << "plen: " << plen << endl;
#endif
    unsigned int prefix[13];
    u_char uc_prefix[13];
    if ((ret = fscanf(fp, addr_p, prefix+0, prefix+1, prefix+2, prefix+3,
		      prefix+4, prefix+5, prefix+6, prefix+7, prefix+8,
		      prefix+9, prefix+10, prefix+11, prefix+12)) != 13) {
      cout << "ERROR in reading iaddr prefix: ret = " << ret << endl;
      exit(1);
    }
#ifdef DEBUG_READ  
    cout << "PREFIX: ";
    for (i = 0; i < 13; i++)
      cout << hex << (int)prefix[i] << " ";
    cout << dec << endl;
#endif
    for (i = 0; i < 13; i++) uc_prefix[i] = (u_char)prefix[i];
    iaddr->AddPrefix(plen,(u_char *)prefix);
  }
#ifdef DEBUG_READ  
#endif
  return(iaddr);
}


static ig_nodal_info_group * parse_nodal_ig(FILE *fp, u_char *nid, NodeID *& myPeerGroupLeader)
{
  int ret = 0, i, j;
  u_char token[1024];
  u_char nodeid[80];

  bcopy(nid, nodeid, 20);

  // read the Nodal IG and build it
  u_int ig_type;
  if ((ret = fscanf(fp, nig_type, &ig_type)) != 1) {
    cout << "ERROR in reading node type of the Nodal IG." << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "Node Type: " << ig_type << endl;
#endif
  unsigned int esi[6];
  if ((ret = fscanf(fp, nig_esi, esi+0,esi+1,esi+2,esi+3,esi+4,esi+5)) != 6) {
    cout << "ERROR in reading node esi: ret = " << ret << endl;
    exit(1);
  }

  for (i = 0, j = 13; i < 6 && j < 19; i++, j++)
    nodeid[j] = esi[i];

#ifdef DEBUG_READ  
  cout << "ESI: ";
  for (i = 0; i < 6; i++)
    cout << hex << (int)esi[i] << " ";
  cout << dec << endl;
#endif
  u_int leadership_pri;
  if ((ret = fscanf(fp, nig_lp, &leadership_pri)) != 1) {
    cout << "ERROR in reading node leadership priority: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "Node Leadership Prio: " << leadership_pri << endl;
#endif
  
  u_int flags;
  if ((ret = fscanf(fp, nig_flags, &flags)) != 1) {
    cout << "ERROR in reading node flags: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "Node IG flags = " << flags << endl;
#endif

  unsigned int ppgl[22];
  u_char uc_ppgl[22];
  if ((ret = fscanf(fp, nig_ppgl, ppgl+0,ppgl+1,ppgl+2,ppgl+3,ppgl+4,
		    ppgl+5,ppgl+6,ppgl+7,ppgl+8,
		    ppgl+9,ppgl+10,ppgl+11,ppgl+12,ppgl+13,ppgl+14,ppgl+15,
		    ppgl+16,ppgl+17,
		    ppgl+18,ppgl+19,ppgl+20,ppgl+21)) != 22) {
    cout << "ERROR in reading node PGL ID: ret = " << ret << endl;
    exit(1);
  }
#ifdef DEBUG_READ  
  cout << "PreferredPGLNodeID: " << (int)ppgl[0] << (int)ppgl[1] << ":";
  for (int i = 2; i < 22; i++)
    cout << hex << (int)ppgl[i] << " ";
  cout << dec << endl;
#endif
  for (i = 0; i < 22; i++) 
    uc_ppgl[i] = (u_char)ppgl[i];
  myPeerGroupLeader = new NodeID((u_char *)uc_ppgl);

  fscanf(fp, nodal_ig_end, token);
  fscanf(fp, node_params_end, token);

  return new ig_nodal_info_group(nodeid, (u_char)leadership_pri, 
				 (u_char)flags, (u_char *)uc_ppgl);
}


Conduit * InitDB(FILE * fp, const char *dbConduitName, 
		 Database *& db, NodeID *& myNodeID, 
		 NodeID *& myParentNodeID,
		 NodeID *& myPreferredPeerGroupLeader,
		 bool more, int level)
{
  Conduit  * dbConduit = 0;
  Protocol * dbProtocol = 0;
  PeerID   * myPeerGroupID = 0;

  u_char token[1024];	// One line of input from *fp
  u_int  nid[22];	// Node ID
  u_char uc_nid[22];	// Node ID
  u_int  pnid[22];	// Parent Node ID
  u_char uc_pnid[22];	// Parent Node ID

  // These are the PTSEs we need to initialize our Database
  ig_ptse * nodal_ptse  = 0;
  ig_ptse * hl_ptse     = 0;
  ig_ptse * iaddr_ptse  = 0;
  ig_ptse * uplink_ptse = 0;
  ig_uplinks * uplig = 0;
  ig_nodal_info_group * nodal_ig_ptr = 0;

  PTSPPkt * ptsp = 0;

  u_int num_links = 0;
  u_int num_iaddr = 0;
  u_int num_xaddr = 0;
  u_int num_nig   = 0;
  u_int num_uplig = 0;

  u_int id_number = 1;

  const u_char * nodeid = 0;

  int  ret, i;
  bool have_nodeid = false;

  while ((ret = fscanf(fp, "%s\n", token)) != EOF) {
    if (ret != 1) {
      cout << "ERROR in reading while determining: node_params OR link_params." << endl;
      exit(1);
    }
#ifdef DEBUG_READ  
    cout << "Token: " << token << endl;
#endif
    switch (token[1]) {
      case 'n':
	// NodeID
	if ((ret = fscanf(fp, id, nid+0, nid+1, nid+2, nid+3, nid+4, nid+5,
			  nid+6, nid+7, nid+8, nid+9, nid+10,
			  nid+11, nid+12, nid+13, nid+14, nid+15,
			  nid+16, nid+17, nid+18, nid+19, nid+20, nid+21)) != 22) {
	  cout << "ERROR in reading node id: ret = " << ret << endl << "Using parameter instead." << endl;
	  const u_char * mynid = myNodeID->GetNID();
	  for (i = 0; i < 22; i++)
	    nid[i] = mynid[i];
	}

	for (i = 0; i < 22; i++) 
	  uc_nid[i] = (u_char)nid[i];

#ifdef DEBUG_READ  
	cout << "NodeID: " << (int)uc_nid[0] << " " << (int)uc_nid[1] << ":";
	for (i = 2; i < 22; i++)
	  cout << hex << (int)uc_nid[i] << " ";
	cout << dec << endl;
#endif
	if (!have_nodeid) {
	  //      myNodeID = new NodeID(&(uc_nid[0]));
	  myNodeID = new NodeID((u_char *)uc_nid);
	  myPeerGroupID = new PeerID(uc_nid+1);

	  if (!more) {
	    db = new Database(myNodeID);
	    dbProtocol = new Protocol(db);
	    dbConduit = new Conduit(dbConduitName, dbProtocol);
	  }
	  if (db != 0) {
	    // Parent ID
	    if ((ret = fscanf(fp, pid, pnid+0, pnid+1, pnid+2, pnid+3, pnid+4,
			      pnid+5, pnid+6, pnid+7, pnid+8, pnid+9, pnid+10,
			      pnid+11,pnid+12,pnid+13,pnid+14, pnid+15, pnid+16, 
			      pnid+17, pnid+18, pnid+19, pnid+20, 
			      pnid+21)) != 22) {
	      cout << "ERROR in reading parent node id: ret = " << ret << endl;
	      exit(1);
	    }
	  
	    for (i = 0; i < 22; i++) 
	      uc_pnid[i] = (u_char)pnid[i];
#ifdef DEBUG_READ  
	    cout << "ParentNodeID: " << (int)uc_pnid[0] << " " << (int)uc_pnid[1] << ":";
	    for (i = 2; i < 22; i++)
	      cout << hex << (int)uc_pnid[i] << endl;
	    cout << dec << endl;
#endif
	    myParentNodeID = new NodeID((u_char *)uc_pnid);
	  }
	}    

	if (db) {
	  // Nodal IG -- num_links contains the number of nodal_igs to be seen
	  if ((ret = fscanf(fp, nodal_ig, &num_links)) == 1) {
	    nodal_ptse  = 
	      new ig_ptse(InfoGroup::ig_nodal_info_group_id, 1, 1, 0, 
			  (INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME ?
			   LONG_LIFE : db->PTSELifetime()));

	    for (int j = 0; j < num_links; j++) {
	      if (nodal_ig_ptr = parse_nodal_ig(fp, (u_char *)(uc_nid+2), 
					myPreferredPeerGroupLeader))
		nodal_ptse->AddIG(nodal_ig_ptr);
	    }
	    ptsp = new PTSPPkt(uc_nid, uc_nid + 1);
	    ptsp->AddPTSE(nodal_ptse);
	    
	    num_nig ++;
	  } else {
	    cerr << "ERROR reading nodal_ig, fscanf returned " << ret << endl;
	    exit(1);
	  }
	} else {
	  cout << "No database to insert Nodal IG into. " << endl;
	  exit(1);
	}
	
	break;
	
	
    case 'l': // link params
        if ((ret = fscanf(fp, hl_ig, &num_links)) != 1) {
	  cerr << "ERROR in reading number of links in horizontal links IG, fscanf returned " << ret << endl;
	  exit(1);
	}
#ifdef DEBUG_READ  
	cout << "num links is " << num_links << endl;
#endif
	for (int k = 0 ; k < num_links; k++) {
	  ig_horizontal_links *hl = parse_hl(fp);
	  u_int num_raigs;
	  if ((ret = fscanf(fp, raig, &num_raigs)) != 1) {
	    cout << "ERROR in reading Hor. Link num raigs: ret = " << ret << endl;
	    exit(1);
	  }
	  for (int r = 0; r < num_raigs; r++)
	    hl->AddRAIG(parse_raig(fp));
	  fscanf(fp, raig_end, token);

	  // read the Horizontal link IG and build it
	  if (db != 0) {
	    hl_ptse = new ig_ptse(InfoGroup::ig_horizontal_links_id, 
				  (id_number = hl->GetLocalPID() + 3), 1, 0, 
				  (INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME?
				   LONG_LIFE : db->PTSELifetime()));
	    hl_ptse->AddIG(hl);
	  
	    ptsp->AddPTSE(hl_ptse);
	  } else {
	    cout << "No database to insert Hlink IG into." << endl;
	    exit(1);
	  }
	}

	fscanf(fp, hlig_end, token);
	fscanf(fp, link_params_end, token);

	break;


      case 'i':	// read internal reachable addresses IG and build it
	if ((ret = fscanf(fp, addr, &num_iaddr)) != 1) {
	  cout << "ERROR in reading num_iaddr: ret = " << ret << endl;
	  exit(1);
	}
#ifdef DEBUG_READ  
	cout << "num_iaddr = " << num_iaddr << endl;
#endif
	for (i = 0; i < num_iaddr; i++) {
	  if (db != 0) iaddr_ptse  = new 
			 ig_ptse(InfoGroup::ig_internal_reachable_atm_addr_id,
				 2, 1, 0, (INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME ?
					   LONG_LIFE : db->PTSELifetime()));

	  ig_internal_reachable_atm_addr *iaddr = parse_iaddr(fp);
	  u_int num_raigs;
	  if ((ret = fscanf(fp, raig, &num_raigs)) != 1) {
	    cout << "ERROR in reading internal addr num raigs: ret = " 
		 << ret << endl;
	    exit(1);
	  }

	  for (int r = 0; r < num_raigs; r++) {
	    ig_resrc_avail_info * aRAIG = parse_raig(fp);
	    if (db != 0) iaddr->AddOutGoingRAIG(aRAIG);
	  }

	  fscanf(fp, raig_end, token);
	  
	  if (db != 0) {
	    iaddr_ptse->AddIG(iaddr);
	    ptsp->AddPTSE(iaddr_ptse);
	  } else {
	    cout << "No database to insert Internal Reachable Addresses IG into." << endl;
	    exit(1);
	  }
	}
	fscanf(fp, addr_end, token);
	fscanf(fp, iaddr_end, token);
	
	break;

      case 'e': // read external reachable addresses IG and build it
	if ((ret = fscanf(fp, xaddr, &num_xaddr)) != 1) {
	  cout << "ERROR in reading Hor. Link num_xaddr: ret = " << ret << endl;
	  exit(1);
	}
#ifdef DEBUG_READ  
	cout << "HL IG num_xaddr = " << num_xaddr << endl;
#endif
	break;


      case 'u': // Uplinks
	if (more) {
	  nodeid = myNodeID->GetNID();
	  ptsp = new  PTSPPkt(nodeid, nodeid +1);
	}
        // Read the uplinks
        if ((ret = fscanf(fp, upl_ig, &num_uplig)) != 1){
          cout << "ERROR in reading Uplink ig ret = " << ret << endl;
          exit(1);
        }
	for (i = 0; i < num_uplig ; i++) {
	  uplig = parse_upl(fp);
	  
	  uplink_ptse = new ig_ptse(InfoGroup::ig_uplinks_id,
				    uplig->GetLocalPID()+10, 1, 0 ,
				    (INITIAL_DB_PTSES_HAVE_INFINITE_LIFETIME ?
				     LONG_LIFE : 120));
	  uplink_ptse->AddIG(uplig);
	  if (db != 0)
	    ptsp->AddPTSE(uplink_ptse);
	  else {
	    cout << "No database to insert Uplinks IG." << endl;
	    exit(1);
	  }
	}
	fscanf(fp, upl_params_end, token);
	break;


      default:
	cout << "bad token" << endl;
	exit(1);
	break;
      }
  }

  if (num_nig + num_links + num_iaddr != 0) { 
#if 0
    const list<ig_ptse *> & ptse_list = ptsp->GetElements();
    list_item li;
    forall_items(li, ptse_list) {
      InfoGroup * tmp  = (ptse_list.inf(li))->copy();
      codec_test(tmp,5);
    }
#endif
    db->Insert(ptsp); 
  }

  return dbConduit;
}



void codec_test(InfoGroup * x, int ct)
{
  if (ct > 0) {
    InfoGroup * y= x->copy();
    ((ig_ptse*)x)->UnReference();
    cout << *((ig_ptse *)y) << endl;
    codec_test(y,ct-1);
  } else 
    return;
}
