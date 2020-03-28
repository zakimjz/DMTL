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
// canonical code specialization for trees

#ifndef _TREE_CAN_CODE_
#define _TREE_CAN_CODE_

using namespace std;

#include <string>
#include <sstream>
#include "generic_classes.h"
#include "typedefs.h"

/**
 * \brief Tree canonical Code class by partial specialization of
 * generic canonical_code class.
 *
 * pattern_prop is set to directed, acycle, indegree_lte_one (tree property)
 */
template<typename PP, typename V_T, typename E_T, template <typename> class ALLOC>
class canonical_code<TREE_PROP, V_T, E_T, ALLOC>
{

 public:

  typedef int STORAGE_TYPE;
  typedef V_T INIT_TYPE;
  typedef eqint COMPARISON_FUNC;

  canonical_code() {}

  void backtrack() {
  }

  template<class PAT>
  void init(const INIT_TYPE& v, PAT* pattern) { 
    typename map<V_T, int>::iterator i;
    i = len1_pat_map.find(v);

    if(i != len1_pat_map.end())
      _can_code = i->second;
    else {
      len1_pat_map[v] = init_counter;
      _can_code = init_counter++;
    }

    pattern->update_rmpath(0);
  }

  void insert_vertex(const INIT_TYPE& v) {
  }

  void update_code() { 
    _can_code = id_generator--;
  }


  STORAGE_TYPE getCode() const {
    return _can_code;
  }

 private:
  STORAGE_TYPE _can_code;
  static int id_generator;
  static map<V_T, int> len1_pat_map;
  static int init_counter;

};//end class canonical_code for tree

template<class PP, typename V_T, typename E_T, template <typename> class ALLOC>
int canonical_code<TREE_PROP, V_T, E_T, ALLOC >::id_generator = -1;

template<class PP, typename V_T, typename E_T, template <typename> class ALLOC >
int canonical_code<TREE_PROP, V_T, E_T, ALLOC >::init_counter = 1;

template<class PP, typename V_T, typename E_T, template <typename> class ALLOC >
map<V_T, int> canonical_code<TREE_PROP, V_T, E_T, ALLOC >::len1_pat_map;


#endif
