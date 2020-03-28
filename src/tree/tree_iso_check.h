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
// isomorphism cheking for trees

#ifndef _TREE_ISO_CHECK_H_
#define _TREE_ISO_CHECK_H_

#include <stack>
#include <utility>

#include "typedefs.h"

template<class PP, class MP, class ST, template<class, typename, typename, template <typename> class > class CC,
         template<typename> class ALLOC>
class pattern;

class directed;
class acyclic;
class indegree_lte_one;
class ordered;

template<class prop, class next_property>
class proplist;

using namespace std;

/** Updates the right most path of candidate for last vertex added */
template<class PP, class MP, class ST, template<typename, typename, typename, template <typename> class > class CC,
         template <typename> class ALLOC >
void update_rmost_path(pattern<TREE_PROP, MP, ST, CC, ALLOC>*const& cand_pat) {

  typedef pattern<TREE_PROP, MP, ST, CC, ALLOC> PATTERN;

  int new_vid=cand_pat->rmost_vid(); // vertex added to generate this candidate
  typename PATTERN::RMP_T& rmost_path=cand_pat->_rmost_path;
  int pvid=(cand_pat->in_edges(new_vid)).first->first; // parent of new_vid
  
  // pop off vids tills you hit pvid, or root of tree
  typename PATTERN::RMP_T::iterator it=rmost_path.end()-1;
  if(rmost_path.end()!=rmost_path.begin())
    while(it!=rmost_path.begin()) {
      if(*it==pvid)
        break;
      it=rmost_path.erase(it);
      it--;
        
      // update canonical code
      cand_pat->_canonical_code.backtrack();
    }

    rmost_path.push_back(new_vid);
    // update canonical code
    cand_pat->_canonical_code.insert_vertex(cand_pat->label(new_vid));

}//end update_rmost_path()


/** Returns true if first iterator is within one unit of second one*/
template<typename IT>
int eit_diff_one(IT it1, IT it2) {
  if(it1==it2)
    return true;
  it1++;
  if(it1==it2)
    return true;
  
  return false;
}

/** Returns an IT that is two units before second one */
template<typename IT>
IT last_eits(IT it1, IT it2) {
  IT prev=it1;
  it1++;
  it1++;
  
  while(it1++!=it2)
    prev++;
  
  return prev;
}

/** Returns if given candidate is isomorphic; 
    correspondingly sets the bool flag in cand_pat as well */
template<class PP, class MP, class ST, template<typename, typename, typename, template <typename> class > class CC,
         template <typename> class ALLOC>
bool check_isomorphism(pattern<TREE_PROP, MP, ST, CC, ALLOC>*const& cand_pat) {
  
  typedef pattern<TREE_PROP, MP, ST, CC, ALLOC> PATTERN;
  
  typename PATTERN::RMP_T::const_iterator it;
  typename PATTERN::CONST_EIT_PAIR eit_pair;
  
  update_rmost_path(cand_pat);
  it=cand_pat->_rmost_path.end();
  if(cand_pat->_rmost_path.size())
    it--;
  
  while(it>=cand_pat->_rmost_path.begin()) {
    // get last 2 edges of *it
    eit_pair=cand_pat->out_edges(*it);
    bool max_one_child=false;
    
    
    if(eit_pair.first!=eit_pair.second)
      eit_pair.second--;
    else
      max_one_child=true;
    
    if(eit_pair.first==eit_pair.second)
      max_one_child=true;
    else
      eit_pair.second--;
    
    if(!max_one_child && !check_subtree_order(eit_pair.second, cand_pat)) {
      // last 2 subtrees of this vid were not ordered canonically
      cand_pat->_is_canonical=false;
      return false;
    }
    it--;
  }
  
  // execution reaches this point only if isomorphism check was passed
  cand_pat->_is_canonical=true;
  return true;
  
}//end check_isomorphsim()

/** Receives an iterator to first of two subtrees to be checked, and the graph;
    Returns true if first-subtree <= second-subtree */
template<typename EIT, typename PATTERN>
bool check_subtree_order(EIT& edge_it, const PATTERN*const& cand_pat) {
  /* This function in effect imposes the <= ordering defined on pg 1007 of 
     SLEUTH */
  
  // x and y are names for the two respective nodes being compared
  int vid_x=edge_it->first;
  edge_it++;
  int vid_y=edge_it->first;

  typename PATTERN::CONST_EIT_PAIR edges;
  typedef pair<int, int> st_pair;

  stack<st_pair> stack_x;
  stack<st_pair> stack_y;

  stack_x.push(make_pair(vid_x, 0));
  stack_y.push(make_pair(vid_y, 0));

  while(!stack_x.empty() && !stack_y.empty()) {
    
    const st_pair& node_x=stack_x.top();
    stack_x.pop();
    const st_pair& node_y=stack_y.top();
    stack_y.pop();
    
    /// check if two nodes are at same dfs level ///
    if(node_x.second<node_y.second) {
      // y is deeper
      return false;
    }
    
    if(node_x.second>node_y.second) {
      // x is deeper
      return true;
    }
    
    /// now check labels /// 
    if(cand_pat->label(node_x.first) < cand_pat->label(node_y.first)) {
      return true;
    }

    if(cand_pat->label(node_x.first) > cand_pat->label(node_y.first)) {
      return false;
    }
    
      // push edges of x in reverse order
    edges=cand_pat->out_edges(node_x.first);
    if(edges.second!=edges.first) {
      edges.second--;
      while(edges.second!=edges.first) {
        stack_x.push(make_pair(edges.second->first, node_x.second+1));
        edges.second--;
      }
      stack_x.push(make_pair(edges.second->first, node_x.second+1));
    }
    
    // push edges of y in reverse order
    edges=cand_pat->out_edges(node_y.first);
    if(edges.second!=edges.first) {
      edges.second--;
      while(edges.second!=edges.first) {
        stack_y.push(make_pair(edges.second->first, node_y.second+1));
        edges.second--;
      }
      stack_y.push(make_pair(edges.second->first, node_y.second+1));
    }
    
  }//end while stacks not empty

  if(!stack_x.empty()) {
    // x is deeper
    return true;
  }

  if(!stack_y.empty()) {
    // y is deeper      
    return false;
  }
  
  // both stacks were empty i.e. equal subtrees
  return true;
  
}//end check_subtree_order()


/**
  specialized isomorphism for ordered trees
 */
template<class PP, class MP, class ST, template<typename, typename, typename, template <typename> class > class CC,
         template <typename> class ALLOC >
bool check_isomorphism(pattern<ORD_TREE_PROP, MP, ST, CC, ALLOC>*const& cand_pat) {
  update_rmost_path(cand_pat);
  cand_pat->_is_canonical=true;
  return true;
}

#endif
