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
#ifndef _GRAPH_CAN_CODE_H
#define _GRAPH_CAN_CODE_H

using namespace std;

#include <string>
#include <sstream>
#include <vector>
#include <ext/hash_map>
#include "generic_classes.h"


time_tracker tt_iostream;

template<typename V_T, typename E_T>
struct five_tuple;

template<typename V_T, typename E_T>
ostream& operator<< (ostream&, const five_tuple<V_T, E_T>&);

// NOTE: should the labels (_li, _lij, _lj) in thus struct be stored as 
// references (to avoid copying them) ?? Will that work ??

/**
 * \brief Storing a five_tuple that represent a labeled edge of a graph.
 *
 * <dfs_id1, dfs_id2, vertex_label1, edge_label, vertex_label2> are the 5-tuple.
 * It is used as part of the canonical code of a graph.
 */
template<typename V_T, typename E_T>
struct five_tuple
{
  five_tuple() {}

  five_tuple(const int& id1, const int& id2, const V_T& li, const E_T& lij, const V_T& lj): _i(id1), _j(id2), _li(li), _lj(lj), _lij(lij) {}

  bool operator< (const five_tuple<V_T, E_T>& rhs) const {
    // follows ordering given on pg 10 of gSpan TR
    bool is_fwd=(_i<_j);
    bool rhs_is_fwd=(rhs._i<rhs._j);

    if(!is_fwd && rhs_is_fwd)
      return true;

    if(!is_fwd && !rhs_is_fwd && _j<rhs._j)
      return true;

    if(!is_fwd && !rhs_is_fwd && _j==rhs._j && _lij<rhs._lij)
      return true;

    if(is_fwd && rhs_is_fwd && _i>rhs._i)
      return true;

    if(is_fwd && rhs_is_fwd && _i==rhs._i && _li<rhs._li)
      return true;

    if(is_fwd && rhs_is_fwd && _i==rhs._i && _li==rhs._li && _lij<rhs._lij)
      return true;

    if(is_fwd && rhs_is_fwd && _i==rhs._i && _li==rhs._li && _lij==rhs._lij && _lj<rhs._lj)
      return true;

    return false;
  }//end operator<

  friend ostream& operator<< <>(ostream&, const five_tuple<V_T, E_T>&);

  int _i;
  int _j;
  V_T _li;
  V_T _lj;
  E_T _lij;

};//end struct five_tuple


template<typename V_T, typename E_T>
ostream& operator<< (ostream& ostr, const five_tuple<V_T, E_T>& tuple) {
  ostr<<tuple._i<<" "<<tuple._j<<" "<<tuple._li<<" "<<tuple._lij<<" "<<tuple._lj;
  return ostr;
}

template<typename PP, typename V_T, typename E_T, template <typename> class ALLOC >
class canonical_code<GRAPH_PROP, V_T, E_T, ALLOC >;

template<typename PP, typename V_T, typename E_T, template <typename> class ALLOC >
ostream& operator<< (ostream&, const canonical_code<GRAPH_PROP, V_T, E_T, ALLOC >&);


/**
 * \brief Graph canonical Code class by partial specialization of
 * generic canonical_code class.
 *
 * pattern_prop is set to undirected (graph property)
 */
template<typename PP, typename V_T, typename E_T, template <typename> class ALLOC=std::allocator >
class canonical_code<GRAPH_PROP, V_T, E_T, ALLOC >
{
 public:

  typedef int STORAGE_TYPE;
  typedef five_tuple<V_T, E_T> FIVE_TUPLE;
  typedef FIVE_TUPLE INIT_TYPE;
  typedef eqint COMPARISON_FUNC;

  typedef vector<FIVE_TUPLE, ALLOC<FIVE_TUPLE> > TUPLES;
  typedef typename TUPLES::const_iterator CONST_IT;
  typedef typename TUPLES::iterator IT;
  typedef canonical_code<GRAPH_PROP, V_T, E_T, ALLOC > CAN_CODE;
  typedef HASHNS::hash_map<int, int, HASHNS::hash<int>, std::equal_to<int>, ALLOC<int> > VID_HMAP;
  typedef typename VID_HMAP::const_iterator VM_CONST_IT;
  typedef vector<int, ALLOC<int> > RMP_T;

  canonical_code() : _can_code(id_generator++) {} // defunct default constructor

  /** Parameterized constructor that inserts ft as first tuple into
      DFS code */
  canonical_code(const FIVE_TUPLE& ft, const int&gi, const int& gj) {
    append(ft, gi, gj);
  }

  IT begin() { return _dfs_code.begin();}
  CONST_IT begin() const { return _dfs_code.begin();}
  IT end() { return _dfs_code.end();}
  CONST_IT end() const { return _dfs_code.end();}

  int size() const { return _dfs_code.size();}

  void clear() {
    _dfs_code.clear();
    _cid_to_gid.clear();
    _gid_to_cid.clear();
    _rmp.clear();
  }

  const FIVE_TUPLE& operator[](const int& index) const { return _dfs_code[index];}

  void init_rmp() {
    if(!_rmp.empty())
      _rmp.clear();
    _rmp.push_back(0);
    _rmp.push_back(1);
  }

  void update_rmp(const FIVE_TUPLE& tuple) {
    if(_rmp.empty()) {
      _rmp.push_back(tuple._i);
      _rmp.push_back(tuple._j);
      return;
    }

    // no changes to rmp if it's a back-edge
    if(tuple._i>tuple._j)
      return;

    typename RMP_T::iterator rmp_it=_rmp.end()-1;
    while(rmp_it>=_rmp.begin()) {
      if(*rmp_it==tuple._i)
        break;
      rmp_it=_rmp.erase(rmp_it);
      rmp_it--;
    }
    _rmp.push_back(tuple._j);

  }//end update_rmp()


  template<class PAT>
  void init(const INIT_TYPE& tuple, PAT* pattern) { 
    tt_iostream.start();
    clear();
    _dfs_code.push_back(tuple);

    ostringstream t_ss;
    t_ss << tuple;
    string t_str = t_ss.str();
    char* p = new char[t_str.length()+1];
    t_str.copy(p, string::npos);
    p[t_str.length()] = 0;
    HASHNS::hash_map<const char*, int, HASHNS::hash<const char*>, eqstr>::iterator itr = level_one_hash.find(p); 
    if(itr != level_one_hash.end()) {
      _can_code = itr->second; 
      delete p;
    } else {
      level_one_hash.insert(make_pair(p, _can_code));
    }

    tt_iostream.stop();
      
    pattern->update_rmpath(0);
    pattern->update_rmpath(1);
  }

  void push_back(const FIVE_TUPLE& tuple) {
    _dfs_code.push_back(tuple);
    tt_iostream.start();
    tt_iostream.stop();
  }

  void append(const FIVE_TUPLE& tuple) { 
    push_back(tuple);
  }

  void append(const FIVE_TUPLE& tuple, const int& gi, const int& gj) { 
    push_back(tuple);
    _cid_to_gid.insert(make_pair(tuple._i, gi));
    _cid_to_gid.insert(make_pair(tuple._j, gj));
    _gid_to_cid.insert(make_pair(gi, tuple._i));
    _gid_to_cid.insert(make_pair(gj, tuple._j));
  }

  void update_code() {
    _can_code = id_generator++;
  }


  STORAGE_TYPE getCode() const {
    return _can_code;
  }

  bool operator< (const CAN_CODE& rhs) const {
    int i=0, j=0;
    while(i<_dfs_code.size() && j<rhs._dfs_code.size()) {
      if(rhs._dfs_code[j]<_dfs_code[i])
        return false;
      i++;
      j++;
    }
      
    // both codes are equal till common length
    return _dfs_code.size()>=rhs._dfs_code.size();
  }

  int cid(const int& gi) const {
    VM_CONST_IT it=_gid_to_cid.find(gi);
    if(it==_gid_to_cid.end()) {
      return -1;
    }
    return it->second;
  }

  int gid(const int& ci) const {
    VM_CONST_IT it=_cid_to_gid.find(ci);
    if(it==_cid_to_gid.end()) {
      return -1;
    }
    return it->second;
  }

  RMP_T& rmost_path() { return _rmp;}

  void append_rmp(const int& id) {
    _rmp.push_back(id);
  }

  friend ostream& operator<< <>(ostream&, const canonical_code<GRAPH_PROP, V_T, E_T, ALLOC>&);

 private:
  STORAGE_TYPE _can_code;
  TUPLES _dfs_code;  
  VID_HMAP _cid_to_gid;
  VID_HMAP _gid_to_cid;
  RMP_T _rmp;
  static int id_generator;
  static HASHNS::hash_map<const char*, int, HASHNS::hash<const char*>, eqstr> level_one_hash;

};//end class canonical_code for graph

template<typename PP, typename V_T, typename E_T, template <typename> class ALLOC>
ostream& operator<< (ostream& ostr, const canonical_code<GRAPH_PROP, V_T, E_T, ALLOC>& cc) {
  typename canonical_code<GRAPH_PROP, V_T, E_T, ALLOC>::TUPLES::const_iterator it;
  for(it=cc._dfs_code.begin(); it!=cc._dfs_code.end(); it++)
    ostr<<*it<<endl;
  return ostr;
}

template<class PP, typename v, typename e, template <typename> class ALLOC >
int canonical_code<GRAPH_PROP, v, e, ALLOC>::id_generator = 1;

template<class PP, typename v, typename e, template <typename> class ALLOC >
HASHNS::hash_map<const char*, int, HASHNS::hash<const char*>, eqstr> 
canonical_code<GRAPH_PROP, v, e, ALLOC>::level_one_hash;

#endif
