/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by salems@cs.rpi.edu
 *  Modifications:
 *           added file name passing -- Zaki 7/25/06
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
#ifndef FILE_STORAGE_MANAGER_H
#define FILE_STORAGE_MANAGER_H

#include<string>
#include<memory>
#include <ext/hash_map>
#include "pat_fam.h"
#include "pat_support.h"
#include "generic_classes.h"
#include "helper_funs.h"
#include "time_tracker.h"
#include "file_cache_manager.h"

using namespace std;

#define FILENAME "__DMTL_file_cache.dat"
/**
* \brief Storage Manager class partially specialized to File-based Storage manager to 
 * stores VAT in predefined-size buffer and uses a binary file for swapping in/out vats.
 *
 * An object store. Specifically we want to store VATs and their associated patterns. 
 * It provides routines to find/access/store VATs with a pattern key.
 */
template<class PAT, template <typename> class ALLOC, class VAT >
class storage_manager<PAT, VAT, ALLOC, file_storage > {
  
  typedef typename PAT::CC_STORAGE_TYPE C_ST;
  typedef typename PAT::CC_COMPARISON_FUNC C_CF;
  
public:
    
  typedef int key_type;
  // typename TYPE T;
  typedef VAT data_type;
  typedef VAT VATT;
  // This way you have designed your cache, so you can
  // define cache manager for your caching system.
  // Let us use a basic manager.
  
  typedef cache_manager<key_type, data_type> cache_type;
  
  typedef typename cache_type::size_type size_type;
  typedef pattern_support<typename PAT::MINE_PROPS> PAT_SUP;
  
  storage_manager(){}
  
  storage_manager(typename cache_type::size_type C=1,const char * filename=FILENAME){
    Cache = new cache_type(C, filename);
    manager_count++;   
  }
  
  //Destructor
  ~storage_manager(){
    if(manager_count==1){
      delete Cache;
    }
    manager_count--;
  }
  
  /** Returns number of the entries in the cache.
    */
  size_type mem_entries() const {
    return Cache->mem_entries();
  } // no_entries
  
  /** Returns number of the entries in the file
    */
  size_type file_entries() const {
    return  Cache->file_entries();
  } // no_entries
  
  void print_stats() const{
    Cache->print_stats();
  }
  
  bool find(PAT* const& p) const{   
    return Cache->find_vat(p->pat_id());
  }//end find()
  
  
  void delete_vat(PAT* const& p){
    Cache->delete_vat(p->pat_id());
  }
  
  void lock_vat(PAT* const& p) {
    Cache->lock_vat(p->pat_id());
  } //lock_vat

  void unlock_vat(PAT* const& p) {
    Cache->unlock_vat(p->pat_id());
  } //unlock_vat

  // get Vat* from the cache
  VAT* get_vat(PAT* const& p) const {
    VAT *value=Cache->get_vat_point(p->pat_id());
    return value;
  } //get_using_pattern

  // get Vat* from the cache-level 1
  VAT* get_vat(const key_type & id) const{
    VAT *value=Cache->get_vat_point_levelone(id);
    return value;
  } //get_using_ID

  /**
  * Generate candidate VATs for the next level, from the provided patterns.
   */
  VAT** intersect(PAT* const& p1, PAT* const& p2, PAT_SUP ** cand_sups,PAT ** cand_pats, bool is_l2) {
    // Get the vats.
    VAT* v1, *v2;
    v1 = Cache->get_vat_point(p1->pat_id());
    static int p1old=0;
    int p1new= p1->pat_id();
    if(p1new!=p1old){
      Cache->lock_vat(p1new);
      Cache->unlock_vat(p1old);      
      p1old=p1new;
    }
    
    v2 = Cache->get_vat_point(p2->pat_id());
        
    if(v1 == 0 || v2 == 0) {
      cout<< "file_storage_manager: vat not found for pattern = " << p1->pat_id() << endl;
      return NULL;
    }
    // TODO: Lock v1 and v2. 
    return VAT::intersection(v1, v2, cand_sups, cand_pats,is_l2);
  }
  
  //adding vat to the cache--not in level 1
  bool add_vat(const PAT* const& p, VAT* v){
    bool ret;
    ret=Cache->insert(p->pat_id(),v);
    return ret;
  }//end add_vat()
  
  //adding vat to the cache--not in level 1
  void write_file(const key_type & id,VAT* v){
    Cache->write_file(id,*v);
  }//end add_vat()
  
  //adding vat to the cache--level 1
  bool my_add_vat(const key_type & id, VAT* v){
    bool ret;
    ret=Cache->insert_levelone(id,v);
    //delete v;
    return ret;
  }//end add_vat()
  
  /** initialize_vat(map<int, int>)
  *\param map(int, int): a map data structure that contains
  * the size of each level-1 patterns
  */
  void initialize_vat(std::map<int, int> & mapcount) {
    std::map<int, int>::iterator it;
    for (it = mapcount.begin(); it != mapcount.end(); ++it) {
      VAT* ivat=new VAT();
      ivat->resize(it->second);  
      my_add_vat(it->first,ivat);
    }
  } //end initialize_vat

private:
  cache_type* Cache;
  
  static int manager_count; // Ensures that we do not delete the file twice.
}; // class StorageManager

template<class PAT, template <typename> class ALLOC, class VAT >
int storage_manager<PAT, VAT, ALLOC, file_storage >::manager_count=0;

#endif
