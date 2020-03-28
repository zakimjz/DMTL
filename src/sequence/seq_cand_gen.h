/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:  
 *           Changed cand gen for induced mining -- Zaki 7/25/06
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
#ifndef _SEQ_CAND_GEN_H_
#define _SEQ_CAND_GEN_H_

#include "count_support.h"
#include "seq_iso_check.h"
#include "seq_can_code.h"
#include "seq_operators.h"
#include "helper_funs.h"

extern unsigned long freq_pats_count;
extern bool print;

/** 
* Method performs candidate generation for sequences. 
* NOTE: Right now we are just doing pure sequences - joins which only result in a sequence.
*/
template<class PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class> class CC, template <typename> class ALLOC, class SM_TYPE>
void cand_gen(const pat_fam<SEQ_PATTERN>& Fk, const pat_fam<SEQ_PATTERN>&, int min_sup, 
              pat_fam<SEQ_PATTERN>& freq_pats, count_support<SEQ_PROP, V_Fkk_MINE_PROP, PAT_ST, CC, ALLOC, SM_TYPE >& cs, int depth) {
  
  typedef pat_fam<SEQ_PATTERN> PAT_FAM;
  typedef typename PAT_FAM::CONST_IT PF_CIT;
  typedef HASHNS::hash_map<int, int, HASHNS::hash<int> > INT_TO_INT;
  
  PF_CIT it_i, it_j;
  
  int sz=Fk.end()-Fk.begin();
  PAT_FAM pat_fams[sz]; 
  int patfam_count=0;
  INT_TO_INT patfam_index;

  for(it_i=Fk.begin(); it_i!=Fk.end(); it_i++) {
    bool skip0 = false;
    SEQ_PATTERN* pat_i = *it_i;
    
    //used during induced mining to check if the cand is embedded/induced
    //frequent
    if (!pat_i->is_freq(min_sup)) skip0 = true; 
    
    //record this pattern's index in the array of eq classes, if reqd
    if(patfam_index.find(pat_i->pat_id())==patfam_index.end()){    
      if(patfam_index.insert(make_pair(pat_i->pat_id(), patfam_count)).second)
        patfam_count++;
      else
        cerr<<"sequence-cand_gen(): patfam_index.insert failed"<<endl;
	}
    
    for(it_j=it_i; it_j!=Fk.end(); it_j++) {
      bool skip1 = false;
      
      SEQ_PATTERN* pat_j = *it_j;

      //used during induced mining to check if the cand is embedded/induced
      //frequent
      if (!pat_j->is_freq(min_sup)) skip1 = true; 

      // if both are embedded then skip these candidates
      if (skip0 && skip1) continue;
  
      if(patfam_index.find(pat_j->pat_id())==patfam_index.end())
        if(patfam_index.insert(make_pair(pat_j->pat_id(), patfam_count)).second)
          patfam_count++;
        else
          cout<<"spade.dfs_mine: patfam_index.insert failed"<<endl;
      
      
      int num_cands = 2; // Number of candidates.
      
      // Generate candidates.
      SEQ_PATTERN** cand_pats=join(pat_i, pat_j, skip0, skip1);
      
      // Perform isomorphism check.
      for(int i=0; i<num_cands; i++){
	if(cand_pats[i]){
          check_isomorphism(cand_pats[i]);
	}
      }
      
      
      // Count support of candidates
      if (cand_pats[0] || cand_pats[1])
	cs.count(pat_i, pat_j, cand_pats, min_sup, num_cands);
      
      //for (int i=0; i < num_cands; i++){
      //	if (cand_pats[i]){
      //  cout << "COUNT " << pat_i << " ** " << pat_j << " ** " << cand_pats[i] << endl;
      //}	
      //}
      
      // Discard infrequent patterns.
      for(int i=0; i<num_cands; i++) {
        if(!cand_pats[i])
          continue;
        
        if(!cand_pats[i]->is_valid(min_sup)) {
          delete cand_pats[i];
          cand_pats[i]=0;
        } 
        else {
          int idx=0;
          if(i == 0) // P->A is the pattern family.
            idx = patfam_index[pat_i->pat_id()];
          else if(i == 1) // P->B is the pattern family.
            idx = patfam_index[pat_j->pat_id()];
          
          pat_fams[idx].push_back(cand_pats[i]);
          
	  //cout << "ADD " << cand_pats[i];
          if(cand_pats[i]->is_freq(min_sup)) {
            // freq_pats.push_back(cand_pats[i]);
            if(print){
              cout << cand_pats[i];
            }
            freq_pats_count++;
          }
        }
      }
      
      delete[] cand_pats;
      
    }//end for it_j
    
    // Delete the VAT for it_i. Its never going to be used.
    cs.delete_vat(*it_i);
    
  } //end for it_i
  
  for(int i=0; i<sz; i++) {
    //recursive call
    if(!pat_fams[i].empty()) {
      // sort patterns
      //typename pat_fam<SEQ_PATTERN>::iterator b=pat_fams[i].begin(), e=pat_fams[i].end();
      //sort(b, e, less_than<SEQ_PATTERN>());
      //cout << "calling " << depth+1 << " -- " << pat_fams[i].size() << endl;
      cand_gen(pat_fams[i], pat_fams[i], min_sup, freq_pats, cs, depth+1);
    }
  }
  
} //end cand_gen()

template<class PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class > class CC, template <typename> class ALLOC >
  SEQ_PATTERN** join(const SEQ_PATTERN* pat_i, SEQ_PATTERN* pat_j, 
		     bool skip0=false, bool skip1 =false) {
  
  // NOTE: 
  // Right now we are just doing the sequence atom JOIN sequence atom.
  // Even in this case we are omitting the join that leads to an event atom.
  
  const typename SEQ_PATTERN::EDGE_T DEF_E_LBL=0;  // Default for edge label.
  SEQ_PATTERN** cand_pats=new SEQ_PATTERN*[2];
  
  // If both patterns are the same then the join results in a single new candidate pattern.
  // P->A join P->A = P->A->A (same patterns)
  // P->A join P->B = P->A->B (different patterns)
  if (skip0){
    cand_pats[0] = 0;
  }
  else{
    cand_pats[0] = pat_i->clone();
    const typename SEQ_PATTERN::VERTEX_T& new_v=pat_j->rmost_vertex();
    int old_rmv = pat_i->rmost_vid();
    int new_rmv = cand_pats[0]->add_vertex(new_v);
    cand_pats[0]->add_out_edge(old_rmv, new_rmv, DEF_E_LBL);
    cand_pats[0]->add_in_edge(new_rmv, old_rmv, DEF_E_LBL);
  }
  
  // If both patterns are the same then the second candidate is not generated.
  if(skip1 || pat_i->rmost_vertex() == pat_j->rmost_vertex()) {
    cand_pats[1] = 0;
  }
  else {
    // If the two patterns are different, join results in an additional candidate.
    // P->B join P->A = P->B->A (Additional pattern)
    cand_pats[1] = pat_j->clone();
    const typename SEQ_PATTERN::VERTEX_T& new_v=pat_i->rmost_vertex();
    int old_rmv = pat_j->rmost_vid();
    int new_rmv = cand_pats[1]->add_vertex(new_v);
    cand_pats[1]->add_out_edge(old_rmv, new_rmv, DEF_E_LBL);
    cand_pats[1]->add_in_edge(new_rmv, old_rmv, DEF_E_LBL);
  }
  
  return cand_pats;
}

#endif
