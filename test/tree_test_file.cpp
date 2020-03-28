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

// test driver file for tree mining (embedded and induced working)

#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ext/hash_map>
#include <algorithm>
using namespace std;

// tree_iso_check.h and pattern.h must be included at the beginning in this order
#include "tree_iso_check.h"
#include "pattern.h"
#include "tree_vat.h"
#include "count_support.h"
#include "tree_can_code.h"
#include "mem_storage_manager.h"
#include "file_storage_manager.h"
#include "typedefs.h"

#include "time_tracker.h"
#include "pat_fam.h"
#include "tree_tokenizer.h"
#include "db_reader.h"
#include "tree_cand_gen.h"
#include "tree_operators.h"


#define ORD_TREE_PR proplist<directed, proplist<acyclic, proplist<indegree_lte_one, proplist<ordered > > > > 
#define UNORD_TREE_PR proplist<directed, proplist<acyclic, proplist<indegree_lte_one, proplist<unordered > > > > 
#define V_Fkk_EMB_MINE_PR proplist<Fk_Fk, proplist<vert_mine, proplist<embedded > > >
#define V_Fkk_IND_MINE_PR proplist<Fk_Fk, proplist<vert_mine, proplist<induced > > >
#define DMTL_TKNZ_PR proplist<dmtl_format>

// Defining ALLOCATOR so as to provide the flexibility to add another allocator.
#define ALLOCATOR std::allocator

#define MEG 1024*1024
unsigned long int freq_pats_count=0;

time_tracker tt_db_read;
time_tracker tt_total;
bool print=false;
unsigned long long init_cache_size;
char *infile;
int minsup;

void print_usage(char *prog){
  cerr<<"Usage: "<<prog<<" -i input-filename -s minsup <-I|-E> <-O|-U> [-p] [-f file buffer size in MB]"<<endl;
  cerr<<"Input file should be in ASCII (plain text), minsup is a whole integer"<<endl;
  cerr<<"One of <-I|-E> and <-O|-U> each are required arguments"<<endl;
  cerr<<"Append -p to print out all frequent trees to stdout"<<endl;
  cerr<<"-f specifies the size of the memory buffer. Default: 512MB."<<endl;
  exit(0);
}

void parse_args(int argc, char* argv[], char*& infile, int& minsup, bool& print, bool& induced, bool& ordered, unsigned long long & init_cache_size) {  
  init_cache_size=512*MEG;
  minsup = 1;
  print = false;
  
  if(argc<4) {
    print_usage(argv[0]);
  }

  for (int i=1; i < argc; ++i){
    if (strcmp(argv[i], "-i") == 0){
      infile=argv[++i];
    }
    else if (strcmp(argv[i], "-s") == 0){
      minsup=atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-I") == 0){
      induced=true;
    }
    else if(strcmp(argv[i],"-E") == 0){
      induced=false;
    }
    else if (strcmp(argv[i], "-O") == 0){
      ordered=true;
    }
    else if(strcmp(argv[i],"-U") == 0){
      ordered=false;
    }
    else if(strcmp(argv[i],"-p") == 0){
      print=true;
    }
    else if (strcmp(argv[i], "-f") == 0){
      init_cache_size=(unsigned long long) (atof(argv[++i])*MEG);
    }
    else{
      print_usage(argv[0]);
    }
  }
}//end parse_args()

template<typename PAT_PROPS, typename MINE_PROPS>
void mine() {
  // COMMENT: For dealing with dataset in integer format. Comment out the next line and 
  //          uncomment the line after that.
  typedef adj_list<std::string, int, ALLOCATOR > PAT_ST;
  // typedef adj_list<int, int> PAT_ST;
  typedef canonical_code<PAT_PROPS, int, int, ALLOCATOR > CAN_CODE;
  typedef pattern<PAT_PROPS, MINE_PROPS, PAT_ST, canonical_code, ALLOCATOR > TREE_PAT;
  
  typedef vat<PAT_PROPS, MINE_PROPS, ALLOCATOR, std::vector> TREE_VAT;
  
  storage_manager<TREE_PAT, TREE_VAT, ALLOCATOR, memory_storage> vat_map;
  db_reader<TREE_PAT, DMTL_TKNZ_PR, ALLOCATOR> in_db(infile);
  
  pat_fam<TREE_PAT> level_one_pats;
  pat_fam<TREE_PAT> freq_pats;
  
  tt_db_read.start();
  in_db.get_length_one(level_one_pats, vat_map, minsup);
  tt_db_read.stop();
  
  cout<<level_one_pats.size()<<" level-1 patterns found"<<endl;

  
  storage_manager<TREE_PAT, TREE_VAT, ALLOCATOR,  file_storage> strg_mgr_file(init_cache_size);
  
  typename pat_fam<TREE_PAT>::CONST_IT cit = level_one_pats.begin();

  while(cit != level_one_pats.end()) {
    TREE_PAT* p = *cit;
    strg_mgr_file.add_vat(p, vat_map.get_vat(p));
    cit++;
  }

  
  // count_support<PAT_PROPS, MINE_PROPS, PAT_ST, canonical_code, ALLOCATOR, memory_storage> cs(vat_map);
  count_support<PAT_PROPS, MINE_PROPS, PAT_ST, canonical_code, ALLOCATOR, file_storage> cs(strg_mgr_file);
  
  

  freq_pats=level_one_pats;
  cand_gen(level_one_pats, level_one_pats, minsup, freq_pats, cs);
  tt_total.stop();
  
  if(print) {
    cout<<freq_pats<<endl;
  }

  cout<<freq_pats_count+level_one_pats.size()<<" total patterns found"<<endl;
}//end mine()

/** \fn int main(int argc, char* argv[])
 * example to demonstrate plain integer-based mining
 */
int main(int argc, char* argv[]) {
  bool induced_f=false, ordered_f=false;   // Boolean to choose between embedded/induced 
                                           // or ordered/unordered

  tt_total.start();                        // For timing calculation
  parse_args(argc, argv, infile, minsup, print, induced_f, ordered_f, init_cache_size); // parsing the argument


  // Arguments printing ...

  cout<<"Command line parameters given were -"<<endl;
  cout<<"infile="<<infile<<endl;
  cout<<"minsup="<<minsup<<endl;
  cout<<"print flag="<<print<<endl;
  cout<<"induced flag="<<induced_f<<endl;
  cout<<"ordered flag="<<ordered_f<<endl;


  // Choosing among 4 different algorithms.

  if(induced_f && ordered_f) 
    mine<ORD_TREE_PR, V_Fkk_IND_MINE_PR >();                       // Calling mine() algorithm

  else if(induced_f && !ordered_f)
    mine<UNORD_TREE_PR, V_Fkk_IND_MINE_PR >();                     // Calling mine() algorithm

  else if(!induced_f && ordered_f)
    mine<ORD_TREE_PR, V_Fkk_EMB_MINE_PR>();                        // Calling mine() algorithm

  else
    mine<UNORD_TREE_PR, V_Fkk_EMB_MINE_PR>();                      // Calling mine() algorithm


  // Printing Outputs

  cout<<"Timing Data =================:"<<endl;
  cout<<"Wall clock time to read db in:"<<tt_db_read.print()<<endl;
#ifdef GET_TIME
  cout<<"Wall clock time in get_vat() calls:"<<tt_get_vat.print()<<endl;
  cout<<"Wall clock time in add_vat() calls:"<<tt_add_vat.print()<<endl;
  cout<<"Wall clock time in delete_vat() calls:"<<tt_delete_vat.print()<<endl;
  cout<<"Wall clock time to construct new itemsets: "<<tt_new_iset.print()<<endl;
  cout<<"Wall clock time to construct new VATs: "<<tt_new_vat.print()<<endl;
#endif
  cout<<endl<<"TOTAL wall clock time taken:"<<tt_total.print()<<endl; 
  
}

