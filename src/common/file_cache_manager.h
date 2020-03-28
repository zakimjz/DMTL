/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by salems@cs.rpi.edu
 *  Modifications:
 *           added file cache deleting, and name passing -- Zaki 7/25/06
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
#ifndef FILE_CACHE_MANAGER_H
#define FILE_CACHE_MANAGER_H

#include<cstdio>
#include<map>
using namespace std;

/**
 * \brief Class manages the memory cache for the file-based storage manager.
 *        cache_manager is responsible for bringing objects into memory and 
 *        implementing eviction algorithm when the cache is full and a new 
 *        object needs to be brought in.
 */
template <typename Key, typename Data>
class cache_manager {

 public:
  /**
  */
  typedef Key key_type;

  /**
   */
  typedef Data data_type;

  typedef std::map<Key, std::pair<bool,data_type*> > storage_type;

  /**
   */
  typedef unsigned long long size_type;

  /**
   */
  typedef std::map< key_type, std::pair<unsigned long long,unsigned long long> , std::less< key_type > > FileMap;
    
  typedef typename FileMap::iterator File_Map_iter;
  /**
   */

  /**
   */
  typedef typename storage_type::iterator storage_iterator;    
    
  /**
   */
  typedef typename storage_type::const_iterator storage_const_iterator;
    
  /**
   */
  typedef typename storage_type::value_type storage_entry_type;

  
  /** Returns iterator to the beginning of the entries storage.
   */
  storage_iterator begin() { return istorage_.begin(); }

  /** Returns iterator to the end of the internal storage.
   */
  storage_iterator end() { return istorage_.end(); }

  /** Returns const iterator to the beginning of the entries storage.
   */
  storage_const_iterator begin() const { return istorage_.begin(); }

  /** Returns const iterator to the end of the internal storage.
   */
  storage_const_iterator end() const { return istorage_.end(); }

  /** Constructs cache_manager with capacity C
   *  and sets policies and statistics object.
   *  @param C is cache capacity.
   *  @param filename is the file name that 
   *  will keep objects in case of their eviction
   */
    
  // C can be equal 0.
  explicit cache_manager(size_type C,const char* filename)
    : size_(0), capacity_(C), istorage_(), offset(0), file_name(filename){
    my_file_.open(file_name.c_str(), 
		  std::ios::in|std::ios::out|std::ios::binary|ios::trunc);   

  } // explicit cache_manager
    
    
  ~cache_manager() {
    my_file_.close();
    
    if (remove(file_name.c_str()) < 0){
      cerr << "Unable to remove backend file " << file_name << endl;
    }
  }
  
  /** Clears the memory map and the file map
   */
  void clear() {
    
    size_ = 0;
    istorage_.clear();
    file_map_.clear();
  } // clear

  /** Returns current size of the cache.
   */
  size_type size() const { 
    size_type S = size_;
    return S; 
  } // size

    
  /** Returns capacity of the cache.
   */
  size_type capacity() const {
    size_type S = capacity_;
    return S;
  } // capacity

  /** Returns number of the entries in the cache.
   */
  size_type mem_entries() const {
    size_type S = istorage_.size();
    return S;
  } // no_entries

  /** Prints some statistics about the map
   */
  void print_stats() const {
    //cout<<"size:"<<size_<<"offset:"<<offset<<endl;
  } 
  /** Returns number of the entries in the file
   */
  size_type file_entries() const {
    size_type S = file_map_.size();
    return S;
  } // no_entries


    
  /** Finds entry associated with a given key.
   */
  storage_const_iterator find(const key_type& k) const {
    storage_const_iterator iter(istorage_.find(k));
    return iter;
  } // find
  
  void lock_vat(const key_type& k) {
    storage_iterator iter(istorage_.find(k));
    if (iter != istorage_.end()) {
        iter->second.first=1;
    } //if
  } //lock_vat

  void unlock_vat(const key_type& k) {
    storage_iterator iter(istorage_.find(k));
    if (iter != istorage_.end()) {
      iter->second.first=0;
    } //if
  } //unlock_vat

  /** Finds if there is an  entry associated with a given key.
   *  If such exists , delete it from both the memory map and the file map.
   */
  void  delete_vat(const key_type& k){
    storage_iterator iter(istorage_.find(k));
    if (iter != istorage_.end()){ //found in the memory_map
      size_ -= iter->second.second->byte_size();
      data_type* v =iter->second.second;
      if(v){
	delete v; //reclaim memory for vat
      }
      else 
        std::cout<<"cache_manager.delete_vat: vat is null"<<endl;
      
      istorage_.erase(iter); 
    } // if end
    file_map_iter = file_map_.find(k);
    if(file_map_iter != file_map_.end()){   //found in the file_map
      file_map_.erase(file_map_iter);
    }

  }

  /** Finds if there is an  entry associated with a given key.
   *  If such exists it returns true.
   *  Please note, we are looking in the memory map and in the file map.
   */ 
  bool find_vat ( const key_type& k ){
    storage_iterator iter(istorage_.find(k));  // TODO: For compilation sake: Added * before k.
    if (iter != istorage_.end()){ //found in the memory_map    
      return true;
    }
    file_map_iter = file_map_.find(k);  // TODO: For compilation sake: Added * before k.
    if(file_map_iter != file_map_.end()){   //found in the file_map        
      return true;
    }
    return false; 
  } 
  
  /**
   * Return a pointer to the vat that corresponds to the pattern
   */
  data_type * get_vat_point(const key_type& k) {
    storage_iterator iter(istorage_.find(k));
    if (iter != istorage_.end()) {
      return iter->second.second;
    } //if found
    else { 
      file_map_iter = file_map_.find(k);
      if(file_map_iter != file_map_.end()){
        my_file_.seekg(file_map_iter->second.second);
        data_type* temp_object=new data_type();
        temp_object->read_file(my_file_,file_map_iter->second.first); 
        insert(k,temp_object);
        //delete temp_object;
        return get_vat_point(k);          
      } //if in file
      else{
	cout <<"Inside file_cache_manager::get_vat_point. Vat is neither in mem nor in file!!"<<endl;
      }//else not even in file
    } //else

    return 0;
  }

  /** Inserts a new entry to the cache. 
   *  Entry is associated with a given key.
   *  k is a key associated with inserted entry.
   *  e is an entry inserted to the cache.
   *  return true on success and false otherwise.
   */

  bool insert(const key_type& k, data_type* e) {
    if (capacity_ < size_ + e->byte_size()) {
      if (update(e->byte_size(),0) == false) {
	cout<<"insert: update problem\n" << ".bytesize: " << e->byte_size()<<endl;
        return false;
      }
    }
    istorage_.insert(entry_type(k,std::pair<bool,data_type*>(0,e)));
    size_ += e->byte_size();
    return true;
  } // insert

  /* update the the memory by the requested size.. 
   * evict some entries untill the memory has free memory >= rq_size
   */
  bool update(size_type req_size = 0,int levelone=1) {
    if (istorage_.empty()) {    
      cout<<"Nothing in the memory..Update Failed"<<endl;
      return false;
    }

    long int S = 0;
    if (req_size > 0) S = capacity_ - size_ - req_size;

    storage_iterator iter;
    do {
    iter = istorage_.begin();    
    //find an entry which we can evict..
    while (iter->second.first ==1 && iter != istorage_.end())  {
      iter++;
    }

    if (iter == istorage_.end()){
      cout <<"Nothing can be evicted from memory"<<endl;
      return false;
    }
    
    S += iter->second.second->byte_size();
    size_ =  size_ - iter->second.second->byte_size();
    //check if it is already in the file, if not write it to the file
    
    file_map_iter = file_map_.find( iter->first); 
    //not in the file .. or it is level one vat .
    if( file_map_iter == file_map_.end() || levelone ){
      my_file_.seekp(offset);
      std::pair<size_type, size_type> file_offset_info(iter->second.second->byte_size(),offset);
      file_map_[iter->first]=file_offset_info; 
      offset += iter->second.second->byte_size();
      iter->second.second->write_file(my_file_);
    }
     
    if(iter->second.second)
       delete iter->second.second; //reclaim memory for vat
     else 
       std::cout<<"cache_manager.update: vat is null"<<endl;
      
    istorage_.erase(iter);
    // keep key list as a heap      
    if ((istorage_.empty()) && (S < 0)) {
      cout<<"Nothing in the memory..Update Failed"<<endl;
      return false;
    }
  }
  while (S <= 0);
  return true;
    
} // update

private:

// not defined (cache_manager is not assignable)
cache_manager(const cache_manager&);
cache_manager& operator=(const cache_manager&);
typedef std::pair<storage_iterator, bool> result_type;
typedef std::pair<size_type, size_type> pair_type;           
typedef std::pair<key_type, std::pair<bool,data_type*> > entry_type;

File_Map_iter file_map_iter;  

size_type size_; // cache size
size_type capacity_; // cache capacity
storage_type istorage_; // internal storage   
FileMap  file_map_; //file map to store where each record is stored in the file if it is in the file.
size_type offset;  //offset in the file    
std::fstream my_file_; //file to be used for writing back..
 string file_name; //name of the cache file

}; // class cache_manager

#endif // FILE_CACHE_MANAGER_H
