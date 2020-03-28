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
#ifndef _LEVEL_ONE_HMAP_H
#define _LEVEL_ONE_HMAP_H

#include <ext/hash_map>
#include <ext/hash_set>
#include <iostream>
#include <utility>


using namespace std;

/**
 * \brief Class to store edges i.e. level one patterns for graphs;
 *
 * This class is designed to provide a fast lookup of possible labels/vertices
 */
template<typename V_T, typename E_T, template <typename> class ALLOC=std::allocator >
class level_one_hmap
{
public:
  typedef element_parser<V_T> V_EP;

  // typedef HASHNS::hash_set<E_T, HASHNS::hash<E_T>, std::equal_to<E_T>, ALLOC<const E_T> > LABELS;
  typedef HASHNS::hash_set<E_T, HASHNS::hash<E_T>, std::equal_to<E_T>, ALLOC<E_T> > LABELS;
  typedef HASHNS::hash_map<typename V_EP::HASH_TYPE, LABELS, HASHNS::hash<typename V_EP::HASH_TYPE>, 
                           // typename V_EP::COMP_FUNC, ALLOC<std::pair<const typename V_EP::HASH_TYPE, LABELS> > > NEIGHBORS;
                           typename V_EP::COMP_FUNC, ALLOC<std::pair<typename V_EP::HASH_TYPE, LABELS> > > NEIGHBORS;
  typedef HASHNS::hash_map<typename V_EP::HASH_TYPE, NEIGHBORS, HASHNS::hash<typename V_EP::HASH_TYPE>, 
                           // typename V_EP::COMP_FUNC, ALLOC<std::pair<const typename V_EP::HASH_TYPE, NEIGHBORS> > > HMAP;
                           typename V_EP::COMP_FUNC, ALLOC<std::pair<typename V_EP::HASH_TYPE, NEIGHBORS> > > HMAP;
  typedef typename HMAP::const_iterator CONST_IT;
  typedef typename HMAP::iterator IT;
  typedef typename NEIGHBORS::const_iterator CONST_NIT;
  typedef typename NEIGHBORS::iterator NIT;
  typedef typename LABELS::const_iterator CONST_LIT;
  typedef typename LABELS::iterator LIT;


  void print() const {
    cout<<"LEVEL ONE HMAP CONTENTS"<<endl<<endl;

    CONST_IT it;
    CONST_NIT nit;
    CONST_LIT lit;

    for(it=_hmap.begin(); it!=_hmap.end(); it++) {
      cout<<"Vertex="<<it->first<<" has neighbors:"<<endl;
      for(nit=it->second.begin(); nit!=it->second.end(); nit++) {
        cout<<nit->first<<" with labels:";
        for(lit=nit->second.begin(); lit!=nit->second.end(); lit++)
          cout<<" "<<*lit;
          cout<<endl;
        }
        cout<<endl;
      }

  }//print()

  int size() const { return _hmap.size();}

  /** Inserts the edge from src to dest with label lbl in the hmap;
      If any of src/dest are not present in hmap, they are inserted as well;
      This hmap does not permit parallel edges between two nodes */
  // NOTE: in order for this map to be applicable to both directed and
  // undirected graphs, insert adds an edge FROM src TO dest only;
  // hence for undirected graphs, insert shall have to be called twice
  void insert(const V_T& src, const V_T& dest, const E_T& lbl) {
    IT it;
    NIT nit;
    LIT lit;
    pair<LIT, bool> lit_p;
    pair<NIT, bool> nit_p;
    pair<IT, bool> it_p;
   
    typename V_EP::HASH_TYPE ret = V_EP::conv_hash_type(src); 
    // if((it=_hmap.find(V_EP::conv_hash_type(src)))!=_hmap.end()) {
    if((it=_hmap.find(ret))!=_hmap.end()) {
      // src exists in hmap
      if((nit=it->second.find(V_EP::conv_hash_type(dest)))!=it->second.end()) {
        // dest exists in neighbor list
        if(nit->second.find(lbl)==nit->second.end()) {
          // lbl does not exist
          lit_p=nit->second.insert(lbl);
          if(!lit_p.second) {
            cout<<"level_one_map.insert: lbl insert(1) failed for lbl="<<lbl<<endl;
            return;
          }
        }
      }
      else {
        // dest not found in neighbor list of src
        LABELS lset;
        lit_p=lset.insert(lbl);

        if(!lit_p.second) {
          cout<<"level_one_map.insert: lbl insert(2) failed for lbl="<<lbl<<endl;
          return;
        }
    
        nit_p=it->second.insert(make_pair(V_EP::conv_hash_type(dest), lset));
        if(!nit_p.second) {
          cout<<"level_one_map.insert: dest insert(1) failed for dest="<<dest<<endl;
          return;
        }    
      }
    }//end if it=hmap.find..

    else {

      // src not found in hmap
      NEIGHBORS nbr;
      LABELS lset;
      lit_p=lset.insert(lbl);

      if(!lit_p.second) {
        cout<<"level_one_map.insert: lbl insert(3) failed for lbl="<<lbl<<endl;
        return;
      }

      nit_p=nbr.insert(make_pair(V_EP::conv_hash_type(dest), lset));
      if(!nit_p.second) {
        cout<<"level_one_map.insert: dest insert(2) failed for dest="<<dest<<endl;
        return;
      }

      it_p=_hmap.insert(make_pair(V_EP::conv_hash_type(src), nbr));
      if(!it_p.second) {
        cout<<"level_one_map.insert: src insert(1) failed for src="<<src<<endl;
        return;
      }
    }//end else it!=hmap.find ..

  }//end insert()


  const LABELS& get_labels(const V_T& src, const V_T& dest) const {
    CONST_IT it=_hmap.find(V_EP::conv_hash_type(src));
    if(it==_hmap.end()) {
      cout<<"level_one_map.get_labels: src not found in hmap for src="<<src<<"*"<<endl;
      exit(0);
    }
    
    CONST_NIT nit=it->second.find(V_EP::conv_hash_type(dest));
    if(nit==it->second.end()) {
      return _empty_lbls;
    }

    return nit->second;
  }//end get_labels()

  const NEIGHBORS& get_neighbors(const V_T& src) const {
    CONST_IT it=_hmap.find(V_EP::conv_hash_type(src));
    if(it==_hmap.end()) {
      cout<<"level_one_map.get_neighbors: src not found in hmap for src="<<src<<endl;
      exit(0);
    }
    return it->second;
  }//end get_neighbors()


private:
    // HMAP _hmap(HASHNS::hash<typename V_EP::HASH_TYPE>(), ALLOC<std::pair<const typename V_EP::HASH_TYPE, NEIGHBORS> >());
    HMAP _hmap;
    LABELS _empty_lbls;
    
};//end class level_one_map

#endif
