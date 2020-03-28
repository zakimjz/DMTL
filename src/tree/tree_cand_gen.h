/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
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
// candidate generation for trees

#ifndef _TREE_CAND_GEN_H
#define _TREE_CAND_GEN_H

#include <algorithm>

#include "tree_iso_check.h"
#include "tree_operators.h"
#include "typedefs.h"

extern unsigned long freq_pats_count;
extern bool print;

template<class PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class> class CC, 
template<typename> class ALLOC, class SM_TYPE>

void cand_gen(const pat_fam<TREE_PATTERN >& Fk, const pat_fam<TREE_PATTERN >&, const int& minsup, 
              pat_fam<TREE_PATTERN >& freq_pats, count_support<TREE_PROP, V_Fkk_MINE_PROP, PAT_ST, CC, ALLOC, SM_TYPE>& cs) {
  
  typedef pat_fam<TREE_PATTERN> PAT_FAM;
  typedef typename PAT_FAM::CONST_IT PF_CIT;
  
  PF_CIT it_i, it_j;
  const int num_cands=2;
  
  for(it_i=Fk.begin(); it_i!=Fk.end(); it_i++) {
    
    PAT_FAM pat_fam_i;
    TREE_PATTERN* pat_i=*it_i;
    
    // check if this pattern should be extended
    if(!pat_i->is_freq(minsup) || !pat_i->is_canonical())
      continue;
    
    int pos_i=compute_pos(pat_i);
    for(it_j=Fk.begin(); it_j!=Fk.end(); it_j++) {
      
      TREE_PATTERN* pat_j=*it_j;
      int i;
      
      // check if valid candidate can result
      int pos_j=compute_pos(pat_j);
      
      if(pos_i<pos_j) {
        continue;
      }
      
      // generate new candidates
      TREE_PATTERN** cand_pats=join(pat_i, pat_j, pos_i, pos_j);
      
      for(i=0; i<num_cands; i++)
        if(cand_pats[i])
          check_isomorphism(cand_pats[i]);
      
      // count support of candidates
      cs.count(pat_i, pat_j, cand_pats, minsup, num_cands);
      
      // discard infrequent patterns
      for(i=0; i<num_cands; i++) {
        if(!cand_pats[i])
          continue;
        
        if(!cand_pats[i]->is_valid(minsup)) {
          delete cand_pats[i];
          cand_pats[i]=0;
        }
        else {
          pat_fam_i.push_back(cand_pats[i]);
          if(cand_pats[i]->is_canonical() && cand_pats[i]->is_freq(minsup)) {
            // freq_pats.push_back(cand_pats[i]);
            freq_pats_count++;
            if(print)
              cout << cand_pats[i];
          }
        }
      }
      
      // cleanup for this iteration
      delete[] cand_pats;
      
    }//end for it_j
    
    // recursive call
    if(!pat_fam_i.empty())
      cand_gen(pat_fam_i, pat_fam_i, minsup, freq_pats, cs);
    
  }//end for it_i
  
  // cleanup VATs for entire equivalence class
  for(it_i=Fk.begin(); it_i!=Fk.end(); it_i++) {
    cs.delete_vat(*it_i);
    TREE_PATTERN* pat_i=*it_i;
    if(!pat_i->is_freq(minsup) || !pat_i->is_canonical())
      delete pat_i;
  }
}//end cand_gen()

template<class PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class > class CC, template <typename> class ALLOC>
TREE_PATTERN** join(const TREE_PATTERN* pat_i, TREE_PATTERN* pat_j, const int& pos_i, const int& pos_j) {
  // NOTE: follows scheme of TreeMinerV, perhaps SLEUTH may be more elegant ??
  
  // NOTE: convention is that cand_pats[0] is cousin extension, 
  // cand_pats[1] is child extension
  // This convention is used by vat::intersect
  const typename TREE_PATTERN::EDGE_T E_LBL=0; //default, defunt edge label for trees
  TREE_PATTERN** cand_pats=new TREE_PATTERN*[2];
  
  if(pos_i==pos_j && pat_i->size()>1) {
    // case Ia of paper
    // construct first candidate - cousin extension
    cand_pats[0]=pat_i->clone();
    const typename TREE_PATTERN::VERTEX_T& new_v=pat_j->rmost_vertex();
    int rvid=cand_pats[0]->add_vertex(new_v);
    cand_pats[0]->add_out_edge(pos_j, rvid, E_LBL);
    cand_pats[0]->add_in_edge(rvid, pos_j, E_LBL);
    
    //construct second candidate - child extension
    cand_pats[1]=pat_i->clone();
    rvid=cand_pats[1]->add_vertex(new_v);
    cand_pats[1]->add_out_edge(pat_i->rmost_vid(), rvid, E_LBL);
    cand_pats[1]->add_in_edge(rvid, pat_i->rmost_vid(), E_LBL);
    
    return cand_pats;
  }//end if case Ia
  
  // execution reaches this point iff num_cands=1
  if(pos_i>pos_j) {
    // case II - cousin extension
    // construct candidate
    cand_pats[0]=pat_i->clone();
    const typename TREE_PATTERN::VERTEX_T& new_v=pat_j->rmost_vertex();
    int rvid=cand_pats[0]->add_vertex(new_v);
    cand_pats[0]->add_out_edge(pos_j, rvid, E_LBL);
    cand_pats[0]->add_in_edge(rvid, pos_j, E_LBL);
    cand_pats[1]=0;
    return cand_pats;
  }
  
  // assert: this is case Ib - child extension
  cand_pats[0]=0;
  cand_pats[1]=pat_i->clone();
  const typename TREE_PATTERN::VERTEX_T& new_v=pat_j->rmost_vertex();
  int rvid=cand_pats[1]->add_vertex(new_v);
  cand_pats[1]->add_out_edge(pat_i->rmost_vid(), rvid, E_LBL);
  cand_pats[1]->add_in_edge(rvid, pat_i->rmost_vid(), E_LBL);
  return cand_pats;
}//end join()

#endif
