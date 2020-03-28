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
#ifndef _TREE_OPERATORS_H
#define _TREE_OPERATORS_H

#include "typedefs.h"

class Fk_Fk;

/** \brief Returns the pos at which rmost_vid is attached to its parent
 *
 * Assumptions made: rmost_vid has only one in-edge; 
 * Assumptions: vid of a vertex is same as its pos i.e. dfs id
 */
template<class PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class > class CC,
         template <typename> class ALLOC >
int compute_pos(const TREE_PATTERN* pat) {
  typename TREE_PATTERN::CONST_EIT_PAIR ep=pat->in_edges(pat->rmost_vid());
  if(ep.first==ep.second) {
    // single node tree
    return -1;
  }

  return ep.first->first;
}//end compute_pos()

/**
 * \brief An operator class in Tree Mining, compares two patterns.
 */
template<typename PATTERN>
class compare_pos
{
 public:
  bool operator()(const PATTERN* p1, const PATTERN* p2) const {
    return compute_pos(p1)>compute_pos(p2);
  }
};

template<class PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class > class CC,
         template <typename> class ALLOC >
ostream& operator<< (ostream& ostr, const TREE_PATTERN* p) {
  
  stack<typename TREE_PATTERN::CONST_EIT_PAIR> st;
  
  typename TREE_PATTERN::CONST_IT it;
  for(it=p->begin(); it!=p->end(); it++) {
    if(it == p->begin()) {
      ostr << (*it).v;
      st.push(p->out_edges((*it).id));
      
    } else {
      bool btrack = false;
      while(st.top().first == st.top().second) {
        st.pop();
        ostr << "  <--  ";  // backtracking 
        btrack = true;
        st.top().first++;
      }
      
      if(!btrack) {
        ostr << "  ---  " << (*it).v;  // forward edge
      } else {
        ostr << (*it).v;
      }
      
      st.push(p->out_edges((*it).id));
    }
  }
  ostr <<" -- " <<p->_pat_sup;
  
  return ostr;
}

#endif
