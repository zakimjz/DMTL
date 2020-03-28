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
#ifndef _ISET_CAND_GEN_H_
#define _ISET_CAND_GEN_H_

#include "pattern.h"
#include "count_support.h"
#include "iset_iso_check.h"
#include "iset_vat.h"
#include "typedefs.h"

extern unsigned long int freq_pats_count;
extern bool print;

/** 
 * \fn cand_gen(pat_fam<ISET_PATTERN> Fk_one, pat_fam<ISET_PATTERN> Fk_two, int minsup, 
                count_support<ISET_PATTERN, PAT_ST, CC, SM_TYPE> cnt_sup, pat_fam<ISET_PATTERN>& freq_pats) 
  \brief This is the driver code for itemset pattern mining algorithm, it takes two equivalence classes of pattern
         family and generate candidate patterns. For each candidate patterns it check isomporphism and then check
         support. If frequent, it is stored in freq_pats vector. Function is called recursively for the newly
         generated pattern family.

  \param[in] Fk_one The Fk family of frequent patterns, where all patterns belong to the same equivalent class
  \param[in] Fk_two The second Fk family of frequent patterns, UNUSED for Itemset mining
  \param[in] minsup Minimum support threshold set by the user for this mining task
  \param[in] cnt_sup Count support class instances, to provide support counting of a pattern in backend
  \param[out] freq_pats Stores frequent patterns in every level
  \return void
  \pre Fk_one Fk_two are the same for itemset mining.
  \post Mining job finished, with all frequent patterns is in freq_pats parameter.
 *
 */
template<class PP, class MP, class PAT_ST, 
         template<class, typename, typename, template <typename> class> class CC, 
         template <typename> class ALLOC, class SM_TYPE>
void cand_gen(const pat_fam<ISET_PATTERN>& Fk_one, const pat_fam<ISET_PATTERN>& Fk_two, int minsup, 
              count_support<ISET_PROP, V_Fkk_MINE_PROP, PAT_ST, CC, ALLOC, SM_TYPE >& cnt_sup, pat_fam<ISET_PATTERN>& freq_pats) {

  typedef pat_fam<ISET_PATTERN> PAT_FAM;
  typedef typename PAT_FAM::CONST_IT PF_CIT;
  
  PF_CIT it_i, it_j;
  int num_cands = 1;
  
  for(it_i=Fk_one.begin(); it_i!=Fk_one.end(); it_i++) {
    
    PAT_FAM pat_fam_i;  // new equivalent class 
    ISET_PATTERN* pat_i=*it_i;
    
    for(it_j=it_i+1; it_j!=Fk_one.end(); it_j++) {
      
      ISET_PATTERN* pat_j=*it_j;
      
      // Generate candidates from equivalent class Fk_one.
      ISET_PATTERN** cand_pats = join(pat_i, pat_j);
      
      // Isomorphism check, for itemset every candidate patterns pass the iso-check.
      check_isomorphism(cand_pats[0]);
      
      // Count support of candidates, checking whether new patterns in cand_pats are frequent or not.
      cnt_sup.count(pat_i, pat_j, cand_pats, minsup, num_cands);
      
      // Get rid of infrequent candidate. This is itemset
      // so, we have only one candidate pattern.
      if(!cand_pats[0]->is_freq(minsup)) {
        delete cand_pats[0];
      } else {
        // pushing new and frequent candidates in new equivalent class pat_fam_i
        pat_fam_i.push_back(cand_pats[0]);
        // pushing new and frequent candidates in freq_pats for output
        //freq_pats.push_back(cand_pats[0]);
        // number of frequent patterns increase by 1
        freq_pats_count++;
        if(print)
          cout << cand_pats[0];
      }
      // delete the array of pointers holding the cand_pats 
      delete[] cand_pats;
    }
    
    cnt_sup.delete_vat(*it_i);
    
    // Recursively mine the new pattern family. Depth first pattern generation
    if(!pat_fam_i.empty()) {
      cand_gen(pat_fam_i, pat_fam_i, minsup, cnt_sup, freq_pats);
   
      // Deleting individual elements (which are pointers) in the vector. 
      for(unsigned int c=0; c < pat_fam_i.size(); c++) {
        delete pat_fam_i[c];
      }

      // Deleting the pat_fam_i after the recursion calls processed all its
      // descendent patterns.
      pat_fam_i.clear();
    }
    
  } //end for i
  
} //end cand_gen()

/**
 * \fn join(const pattern<ISET_PROP>* pat_i, const pattern<ISET_PROP>* pat_j)
   \brief Join pat_i and pat_j patterns and return new candidate patterns. ISET_PROP
   is a typedef that defines the itemset property class.

   \param[in] pat_i The first parent pattern
   \param[in] pat_j The second parent pattern
   \return An array of pattern pointers
   \pre pat_i and pat_j belongs to the same equivalent class
        pat_i < pat_j
   \post Only one candidate is generated

 * example: pat_i = {A, B, C}, pat_j = {A, B, D} 
 * join(pat_i, pat_j) returns  {A, B, C, D}
                                      
 */ 
template<class PP, class MP, class PAT_ST, 
         template<class, typename, typename, template <typename> class> class CC, 
         template <typename> class ALLOC >
ISET_PATTERN** join(const ISET_PATTERN* pat_i, ISET_PATTERN* pat_j) {
  
  ISET_PATTERN** cand_pats=new ISET_PATTERN*[1];  // an array of one element that holds pointer to a pattern
                                        // this allocation of memory is released in cand_gen() routine
                                        // after we either delete the candidate, if it is infrequent
                                        // or store it in freq_pats data structure if it is frequent 
  cand_pats[0] = pat_i->clone();   
  const typename ISET_PATTERN::VERTEX_T& rmv = pat_j->rmost_vertex();
  cand_pats[0]->add_vertex(rmv);
  
  return cand_pats;
}

#endif
