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
#ifndef MEM_STORAGE_MANAGER_H_
#define MEM_STORAGE_MANAGER_H_

#include<memory>
#include <ext/hash_map>
#include "pat_fam.h"
#include "pat_support.h"
#include "generic_classes.h"
#include "helper_funs.h"
#include "time_tracker.h"

using namespace std;

// time_tracker tt_tot_inter;

/**
* \brief Storage Manager class partially specialized to Memory-based Storage manager to 
 * stores VAT in Memory.
 *
 * An object store. Specifically we want to store VATs and their associated patterns. 
 * It provides routines to find/access/store VATs with a pattern key.
 */

template<class PAT, template <typename> class ALLOC, class VAT >
class storage_manager<PAT, VAT, ALLOC, memory_storage> 
{
  
  typedef typename PAT::CC_STORAGE_TYPE C_ST;
  typedef typename PAT::CC_COMPARISON_FUNC C_CF;
  // typedef typename VAT::VAT_ALLOC ALLOC;
  
public:
    
  typedef HASHNS::hash_map<C_ST, VAT*, HASHNS::hash<C_ST>, C_CF, ALLOC<pair<const C_ST, VAT*> > > CC_ST_TO_VATPTR;
  typedef typename CC_ST_TO_VATPTR::const_iterator CONST_IT;
  typedef typename CC_ST_TO_VATPTR::iterator IT;
  typedef pattern_support<typename PAT::MINE_PROPS> PAT_SUP;
  
  storage_manager() {}
  
  storage_manager(pat_fam<PAT>& freq_one_pats, vector<VAT*>& freq_one_vats) {
    
    typename pat_fam<PAT>::CONST_IT cit = freq_one_pats.begin();
    int idx = 0;
    
    while(cit != freq_one_pats.end()) {
      PAT* p = *cit;
      
      _pat_to_vat.insert(make_pair(p->pat_id(), freq_one_vats[idx++]));
      cit++;
    }
  }
  
  /**
    * Return true if the pattern is found in the storage manager.
   */
  bool find(PAT* const& p) const {
    CONST_IT hmap_it;
    hmap_it=_pat_to_vat.find(p->pat_id());
    
    return !(hmap_it==_pat_to_vat.end());
  }//end find()
  
  /** 
    * Return the vat corresponding to the pattern.
    */  
  VAT* get_vat(PAT* const& p) const {
    
    CONST_IT hmap_it;
    hmap_it=_pat_to_vat.find(p->pat_id());
    if(hmap_it!=_pat_to_vat.end()) {
      //vat found
      return hmap_it->second;
    }
    else {
      return 0;
    }
  }
  
  /**
    * Delete the vat.
   */
  void delete_vat(PAT* const& p) {
    
    IT hmap_it;
    hmap_it=_pat_to_vat.find(p->pat_id());
    if(hmap_it!=_pat_to_vat.end()) {
      VAT* vat=hmap_it->second;
      if(vat)
        delete vat; //reclaim memory for vat
      else
        std::cout<<"storage_manager.delete_vat: vat is null"<<endl;
      _pat_to_vat.erase(hmap_it); //erases pat-vat mapping from hashmap
    }
    else {
      std::cout<<"storage_manager.delete_vat:vat not found for "<<p->pat_id()<<endl;
    }
  }
  
  /** 
    * Map the pattern to the VAT.
    */
  bool add_vat(PAT* const& p, VAT* v) {
    bool ret=(_pat_to_vat.insert(make_pair(p->pat_id(), v))).second;
    
    return ret;
  }
  
  /**
    * Generate candidate VATs for the next level, from the provided patterns.
   */
  VAT** intersect(PAT* const& p1, PAT* const& p2, PAT_SUP** cand_sups, PAT** cand_pats, bool is_l2) {
    
    // Get the vats.
    VAT* v1, *v2;
    
    CONST_IT hmap_it;
    hmap_it = _pat_to_vat.find(p1->pat_id());
    if(hmap_it!=_pat_to_vat.end()) {
      //vat found
      v1 = hmap_it->second;
    } 
    else {
      cout<< "storage_manager: vat not found for pattern = " << p1->pat_id() << endl;
      return 0;
    }
    
    hmap_it = _pat_to_vat.find(p2->pat_id());
    if(hmap_it!=_pat_to_vat.end()) {
      //vat found
      v2 = hmap_it->second;
    } 
    else {
      cout<< "storage_manager: vat not found for pattern = " << p2->pat_id() << endl;
      return 0;
    }
    
    VAT** ret = VAT::intersection(v1, v2, cand_sups, cand_pats, is_l2);
    
    return ret;
  }
  
  void print() const {
    CONST_IT hmap_it;
    for(hmap_it=_pat_to_vat.begin();hmap_it!=_pat_to_vat.end();hmap_it++)
      cout<<hmap_it->first<<"->"<<hmap_it->second<<endl;
  }// end print()
  
private:
    
  CC_ST_TO_VATPTR _pat_to_vat; // A pattern to vat map
};

#endif
