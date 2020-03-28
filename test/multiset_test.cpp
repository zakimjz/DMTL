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

/** \file multiset_test.cpp - example test file to show usage of library classes for plain multiset (integers) mining */
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ext/hash_map>
#include <algorithm>
using namespace std;


// mset_iso_check.h and pattern.h must be included at the beginning in this order
#include "mset_iso_check.h" 
#include "pattern.h"
#include "mset_vat.h"
#include "count_support.h"
#include "mset_can_code.h"
#include "mem_storage_manager.h"
#include "typedefs.h"

#include "time_tracker.h"
#include "pat_fam.h"
#include "mset_tokenizer.h"
#include "db_reader.h"
#include "mset_cand_gen.h"
#include "mset_operators.h"

#define MSET_PR proplist<no_edges >
#define MSET_MINE_PR proplist<Fk_Fk, proplist<vert_mine > >
#define DMTL_TKNZ_PR proplist<dmtl_format>

#ifdef MMALLOC
#include "mmalloc_allocator.h"
#define ALLOCATOR mmalloc_allocator
#else
#define ALLOCATOR std::allocator
#endif
unsigned long int freq_pats_count =0;
bool print=false;

void print_usage(char *prog){
  cerr<<"Usage: "<< prog <<" -i input-filename -s minsup [-p]"<<endl;
  cerr<<"Input file should be in ASCII (plain text), minsup is a whole integer"<<endl;
  cerr<<"add -p to print the patterns" << endl;
  exit(0);
}

void parse_args(int argc, char* argv[], char*& infile, int& minsup, bool& print)
{
  if(argc<5) {
    print_usage(argv[0]);
  }

  for (int i=1; i < argc; ++i) {
    if (strcmp(argv[i], "-i") == 0) {
      infile=argv[++i];
    }
    else if (strcmp(argv[i], "-s") == 0){
      minsup=atoi(argv[++i]);
    }
    else if(strcmp(argv[i],"-p") == 0){
      print=true;
    }
    else{
      print_usage(argv[0]);
    }
  }
 
}//end parse_args()

/** \fn int main(int argc, char* argv[])
* example to demonstrate plain integer-based mining
*/
int main(int argc, char* argv[])
{
  
  // COMMENT: For dealing with dataset in integer format. Comment out the next line and 
  //          uncomment the line after that.
  typedef adj_list<std::string, int, ALLOCATOR >                           PAT_ST;
  // typedef adj_list<int, int, ALLOCATOR >                                      PAT_ST;
  typedef canonical_code<MSET_PR, int, int, ALLOCATOR >                       CC;
  typedef pattern<MSET_PR, MSET_MINE_PR, PAT_ST, canonical_code, ALLOCATOR >  MULTISET;
  typedef vat<MSET_PR, MSET_MINE_PR, ALLOCATOR, std::vector >                 MSET_VAT;
  
  time_tracker tt_total;
  char* infile;
  int minsup;

  tt_total.start();
  parse_args(argc, argv, infile, minsup, print);
  
  cout<<"Command line parameters given were -"<<endl;
  cout<<"infile="<<infile<<endl;
  cout<<"minsup="<<minsup<<endl;
  cout<<"print flag="<<print<<endl;
  
  db_reader<MULTISET, DMTL_TKNZ_PR, ALLOCATOR > in_db(infile);
  pat_fam<MULTISET> level_one_pats, freq_pats;
  storage_manager<MULTISET, MSET_VAT, ALLOCATOR, memory_storage> strg_mgr;
  
  time_tracker tt_db_read;
  tt_db_read.start();
  in_db.get_length_one(level_one_pats, strg_mgr, minsup);
  tt_db_read.stop();
  cout << "Size of length one patterns = " << level_one_pats.size() << endl;
  
  count_support<MSET_PR, MSET_MINE_PR, PAT_ST, canonical_code, ALLOCATOR, memory_storage > cnt_sup(strg_mgr);
  
  freq_pats=level_one_pats;
  cand_gen(level_one_pats, level_one_pats, minsup, cnt_sup, freq_pats);
  tt_total.stop();
  
  // Print the frequent patterns.
  if(print) {
    cout<<freq_pats<<endl;
  }
  
  cout<<"The following statistics are purely for performance measurement"<<endl;
  cout<<"Wall clock time taken to read db in:"<<tt_db_read.print()<<endl;
#ifdef GET_TIME
  cout<<"Wall clock time spent in get_vat() calls:"<<tt_get_vat.print()<<endl;
  cout<<"Wall clock time spent in add_vat() calls:"<<tt_add_vat.print()<<endl;
  cout<<"Wall clock time spent in delete_vat() calls:"<<tt_delete_vat.print()<<endl;
  cout<<"Wall clock time taken to construct new multisets: "<<tt_new_mset.print()<<endl;
  cout<<"Wall clock time taken to construct new VATs: "<<tt_new_vat.print()<<endl;
#endif
  cout<<endl<<"TOTAL wall clock time taken:"<<tt_total.print()<<endl;
  
  cout<<freq_pats_count+level_one_pats.size()<<" patterns found"<<endl;
}
