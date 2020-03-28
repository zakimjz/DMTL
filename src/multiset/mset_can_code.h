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
#ifndef _MSET_CAN_CODE_
#define _MSET_CAN_CODE_

#include<sstream>
#include<string>
#include<map>

#include "helper_funs.h"
#include "typedefs.h"
#include "generic_classes.h"

using namespace std;


/**
 * \brief Itemset canonical Code class by partial specialization of
 * generic canonical_code class.
 *
 * pattern_prop is set to no_edges(itemset property)
 */
template<class PP, typename vertex_t, typename edge_t, 
         template <typename> class ALLOC >
class canonical_code<MSET_PROP, vertex_t, edge_t, ALLOC > 
{
 public:
  
  typedef int STORAGE_TYPE;
  typedef vertex_t INIT_TYPE;
  typedef eqint COMPARISON_FUNC;
  
  canonical_code() {}
  
  /**
   * Used to initialize the canonical code. Called from 
   * db_reader so that the pattern's canonical code can 
   * be initialized. 
   * \param[in] v It is the vertex for which the code is generated
   * \param[in] pat The associated pattern
   * \return void
   */
  template<class PAT>
    void init(INIT_TYPE v, PAT* pat) {
    
    typename map<vertex_t, int>::iterator i;
    i = len1_pat_map.find(v);
    
    if(i != len1_pat_map.end())
      _c_code = i->second;
    else {
      len1_pat_map[v] = init_counter;
      _c_code = init_counter++;
    }
  }

 /** Generate canonical code for each length-1 pattern from static-id  
  * \param[in] v Input vertex of vertex_t datatype.
    \return void
  */ 
  void insert_vertex(const vertex_t& v) {
    _c_code = id_generator--; 
  }
  
  /**
   * Return the canonical code.
   */
  STORAGE_TYPE getCode() const {
    return _c_code;
  }
  
  void update_code() {}
  
  
 private:
  STORAGE_TYPE _c_code;
  static int id_generator;
  static map<vertex_t, int> len1_pat_map;
  static int init_counter;
};

// Setting it to -1 since for level-1 the _c_code should be 
// same as the vertex id. Beyond level-1 the id gets assigned 
// from the id_generator.
template<class PP, typename v, typename e, template <typename> class ALLOC >
int canonical_code<MSET_PROP, v, e, ALLOC>::id_generator = -1;

template<class PP, typename v, typename e, template <typename> class ALLOC >
int canonical_code<MSET_PROP, v, e, ALLOC>::init_counter = 1;

template<class PP, typename v, typename e, template <typename> class ALLOC >
map<v, int> canonical_code<MSET_PROP, v, e, ALLOC>::len1_pat_map;

#endif
