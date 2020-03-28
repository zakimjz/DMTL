/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:
 *      added tokenizer -- Zaki, 5/8/06
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

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ext/hash_map>
#include <algorithm>
using namespace std;


// seq_iso_check.h and pattern.h must be included at the beginning in this order
#include "seq_iso_check.h"
#include "pattern.h"
#include "seq_vat.h"
#include "count_support.h"
#include "seq_can_code.h"
#include "mem_storage_manager.h"
#include "file_storage_manager.h"
#include "typedefs.h"


#include "time_tracker.h"
#include "pat_fam.h"
#include "seq_tokenizer.h"
#include "db_reader.h"
#include "seq_cand_gen.h"
#include "seq_operators.h"

#define SEQ_PR proplist<directed, proplist<acyclic, proplist<indegree_lte_one, proplist<outdegree_lte_one> > > >
#define V_Fkk_IND_MINE_PR proplist<Fk_Fk, proplist<vert_mine, proplist<induced> > >
#define V_Fkk_EMB_MINE_PR proplist<Fk_Fk, proplist<vert_mine, proplist<embedded> > >
#define DMTL_TKNZ_PR proplist<dmtl_format>

// Defining ALLOCATOR so as to provide the flexibility to add another allocator.
#define ALLOCATOR std::allocator

#define MEG 1024*1024;
// Some Global variables.

char* infile;
int minsup;
bool print=false;
unsigned long long init_cache_size;

time_tracker tt_total;
time_tracker tt_db_read;
unsigned long int freq_pats_count=0;

void print_usage(char *prog) {
  cerr<<"Usage: "<< prog <<" -i input-filename -s minsup [-I|-E] [-p] [-f buffer size in MB]"<<endl;
  cerr<<"Input file should be in ASCII (plain text), minsup is a whole integer"<<endl;
  cerr<<"add -p to print the patterns" << endl;
  cerr<<"default is -E (embedded), use -I for induced" << endl;
  cerr<<"-f specifies the size of the memory buffer. Default: 512MB."<<endl;
  exit(0);
}

void parse_args(int argc, char* argv[], char*& infile, int& minsup, 
                bool& print, bool &induced, unsigned long long&  init_cache_size) {
  minsup = 1;
  induced = false;
  print = false;
  init_cache_size = 512*MEG;
  
  if(argc<4){
    print_usage(argv[0]);
  }
  for (int i=1; i < argc; ++i){
    if (strcmp(argv[i], "-i") == 0){
      infile=argv[++i];
      //cout << "INFILE = " << infile << endl;
    }
    else if (strcmp(argv[i], "-s") == 0){
      minsup=atoi(argv[++i]);
      //cout << "MINSUP = " << minsup << endl;
    }
    else if(strcmp(argv[i],"-p") == 0){
      print=true;
      //cout << "PRINT = 1" << endl;
    }
    else if (strcmp(argv[i], "-I") == 0){
      induced = true;
    }
    else if (strcmp(argv[i], "-E") == 0){
      induced = false;
    }
    else if (strcmp(argv[i], "-f") == 0){
      init_cache_size= (unsigned long long) atof(argv[++i])*MEG;
      cout << "INIT CACHE SIZE " << init_cache_size << endl;
    }
    else{
      print_usage(argv[0]);
    }
  }
  
}//end parse_args()

template<typename MINE_PROPS>
void mine() {

  // COMMENT: For dealing with dataset in std::string format. Comment out the next line and 
  //          uncomment the line after that.
  // typedef adj_list<int, int>                                                       PAT_ST;
  typedef adj_list<std::string, int, ALLOCATOR>                                       PAT_ST;
  typedef canonical_code<SEQ_PR, int, int, ALLOCATOR >                                CC;
  typedef pattern<SEQ_PR, MINE_PROPS, PAT_ST, canonical_code, ALLOCATOR>       SEQ_PAT;

  typedef vat<SEQ_PR, MINE_PROPS, ALLOCATOR, std::vector>                             SEQ_VAT;
  storage_manager<SEQ_PAT, SEQ_VAT, ALLOCATOR, memory_storage> strg_mgr;
  db_reader<SEQ_PAT, DMTL_TKNZ_PR, ALLOCATOR > in_db(infile);

  pat_fam<SEQ_PAT> level_one_pats, freq_pats;

  tt_db_read.start();
  in_db.get_length_one(level_one_pats, strg_mgr, minsup);
  tt_db_read.stop();

  storage_manager<SEQ_PAT, SEQ_VAT, ALLOCATOR, file_storage> strg_mgr_file(init_cache_size);
  
  typename pat_fam<SEQ_PAT>::CONST_IT cit = level_one_pats.begin();

  while(cit != level_one_pats.end()) {
    SEQ_PAT* p = *cit;
    strg_mgr_file.add_vat(p, strg_mgr.get_vat(p));
    cit++;
  }
  
  //count_support<SEQ_PROP, V_Fkk_EMB_MINE_PROP, PAT_ST, canonical_code, memory_storage > cnt_sup(strg_mgr);
  count_support<SEQ_PR, MINE_PROPS, PAT_ST, canonical_code, ALLOCATOR, file_storage > cnt_sup(strg_mgr_file);
  cout << "SIZE OF LEN 1 VATS = " << level_one_pats.size() << endl;
  cout<<"Wall clock time taken to read db in:"<<tt_db_read.print()<<endl;
  freq_pats=level_one_pats;
  cand_gen(level_one_pats, level_one_pats, minsup, freq_pats, cnt_sup, 0);
  tt_total.stop();

  if(print) {
    cout<<freq_pats<<endl;
  }
  cout<<freq_pats_count+level_one_pats.size()<<" total patterns found"<<endl;
}
/** \fn int main(int argc, char* argv[])
 * example to demonstrate plain integer-based mining
 */
int main(int argc, char* argv[]) {

  bool induced_f;                                  //Boolean to choose between Induced/Embedded Mining
 
  tt_total.start();
  parse_args(argc, argv, infile, minsup, print, induced_f, init_cache_size);

  cout<<"Command line parameters given were -"<<endl;
  cout<<"infile="<<infile<<endl;
  cout<<"minsup="<<minsup<<endl;
  cout<<"print flag="<<print<<endl;
  cout <<"Induced flag="<<induced_f<<endl;

  if (induced_f) {
    mine<V_Fkk_IND_MINE_PR>();
  } else {

    mine<V_Fkk_EMB_MINE_PR>();
  }

  // Printing Outputs
  //cout<<"Total patterns:" << freq_pats_count+ << endl;
  cout<<"The following statistics are purely for performance measurement"<<endl;
  cout<<"Wall clock time taken to read db in:"<<tt_db_read.print()<<endl;
#ifdef GET_TIME
  cout<<"Wall clock time spent in get_vat() calls:"<<tt_get_vat.print()<<endl;
  cout<<"Wall clock time spent in add_vat() calls:"<<tt_add_vat.print()<<endl;
  cout<<"Wall clock time spent in delete_vat() calls:"<<tt_delete_vat.print()<<endl;
  cout<<"Wall clock time taken to construct new itemsets: "<<tt_new_iset.print()<<endl;
  cout<<"Wall clock time taken to construct new VATs: "<<tt_new_vat.print()<<endl;
#endif
  cout<<endl<<"TOTAL wall clock time taken:"<<tt_total.print()<<endl;

}
