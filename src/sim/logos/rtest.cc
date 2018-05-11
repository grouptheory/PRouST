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


//
// this code is used to debug elements of the routing algorithm.
// it dosn't do anthing particularly useful and the output varies
// as I edit. THIS IS NOT a BENCHMARK
//
#if !defined(LINT)
static char const _rtest_cc_rcsid_[] =
"$Id: rtest.cc,v 1.16 1998/05/19 16:53:16 marsh Exp $";
#endif

#include <iostream.h>
#include <sim/logos/Logos.h>
#include <sim/logos/BaseLogos.h>
#include <sim/logos/LogosGraph.h>
#include <sim/logos/DynamicLogos.h>
#include <codec/pnni_ig/id.h>

//
//  1 = 2 = 3 = 4
//      |   |   |
//      5 = 6 = 7
//      |   |   |
//      8 = 9 =10 = 11 = 12
//
//
//

int main (int argc, char** argv)
{
    int e=0; // counter for next edge pair in ME macro (MakeEdge)
    int i,j;



#define NN 13 // number of nodes + 1 (0 is used to construct "to" !)
    // two of the nodes (11,12) are in another peer group
#define NE 30 // number of edges 13 forward 13 backward
    
    logos_node  *ln[NN]; // for sanity we don't use ln[0] 
    logos_edge  *le[NE];
    Logos  *logo = new Logos((Database *)0);

    NodeID *nid[NN];
    edge bar;
    attr_metrics *fat_path  = new attr_metrics(300, 300,   5, 30, 9, 9); //yes
    attr_metrics *med_path  = new attr_metrics(300,  50,  40, 40, 7, 7); //not2
    attr_metrics *thin_path = new attr_metrics(300,  20, 100, 50, 6, 6); //no

    attr_metrics *path_requires =
	                      new attr_metrics(100,  50,  80,100, 7, 7);
    
    unsigned char *unid = new unsigned char[22];
    unsigned char *u = unid;

    u[ 0] = 96;  // 12 bytes of bits  = 96
    u[ 1] = 160; // 20 bytes of bits  = 160

    u[ 2]=0x47; u[ 3]=0x00; u[ 4]=0x05; u[ 5]=0x80;
    u[ 6]=0xff; u[ 7]=0xde; u[ 8]=0x00; u[ 9]=0x00;
    u[10]=0x01; u[11]=0x00; u[12]=0x0b; u[13]=0x0a;

    // esi
    u[14]=0x0b; u[15]=0x0e; u[17]=0x00; u[18]=0x04; u[19]=0x05; u[20]=0x09;

    // selector
    u[21]=0;

    // create switches 1 to 10 with unique NodeID
    int logical_port=0;  // I need to test -- no real sense to this
    for (i=1; i<= 10; i++)
    {
      u[0]=96;
      u[1]=160;
      u[13]=i;	           // the 12'th byte from [2]
      nid[i]= new NodeID(&u[0]);
      ln[i] = new logos_node(nid[i],logical_port++);
      logo->Insert(ln[i]);  // insert the newnode n logo->_graph

    };
    // 88:96:4705.1111.2222.3333.4444.5555.66.667777888899.00
    //  0  1  2 3  4 5  6 7  8 9 1011 1213 14
    //         16   32   48   64   80 8896   ^-- 104
    // create 11 and 12
    u[0] = 88;
    u[1] = 96;
    u[12]++;
    nid[11] = new NodeID(&u[0]);
    ln[11] = new logos_node(nid[11]);
    logo->Insert(ln[11]);  // insert the newnode n logo->_graph

    u[12]++;
    nid[12] = new NodeID(&u[0]);
    ln[12] = new logos_node(nid[12]);
    logo->Insert(ln[12]);  // insert the newnode n logo->_graph

   
// a quick macro to create two logo edges to connect two nodes
#define ME(path,n1,n2) \
    le[e]   = new logos_edge(ln[n1], ln[n2], path, e); \
    le[e+1] = new logos_edge(ln[n2], ln[n1], path, e+1); \
    bar = logo->NewEdge(ln[n1], ln[n2], le[e]);   \
    bar = logo->NewEdge(ln[n2], ln[n1], le[e+1]); \
    e+=2

    //    M=mdediun, t= thin, (- |) = fat
    //
    //    1 M 2 - 3 - 4
    //        M   t   |
    //        5 - 6 t 7
    //        |   |   |
    //        8 t 9 - 10
    
    ME(med_path,  1,2);

    ME(fat_path,  2,3);    ME(fat_path,  3,4);

    ME(med_path,  2,5);    ME(thin_path, 3,6);    ME(fat_path,  4,7);

    ME(fat_path,  5,6);    ME(thin_path, 6,7);

    ME(fat_path,  5,8);    ME(fat_path,  6,9);    ME(fat_path,  7,10);

    ME(thin_path, 8,9);    ME(fat_path,  9,10);
    // add second path fro 2 to 5
    ME(fat_path,  2,5);

    ME(fat_path,  10,11);
    ME(fat_path,  11,12);

    //logo->SaveGraph("/dev/tty");
    //    logo->SaveGraph("legos.grph");
    //logo->ShowGraph("graph.out");

    u[13] = 8;         // this was node 8 -- lower left corner

    int dest=8;

    list<NodeID *> my_node_Heirarchy;
    //    my_node_Heirarchy.push( nid[1] );

    for (int lev=96; lev>=64; lev -= 8) {
      NodeID *fakelevel = new NodeID(nid[1]->GetNID());
      fakelevel->SetLevel(lev);
      fakelevel->SetChildLevel( (lev+8)<=96 ? lev+8: 160);
      my_node_Heirarchy.append(fakelevel);
    }
    ie_cause * cause=0;
    PNNI_crankback * crankback;
    list <PNNI_crankback *> *cb=0;

    list<transit *>*rte =
      logo->router()->RouteCompute( nid[1], nid[dest], nid[1],
				    path_requires, 0x0800,
				    cb, cause, crankback);


    cout << "route from " << *nid[1]    << endl;
    cout << "       to  " << *nid[dest] << " is " <<endl;
    if (rte)
      {
	cout << "        -- " << *nid[1] << endl;
	while (rte->size())
	  {
	    transit *tr = rte->pop();
	    NodeID *n = tr->GetNID();
	    cout << "        -- " << *n << endl;
	    delete n;
	    delete tr;
	  }
      }
    cout << endl;

    logo->Rebuild(Logos::aggregation);
    LogosGraph *aggraph=logo->GetGraph(96, Logos::aggregation);
    aggraph->SaveLogosGraph("/dev/tty");
    return 0;
}
