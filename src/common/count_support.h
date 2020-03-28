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
#ifndef _COUNT_SUPPORT_H_
#define _COUNT_SUPPORT_H_

#include "pattern.h"
#include "adj_list.h"
#include "mem_storage_manager.h"
#include "generic_classes.h"


/** 
* \brief count_support class partially specialized for vertical mining.
*
*/
template<class PP, class JOIN_TYPE, class TRANS, class ST, template<class, typename, typename, template <typename> class > class CC, 
template <typename> class ALLOC, class SM_TYPE >
class count_support<PP, proplist<JOIN_TYPE, proplist<vert_mine, TRANS > >, ST, CC, ALLOC, SM_TYPE >
{
  
public:
  typedef proplist<JOIN_TYPE, proplist<vert_mine, TRANS > > MINING_PROPS;
  typedef ST PAT_ST_TYPE;
  typedef PP PATTERN_PROPS;
  // typedef SM_TYPE STORAGE_MANAGER_TYPE;
  typedef pattern<PATTERN_PROPS, MINING_PROPS, PAT_ST_TYPE, CC, ALLOC > PATTERN;
  typedef vat<PATTERN_PROPS, MINING_PROPS, ALLOC, std::vector > VAT;
  typedef pattern_support<MINING_PROPS> PAT_SUP;
  
  count_support(storage_manager<PATTERN, VAT, ALLOC, SM_TYPE > const& sm) : _strg_mgr(sm) {} 
  
  // function to count support of candidate patterns
  // cand_supports is populated, num is # of candidates generated
  void count(PATTERN* const& p1, PATTERN* const& p2, PATTERN** const& cand_pats, const int& minsup, const int& num) {
    
    // invoke storage_mgr's intersect to get VATs and support for candidates
    VAT** cand_vats; // pointer to VAT ptrs for candidates
    PAT_SUP** cand_sups=new PAT_SUP*[num];
    
    int i;
    for(i=0; i<num; i++)
      if(cand_pats[i])
        cand_sups[i]=new PAT_SUP;
      else
        cand_sups[i]=0;
    
    bool is_l2=(p1->size()==1);
    
    // intersect() is expected to populate the support member 
    // of each cand_pat
    cand_vats= _strg_mgr.intersect(p1, p2, cand_sups, cand_pats, is_l2);
    
    // check which candidates were frequent
    // and add their VATs to strg_mgr
    for(i=0;i<num;i++) {
      if(!cand_pats[i])
        continue;
      
      //if(cand_pats[i]->is_valid(minsup))
      if(cand_sups[i]->is_valid(minsup)) {
        cand_pats[i]->set_support(cand_sups[i]);
        _strg_mgr.add_vat(cand_pats[i], cand_vats[i]);
      }
      else {
        //reclaim memory
        delete cand_vats[i];
      }
      delete cand_sups[i];
    }//end for
    
    delete[] cand_sups;
    delete[] cand_vats;
    
  }//end count()
  
  void delete_vat(PATTERN* const& p) {
    _strg_mgr.delete_vat(p);
  }
  
private:
  storage_manager<PATTERN, VAT, ALLOC, SM_TYPE> _strg_mgr;
  
}; //end class count_support()

#endif
