/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:
 *    added tokenizer -- Zaki, 5/8/06
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

/** \file graph_test.cpp - example test file to show usage of library classes for undirected graph mining */
/** Generic Graph Miner
 *
 * This toolkit provides a graph miner for generic objects. A graph is represented in terms of its properties.
 * The properties for itemset: undirected.
 *
 * Classes are provided to read in a dataset, compute Vertical Attribute
 * Tables (VATs), and perform the mining operation.
 * For a new object type, some of the classes may have to specialized
 * e.g. db_parser which reads in the dataset.
 *
 * The algorithm followed is gSpan, but a vertical mining approach is adopted.
 *
 * \author Nagender Parimi parimi@cs.rpi.edu, http://www.cs.rpi.edu/~parimi
 */

#include <iostream>
#include <vector>

unsigned long int freq_pats_count =0;
bool print=false;

#include "time_tracker.h"
#include "graph_iso_check.h"
#include "graph_operators.h"
#include "graph_vat.h"
#include "count_support.h"
#include "pattern.h"
#include "graph_can_code.h"
#include "graph_cand_gen_kedge.h"

#include "pat_fam.h"
#include "graph_tokenizer.h"
#include "db_reader.h"
#include "level_one_hmap.h"

#include "mem_storage_manager.h"
#define GRAPH_PR proplist<undirected >
#define GRAPH_MINE_PR proplist<Fk_F1, proplist<vert_mine > >
#define DMTL_TKNZ_PR proplist<dmtl_format>

// Defining ALLOCATOR so as to provide the flexibility to add another allocator.
#define ALLOCATOR std::allocator


time_tracker tt_db_read;
time_tracker tt_total;
int minsup;
char* infile;
int max_edges; //max number of edges in graph

void print_usage(char *prog) {
  cerr<<"Usage: "<<prog<<" -i input-filename -s minsup -k max_edges [-p]"<<endl;
  cerr<<"Input file should be in ASCII (plain text), minsup is a whole integer"<<endl;
  cerr<<"Append -p to print out frequent patterns"<<endl;
  exit(0);
}

void parse_args(int argc, char* argv[]) {
  if(argc<5) {
    print_usage(argv[0]);
  }

  for (int i=1; i < argc; ++i){
    if (strcmp(argv[i], "-i") == 0){
      infile=argv[++i];
    }
    else if (strcmp(argv[i], "-s") == 0){
      minsup=atoi(argv[++i]);
    }
    else if(strcmp(argv[i],"-p") == 0){
      print=true;
    }
    else if(strcmp(argv[i],"-k") == 0){
      max_edges=atoi(argv[++i]);
    }
    else{
      print_usage(argv[0]);
    }
  }

}//end parse_args()

template<typename PATTERN, typename L1MAP>
void populate_level_one_map(pat_fam<PATTERN>& level_one_pats, L1MAP& l1map) {
  typename pat_fam<PATTERN>::CONST_IT pf_it=level_one_pats.begin();
  while(pf_it!=level_one_pats.end()) {
    typename PATTERN::EDGE_T e;
    const typename PATTERN::VERTEX_T& src=(*pf_it)->label(0);
    const typename PATTERN::VERTEX_T& dest=(*pf_it)->label(1);
    if(!(*pf_it)->get_out_edge(0, 1, e)) {
      cerr<<" Edge not found"<<endl;
      return;
    }
    l1map.insert(src, dest, e);
    l1map.insert(dest, src, e);
    pf_it++;
  }
}

int main(int argc, char* argv[])
{

  // COMMENT: For dealing with dataset in int format. Comment out the next line and
  //          uncomment the line after that.
  typedef adj_list<std::string, int, ALLOCATOR > PAT_ST;
  // typedef adj_list<int, int, ALLOCATOR > PAT_ST;
  typedef pattern<GRAPH_PR, GRAPH_MINE_PR, PAT_ST, canonical_code, ALLOCATOR > GRAPH_PAT;
  typedef vat<GRAPH_PR, GRAPH_MINE_PR, ALLOCATOR, std::vector> GRAPH_VAT;

  level_one_hmap<GRAPH_PAT::VERTEX_T, GRAPH_PAT::EDGE_T, ALLOCATOR > l1_map;

  parse_args(argc, argv);
  pat_fam<GRAPH_PAT> level_one_pats;
  pat_fam<GRAPH_PAT> freq_pats;

  storage_manager<GRAPH_PAT, GRAPH_VAT, ALLOCATOR, memory_storage> vat_map;

  tt_db_read.start();
  tt_total.start();
  db_reader<GRAPH_PAT, DMTL_TKNZ_PR, ALLOCATOR > dbr(infile);
  tt_db_read.stop();

  dbr.get_length_one(level_one_pats, vat_map, minsup);

  cout<<endl<<level_one_pats.size()<<" frequent single-edged patterns found"<<endl;
  freq_pats=level_one_pats;

  populate_level_one_map(freq_pats, l1_map);
  count_support<GRAPH_PR, GRAPH_MINE_PR, PAT_ST, canonical_code, ALLOCATOR, memory_storage > cs(vat_map);

  pat_fam<GRAPH_PAT>::const_iterator pit;
  for(pit=level_one_pats.begin(); pit!=level_one_pats.end(); pit++)
    cand_gen(*pit, l1_map, minsup, max_edges, freq_pats, cs);
  tt_total.stop();

  if(print)
    cout<<freq_pats<<endl;
  cout<<endl<<"TIMING STATISTICS"<<endl;
  cout<<"Time taken to read db in: "<<tt_db_read.print()<<"sec"<<endl;
  cout<<"Time taken in isomorphism checks: "<<tt_iso_chk.print()<<"sec"<<endl;
  cout<<"Time taken in VAT intersection: "<<tt_vat.print()<<"sec"<<endl;
  cout<<"Total time taken: "<<tt_total.print()<<"sec"<<endl;
  cout<<endl<<endl<<freq_pats_count+level_one_pats.size()<<" total frequent graphs found"<<endl;

}//main()
