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
/** author: parimi@cs.rpi.edu
    The functions in this file are based on the canonical code class. 
    A new canonical code class would require a new set of isomorphism 
    functions */

#ifndef _GRAPH_ISO_CHECK_H
#define _GRAPH_ISO_CHECK_H

#include<vector>
#include<iostream>
#include "typedefs.h"
#include "time_tracker.h"

using namespace std;
#include "element_parser.h"

#define INF_LABEL "99999999" // max label initializer for edge & vertex labels

template<class PP, class MP, class ST, template<class, typename, typename, template <typename> class > class CC,
         template <typename> class ALLOC >
class pattern;

class undirected;

template<class prop, class next_property>
class proplist;


int  num_startup=0;  
int  num_urmp=0;  
time_tracker tt_iso_chk;
time_tracker tt_iso_startup;
time_tracker tt_update_rmp;
time_tracker tt_minimal;
time_tracker  tt_rest;
void print_rmp(const vector<int>& rmp) {
  cout<<"rmp is:"<<endl;
  for(unsigned int i=0; i<rmp.size(); i++)
    cout<<rmp[i]<<" ";
  cout<<endl;
}

/** Updates the right most path of candidate for last vertex added;
    Also updates its canonical code for new added edge */
// Note: it is assumed that this function is called for graphs of atleast 
// size=2
template<class PP, class MP, class PAT_ST, template<class, typename, typename, template <typename> class > class CC, 
         template <typename> class ALLOC >
void update_rmost_path(GRAPH_PATTERN*const& cand_pat) {
  // How it works: no change in rmost-path (rmp) if last edge added was back-
  // edge - only append to canonical code. 
  // In case of fwd edge, rmp is modified as well

  num_urmp++;  
  tt_update_rmp.start();

  int new_vid=cand_pat->rmost_vid(); // vertex added to generate this candidate
  typename GRAPH_PATTERN::RMP_T& rmost_path=cand_pat->_rmost_path;

  // no change to rmp if this candidate was formed by back extension
  // but canonical code must be updated
  if(new_vid==*(rmost_path.end()-1)) {
    typename GRAPH_PATTERN::CONST_EIT_PAIR eit_p=cand_pat->out_edges(new_vid);
    eit_p.second--;
    int back_vid=eit_p.second->first; // vid at back vertex of edge
    typename GRAPH_PATTERN::CAN_CODE::FIVE_TUPLE new_tuple(new_vid, back_vid, cand_pat->label(new_vid), eit_p.second->second, cand_pat->label(back_vid));
    cand_pat->_canonical_code.push_back(new_tuple);
    return;
  }

  /// new edge is a fwd edge ///
  int pvid=(cand_pat->out_edges(new_vid)).first->first; // parent of new_vid
  typename GRAPH_PATTERN::RMP_T::iterator it=rmost_path.end()-1;

  while(it!=rmost_path.begin()) {
    if(*it==pvid)
    break;
    it=rmost_path.erase(it);
    it--;
  }

  rmost_path.push_back(new_vid);
  // update canonical code
  typename GRAPH_PATTERN::CONST_EIT_PAIR eit_p=cand_pat->out_edges(new_vid);
  typename GRAPH_PATTERN::CAN_CODE::FIVE_TUPLE new_tuple(pvid, new_vid, cand_pat->label(pvid), eit_p.first->second, cand_pat->label(new_vid));
  cand_pat->_canonical_code.push_back(new_tuple);
  tt_update_rmp.stop();

}//end update_rmost_path()

/** Returns true if given candidate is isomorphic; 
    correspondingly sets the bool flag in cand_pat as well */
template<class PP, class MP, class PAT_ST, template<class, typename, typename, template <typename> class> class CC,
         template <typename> class ALLOC >
bool check_isomorphism(GRAPH_PATTERN*const& cand_pat) {
  // How it works: it is a recursive test. At each recursive call, the 
  // minimal edge according to DFS ordering (gSpan) is determined. If this 
  // minimal edge is what cand_pat has at that level, then cand_pat passes 
  // test at that call and further recursive calls have to be made. This 
  // recursion continues till i) test failed at some level or ii) all edges 
  // are exhausted, when cand_pat is declared to have passes isomorphsim test

  tt_iso_chk.start();

  typedef CC<GRAPH_PROP, typename GRAPH_PATTERN::VERTEX_T, typename GRAPH_PATTERN::EDGE_T, ALLOC > CAN_CODE;

  // first step is get rmp updated
  update_rmost_path(cand_pat);

#ifdef PRINT
  cout<<"checking isomorphism for: "<<endl<<cand_pat->canonical_code()<<endl;
#endif

  typename GRAPH_PATTERN::EDGE_T e; // least label of edge
  typename GRAPH_PATTERN::VERTEX_T src_v, dest_v; // least labelled vertices of an edge
  vector<pair<int, int> > ids; // ids with least labelled edges
  vector<CAN_CODE> new_codes;

  tt_rest.start();
  // default startup values
  src_v=cand_pat->label(0);
  dest_v=cand_pat->label(1);
  if(!cand_pat->get_out_edge(0, 1, e)) {
    cerr<<"check_iso(graph): get_out_edge failed in startup"<<endl;
    tt_iso_chk.stop();
    return false;
  }
  tt_rest.stop();

  iso_startup(cand_pat, src_v, dest_v, e, ids);

  // create separate codes for each pair in ids
  // each such pair shall have to be tested for isomorphism
  
  tt_rest.start();
  for(unsigned int i=0; i<ids.size(); i++) {
    CAN_CODE new_cc(typename CAN_CODE::FIVE_TUPLE(0, 1, cand_pat->label(ids[i].first), e, cand_pat->label(ids[i].second)), ids[i].first, ids[i].second);
    new_cc.init_rmp();
    new_codes.push_back(new_cc);
  }

  // codes in new_codes are all equivalent
  // check if any one is < current one


  if(new_codes[0][0]<cand_pat->_canonical_code[0]) {
#ifdef PRINT
      cout<<"Isomorphism decision made at first level: new_tuple="<<new_codes[0][0]<<endl;
      cout<<"current_code="<<cand_pat->_canonical_code[0]<<endl;
#endif
      cand_pat->_is_canonical=false;
      tt_iso_chk.stop();
      return false;
    }
  tt_rest.stop();

  for(unsigned int i=0; i<new_codes.size(); i++) {
    tt_minimal.start();
    bool r = minimal(cand_pat, new_codes[i]);
    if(!r) {
      cand_pat->_is_canonical=false;
      tt_iso_chk.stop();
      return false;
    }
    tt_minimal.stop();
  }

  // execution reaches here iff all codes were found to be >= current one
  cand_pat->_is_canonical=true;
  tt_iso_chk.stop();
  return true;

}//end check_isomorphism()


// find minimal pair, acc to DFS ordering
template<typename PATTERN>
void iso_startup(const PATTERN* cand_pat, typename PATTERN::VERTEX_T& src_v, typename PATTERN::VERTEX_T& dest_v, typename PATTERN::EDGE_T& e, vector<pair<int, int> >& ids) {  
  num_startup++;  
  tt_iso_startup.start();
  // find minimal pair, acc to DFS ordering
  typename PATTERN::CONST_IT it;
  typename PATTERN::CONST_EIT_PAIR eit_p;
  for(it=cand_pat->begin(); it!=cand_pat->end(); it++) {
    if(it->v>src_v)
    continue;

    eit_p=cand_pat->out_edges(it->id);
      
    if(it->v<src_v) {
      // new minimal src label found  
      // find edge with least label      
      ids.clear();
      e=eit_p.first->second;
      src_v=it->v;
      dest_v=cand_pat->label(eit_p.first->first);
      ids.push_back(make_pair(it->id, eit_p.first->first));
      eit_p.first++;
    }
      
    while(eit_p.first!=eit_p.second) {
      if(eit_p.first->second<=e) {
        if(eit_p.first->second<e) {
          // new minimal edge found
          ids.clear();
          e=eit_p.first->second;
          dest_v=cand_pat->label(eit_p.first->first);
          ids.push_back(make_pair(it->id, eit_p.first->first));
          eit_p.first++;
          continue;
        }

        if(cand_pat->label(eit_p.first->first)<=dest_v) {
          if(cand_pat->label(eit_p.first->first)<dest_v) {
            // new least dest label found
            ids.clear();
            dest_v=cand_pat->label(eit_p.first->first);
          }
          ids.push_back(make_pair(it->id, eit_p.first->first));
        }
      }//end if eit_p.first->second<=e
      eit_p.first++;
    }  
  }//end for

  tt_iso_startup.stop();

}//end iso_startup()


/** Returns true if cand_pat's can_code is <= the one generated from new_code;
    calls itself recursively to accomplish this task */
template<typename PATTERN, typename CAN_CODE>
bool minimal(const PATTERN* cand_pat, CAN_CODE& new_code) {

  element_parser<typename PATTERN::EDGE_T> ed_prsr;
  element_parser<typename PATTERN::VERTEX_T> vt_prsr;

  // starts by checking if a back-edge can be added
  // else, the minimal fwd edge is added and minimality checked again

  //// quick check on the status of minimality test ////
  // if all edges have been added to new_code, then no new edges can be added
  // hence cand_pat is minimal
  if(cand_pat->canonical_code().size()==new_code.size())
    return true;

  const CAN_CODE& current_code=cand_pat->canonical_code();
  typename CAN_CODE::CONST_IT cc_it=new_code.end()-1;
  bool is_last_fwd=(cc_it->_i<cc_it->_j); // denotes if last edge in new_code was a fwd edge
  int last_vid=(is_last_fwd)? cc_it->_j: cc_it->_i; // vid to which edge shall be added  
  int last_vid_g=new_code.gid(last_vid);
  typename PATTERN::CONST_EIT_PAIR eit_p=cand_pat->out_edges(last_vid_g);
  int code_index=new_code.size();
  typename CAN_CODE::RMP_T& rmp=new_code.rmost_path();
  typename CAN_CODE::RMP_T::iterator rmp_it;

  ///// first try to add BACK EDGE /////////
  int back_vid, last_back_vid;
  back_vid=last_vid;
  typename PATTERN::EDGE_T e=typename PATTERN::EDGE_T();

  if(is_last_fwd)
    // relatively straight forward - add the farthest back edge you find
    last_back_vid=-1;
  else
    // add a back edge only if it's after the last back edge present
    last_back_vid=cc_it->_j;
  
  while(eit_p.first!=eit_p.second) {
    rmp_it=rmp.end()-3;

      // check if this vid occurs on the rmp of can code
    while(rmp_it>=rmp.begin()) {
      if(*rmp_it==new_code.cid(eit_p.first->first))
        break;

      if(*rmp_it<new_code.cid(eit_p.first->first))
        rmp_it=rmp.begin();
  
      rmp_it--;
    }

    if(rmp_it<rmp.begin()) {
      eit_p.first++;
      continue;
    }

    // atleast 2 nodes prior to last one
    if(new_code.cid(eit_p.first->first)<back_vid // this vertex is farthest one seen so far
       && new_code.cid(eit_p.first->first)>last_back_vid) { // a valid back edge must end  
                                                              // at a vertex after last_back_vid
       back_vid=new_code.cid(eit_p.first->first);
       e=eit_p.first->second;
    }
    eit_p.first++;
  }
      
  if(back_vid!=last_vid) {
    // valid back edge found
    typename PATTERN::VERTEX_T lbl1=cand_pat->label(new_code.gid(last_vid));
    typename PATTERN::VERTEX_T lbl2=cand_pat->label(new_code.gid(back_vid));
    typename CAN_CODE::FIVE_TUPLE new_tuple(last_vid, back_vid, lbl1, e, lbl2);
    if(new_tuple<current_code[code_index]) {
#ifdef PRINT
      cout<<"decision at back-edge: new tuple is more minimal"<<endl;
      cout<<"new_tuple="<<new_tuple<<endl;
      cout<<"curent_tuple="<<current_code[code_index]<<endl;
      cout<<"current_code"<<current_code<<endl;
#endif
      return false;
    }

    if(current_code[code_index]<new_tuple) {
#ifdef PRINT
      cout<<"decision at back-edge: current tuple is more minimal"<<endl;
      cout<<"new_tuple="<<new_tuple<<endl;
      cout<<"curent_tuple="<<current_code[code_index]<<endl;
      cout<<"current_code"<<current_code<<endl;
#endif
      return true;
    }
    else {
      new_code.append(new_tuple);
      // no changes to new_code's rmp, nor to the cid-gid mappings since
      // back edge implies both vertices were already present in it
      return minimal(cand_pat, new_code);
    }
  }
  
  // execution gets here iff back-edge add failed
  ///// try to add a FWD EDGE /////
  // how to: find the deepest outgoing edge, and among all such edges find the 
  // minimal one, i.e. least edge-label + least dest-label

  bool fwd_found=false;
  rmp_it=rmp.end()-1;
  last_vid=*rmp_it;
  vector<int> new_vids; // equivalent minimal vertices whose minimality has to be checked recursively
  e=ed_prsr.parse_element(INF_LABEL);
  typename PATTERN::VERTEX_T dest_v=vt_prsr.parse_element(INF_LABEL);

  while(rmp_it>=rmp.begin()) {
    // get neighbors of current vertex
    eit_p=cand_pat->out_edges(new_code.gid(*rmp_it));

    // try to find minimal fwd edge
    while(eit_p.first!=eit_p.second) {
      if(new_code.cid(eit_p.first->first)!=-1) {
        // this vertex is already part of minimal code (CHECK THIS ??)
        eit_p.first++;
        continue;
      }

      if(eit_p.first->second<=e) {
        // minimal edge
        typename PATTERN::VERTEX_T curr_lbl=cand_pat->label(eit_p.first->first);
        if(eit_p.first->second<e) {
          // new minimal edge found
          new_vids.clear();
          e=eit_p.first->second;
          dest_v=curr_lbl;
        }

        if(curr_lbl<=dest_v) {
          // minimal dest label
          if(curr_lbl<dest_v) {
            new_vids.clear();
            dest_v=curr_lbl;
          }
          new_vids.push_back(eit_p.first->first);
        }
      }
      eit_p.first++;
    }//end while eit_p.first..

    if(!new_vids.empty()) {
      // fwd extension found at this level
      fwd_found=true;
      break;
    }

    rmp_it--;
    rmp.pop_back();
  }//end while !fwd_found..

  if(!fwd_found || rmp_it<rmp.begin()) {
    // no fwd edge extension could be found i.e. all fwd edges have been added
    // so this code is minimal
    //cout<<"Isomorphism decision at fwd-edge: all edges exhausted"<<endl;
    return true;
  }

  typename CAN_CODE::FIVE_TUPLE new_tuple(*rmp_it, last_vid+1, cand_pat->label(new_code.gid(*rmp_it)), e, cand_pat->label(new_vids[0]));
  if(new_tuple<current_code[code_index]) {
    // new edge is more minimal
#ifdef PRINT
    cout<<"decision at fwd-edge: new_edge is more minimal"<<endl;
    cout<<"new_tuple: "<<new_tuple<<endl;
    cout<<"current_tuple: "<<current_code[code_index]<<endl;
    cout<<"current_code="<<current_code<<endl;
#endif
    return false;
  }

  if(current_code[code_index]<new_tuple) {
    // current tuple is more minimal
#ifdef PRINT
    cout<<"decision at fwd-edge: current tuple is more minimal"<<endl;
    cout<<"new_tuple: "<<new_tuple<<endl;
    cout<<"current_tuple: "<<current_code[code_index]<<endl;
    cout<<"current_code="<<current_code<<endl;
#endif
    return true;
  }

  // check minimality against each new code
  for(unsigned int i=0; i<new_vids.size(); i++) {
    CAN_CODE next_code=new_code;
    next_code.append(new_tuple, new_code.gid(*rmp_it), new_vids[i]);
    next_code.append_rmp(last_vid+1);
    if(!minimal(cand_pat, next_code))
      return false;
  }

  // all codes were greater than cand_pat's code
#ifdef PRINT
  cout<<"decision at fwd-edge: this code more minimal than all codes"<<endl;
#endif
  return true;

}//end minimal()

#endif
