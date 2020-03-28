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
#ifndef _SEQ_CAN_CODE_H_
#define _SEQ_CAN_CODE_H_

#include<sstream>
#include<string>
#include<limits>
#include "generic_classes.h"

#define INT_MIN -2147483648

using namespace std;


/**
 * \brief Sequence canonical Code class by partial specialization of
 * generic canonical_code class.
 *
 * pattern_prop is set to directed, acyclic, indegree_lte_one, outdegree_lte_one(sequence property)
 */
// template<class PP, typename V_T, typename E_T, template<typename> class ALLOC=std::allocator >
template<class PP, typename V_T, typename E_T, template<typename> class ALLOC >
class canonical_code<SEQ_PROP, V_T, E_T, ALLOC >
{
 public:
  
  typedef int STORAGE_TYPE;
  typedef V_T INIT_TYPE;
  typedef eqint COMPARISON_FUNC;
  
  canonical_code() {}
  
  /**
   * Used to initialize the canonical code. Called from 
   * db_reader so that the pattern's canonical code can 
   * be initialized. 
   */
  template<typename PAT>
    void init(V_T v, PAT*) {
    typename map<V_T, int>::iterator i;
    i = len1_pat_map.find(v);
    
    if(i != len1_pat_map.end()){
      _can_code = i->second;
	}
    else {
      len1_pat_map[v] = init_counter;
      _can_code = init_counter++;
    }
  }
  
  /**
   * Update the canonical code when a node is added.
   */
  void insert_vertex(const V_T& v) {
    _can_code = id_generator--; 
    if(_can_code == INT_MIN) {
      cout << "Range of int values exhausted while assigning canonical code." << endl;
      cout << "Please report this error to the developers." << endl;
    }
  }
  
  
  void update_code() {}
  
  /**
   * Return the canonical code.
   */
  STORAGE_TYPE getCode() const {
    return _can_code;
  }
  
 private:

  STORAGE_TYPE _can_code;
  static int id_generator;
  static map<V_T, int> len1_pat_map;
  static int init_counter;
};

template<class PP, typename v, typename e, template <typename> class ALLOC >
         int canonical_code<SEQ_PROP, v, e, ALLOC>::id_generator = -1;

template<class PP, typename v, typename e, template <typename> class ALLOC >
         int canonical_code<SEQ_PROP, v, e, ALLOC >::init_counter = 1;

template<class PP, typename v, typename e, template <typename> class ALLOC >
         map<v, int> canonical_code<SEQ_PROP, v, e, ALLOC >::len1_pat_map;

#endif
