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
#ifndef _GRAPH_CAND_GEN_H
#define _GRAPH_CAND_GEN_H

#include "level_one_hmap.h"
#include "typedefs.h"

extern unsigned long int freq_pats_count;
extern bool print;

// candidate generation for graphs
template<typename PP, class MP, class PAT_ST, template<class, typename, typename, template <typename> class > class CC, template <typename> class ALLOC, class EDGE_MAP, class SM_TYPE>
void cand_gen(GRAPH_PATTERN* pat, const EDGE_MAP& emap, const int& minsup, pat_fam<GRAPH_PATTERN >& freq_pats, 
              count_support<GRAPH_PROP, V_Fk1_MINE_PROP, PAT_ST, CC, ALLOC, SM_TYPE>& cs) {

#ifdef PRINT
  cout<<"In call to cand_gen"<<endl;  
#endif
  typedef pat_fam<GRAPH_PATTERN> PAT_FAM;
 
  /// add back edges to get new candidates ///
  back_extensions(pat, emap, minsup, freq_pats, cs);

  /// add fwd edges to get new candidates ///
  fwd_extensions(pat, emap, minsup, freq_pats, cs);

}//end cand_gen()


template<typename PP, class MP, class PAT_ST, template<class, typename, typename, template <typename> class> class CC, template <typename> class ALLOC, class EDGE_MAP, class SM_TYPE>
void back_extensions(GRAPH_PATTERN* pat, const EDGE_MAP& emap, const int& minsup, pat_fam<GRAPH_PATTERN >& freq_pats, 
                     count_support<GRAPH_PROP, V_Fk1_MINE_PROP, PAT_ST, CC, ALLOC, SM_TYPE>& cs) {

  if(pat->rmp_size()<3) // back extensions only for graphs with rmost path 
    return;             // with atleast two edges
#ifdef PRINT
  cout<<"Back extension for "<<pat<<endl;
#endif
  typedef pat_fam<GRAPH_PATTERN> PAT_FAM;

  const typename GRAPH_PATTERN::VERTEX_T& last_v=pat->rmost_vertex();
  const typename GRAPH_PATTERN::RMP_T& rmp=pat->rmost_path();
  typename GRAPH_PATTERN::RMP_T::const_iterator rmp_it;
  typename GRAPH_PATTERN::EDGE_T e;
  GRAPH_PATTERN* edge=0;
  GRAPH_PATTERN* cand_pat=0;
  int rvid=pat->rmost_vid();
  int vid;

  // determine vid on rmp from where to start adding back edges
  rmp_it=rmp.end()-3;
  while(true) {
    if(rmp_it<rmp.begin())
      break;
    if(pat->get_out_edge(rvid, *rmp_it, e)) {
      rmp_it++;
      break;
    }
    rmp_it--;
  }

  if(rmp_it<rmp.begin())
    rmp_it++;

  while(rmp_it<rmp.end()-2) {
    vid=*rmp_it;
    const typename GRAPH_PATTERN::VERTEX_T& back_v=pat->label(vid);

    const typename EDGE_MAP::LABELS& lbls=emap.get_labels(last_v, back_v);
    typename EDGE_MAP::CONST_LIT lit=lbls.begin();

    // get all possible labels for this back edge
    while(lit!=lbls.end()) {

      // construct new candidate with this label
      cand_pat=pat->clone();
      cand_pat->add_out_edge(rvid, vid, *lit);
      cand_pat->add_out_edge(vid, rvid, *lit);

      if(!check_isomorphism(cand_pat)) {
        delete cand_pat;
        cand_pat=0;
        lit++;
        continue;
      }

      // create the one edged-pattern
      edge=new GRAPH_PATTERN;
      if(last_v<back_v)
        make_edge(edge, last_v, back_v, *lit);
      else
        make_edge(edge, back_v, last_v, *lit);
#ifdef PRINT
      cout<<"Trying with back edge="<<edge<<endl;
      cout<<"New candidate="<<cand_pat<<endl;
#endif
      // count support
      cs.count(pat, edge, &cand_pat, minsup, 1);

      delete edge;
      edge=0;

      // recursive call for frequent graph
      if(cand_pat->is_valid(minsup)) {
        //freq_pats.push_back(cand_pat);
        freq_pats_count++;
        if(print)
          cout << cand_pat;
        cand_gen(cand_pat, emap, minsup, freq_pats, cs);
        cs.delete_vat(cand_pat);
      }
      // else {
      delete cand_pat;
      cand_pat=0;
      // }

      lit++;
    }//end while lit

    rmp_it++;
  }//end while rmp_it

}//end back_extensions()


template<typename PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class > class CC, template <typename> class ALLOC, class EDGE_MAP, class SM_TYPE>
void fwd_extensions(GRAPH_PATTERN* pat, const EDGE_MAP& emap, const int& minsup, pat_fam<GRAPH_PATTERN >& freq_pats, 
                    count_support<GRAPH_PROP, V_Fk1_MINE_PROP, PAT_ST, CC, ALLOC, SM_TYPE>& cs) {
#ifdef PRINT
  cout<<"Fwd extension for "<<pat<<endl;
#endif
  typedef pat_fam<GRAPH_PATTERN> PAT_FAM;

  const typename GRAPH_PATTERN::RMP_T& rmp=pat->rmost_path();
  typename GRAPH_PATTERN::RMP_T::const_iterator rmp_it;
  
  typename EDGE_MAP::CONST_NIT nit;
  typename EDGE_MAP::CONST_LIT lit;  
  GRAPH_PATTERN* edge=0;
  GRAPH_PATTERN* cand_pat=0;
  int lvid;
  
  for(rmp_it=rmp.end()-1; rmp_it>=rmp.begin(); rmp_it--) {
    const typename GRAPH_PATTERN::VERTEX_T& src_v=pat->label(*rmp_it);
    const typename EDGE_MAP::NEIGHBORS& nbrs=emap.get_neighbors(src_v);

    for(nit=nbrs.begin(); nit!=nbrs.end(); nit++) {
      const typename GRAPH_PATTERN::VERTEX_T& dest_v=nit->first;

      for(lit=nit->second.begin(); lit!=nit->second.end(); lit++) {
        // construct new candidate
        cand_pat=pat->clone();
        lvid=cand_pat->add_vertex(dest_v);
        cand_pat->add_out_edge(*rmp_it, lvid, *lit);
        cand_pat->add_out_edge(lvid, *rmp_it, *lit);
#ifdef PRINT
        // cout<<"New candidate="<<cand_pat<<endl;
#endif
        if(!check_isomorphism(cand_pat)) {
#ifdef PRINT
          cout<<"New candidate="<<cand_pat<<endl;
          cout<<"candidate did NOT pass isomorphism"<<endl;
#endif
          delete cand_pat;
          cand_pat=0;
          continue;
        } 
        else {
#ifdef PRINT
          cout<<"New candidate="<<cand_pat<<endl;
          cout<<"candidate passed isomorphism"<<endl;
#endif
        }

        // create edge
        edge=new GRAPH_PATTERN;
        if(src_v<dest_v)
          make_edge(edge, src_v, dest_v, *lit);
        else
          make_edge(edge, dest_v, src_v, *lit);

#ifdef PRINT
        cout<<"Trying with edge="<<edge<<endl;
#endif
        // count support
        cs.count(pat, edge, &cand_pat, minsup, 1);
     
        delete edge;
        edge=0;
      
        // recursive call for frequent graph
        if(cand_pat->is_valid(minsup)) {
          // freq_pats.push_back(cand_pat);
          if(print)
            cout << cand_pat;
          freq_pats_count++;
          cand_gen(cand_pat, emap, minsup, freq_pats, cs);
          cs.delete_vat(cand_pat);
        }
        // else {
        delete cand_pat;
        cand_pat=0;
        // }

      }//end for lit
    }//end for nit      
  }//end for rmp_it

}//end fwd_extensions()


/** Populates p with a single-edged pattern;
    v1 is first vertex, v2 is second; 
    It also populates p's canonical code */
// it is assumed that v1<=v2 for VAT intersection to work, this function  does 
// not verify it;
// in particular, storage_manager for a single undirected edge (A-B) stores it 
// as canonical code A-B and not B-A.

template<typename pattern, typename V_T, typename E_T>
void make_edge(pattern* p, const V_T& v1,const V_T& v2, const E_T& e) {
  p->add_vertex(v1);
  p->add_vertex(v2);
  p->add_out_edge(0, 1, e);
  p->add_out_edge(1, 0, e);
  p->init_canonical_code(five_tuple<V_T, E_T>(0, 1, p->label(0), e, p->label(1)));
} //end make_edge()

#endif
