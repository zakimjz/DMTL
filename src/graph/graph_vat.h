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
#ifndef _GRAPH_VAT_H
#define _GRAPH_VAT_H

#include <ext/hash_set>
#include "graph_evat.h"
#include "pattern.h"
#include "generic_classes.h"
#include "typedefs.h"

//#include "pat_support.h"


time_tracker tt_vat, tt_fwd_isect, tt_back_isect;
int fwd_isect_cnt=0;
int back_isect_cnt=0;


template<typename PP, typename MP, template <typename> class ALLOC, 
         template<typename, typename> class ST>
class vat<GRAPH_PROP, V_Fk1_MINE_PROP, ALLOC, ST>;

template<typename PP, typename MP, template <typename> class ALLOC,
         template<typename, typename> class ST>
ostream& operator<< (ostream& ostr, const vat<PP, MP, ALLOC, ST>* v);

/** Graph vat class */
// NOTE: ST should model a vector, else this class shall not compile
// vectors are used to make the design efficient

/**
 * \brief Graph VAT class by partial specialization of the generic VAT class.
 *
 * In this partial specialization, PP is fixed to undirected (undirected graph property),
 * MP is fixed to Fk X F1 and vert_mine (vertical mining with FK X F1),
 * ST is the VAT storage type. For graph, ST should model a vector, else this
 * shall not compile.
 */

template<typename PP, typename MP, template <typename> class ALLOC, template<typename, typename> class ST>
class vat<GRAPH_PROP, V_Fk1_MINE_PROP, ALLOC, ST>
{
 public:

  typedef evat<ALLOC> EVAT;
  typedef vat<GRAPH_PROP, V_Fk1_MINE_PROP, ALLOC, ST> VAT;
  typedef ST<EVAT, ALLOC<EVAT> > RMP_VATS;
  typedef ST<pair<int, RMP_VATS>, ALLOC<pair<int, RMP_VATS> > > GVAT;
  /**< a graph-vat is a collection of evats for each vertex, where each evat 
     must have same size. This collection of evats is organized itself as 
     ST<EVAT> evats, and it holds evats of all edges on right most path 
     of cand_pat */
  typedef HASHNS::hash_set<int, HASHNS::hash<int>, std::equal_to<int>, ALLOC<int> > VSET; /**< Set of vertex ids denoting exactly 
                                                                                               one of this graph's occurence in 
                                                                                               the dataset */
  typedef vector<vector<VSET, ALLOC<VSET> >, ALLOC<vector<VSET, ALLOC<VSET> > > > VSETS; /**< This graph can occur several times 
                                                                                              in one graph in the dataset, and in 
                                                                                              several graphs (tids) as well */
  typedef typename GVAT::const_iterator CONST_IT;
  typedef typename GVAT::iterator IT;
  typedef typename ST<EVAT, ALLOC<EVAT> >::const_iterator CONST_EIT;
  typedef typename VSETS::iterator VS_IT;
  typedef typename VSETS::const_iterator CONST_VS_IT;

  void* operator new(size_t size) {
    ALLOC<VAT> v;
    return v.allocate(size);
  }

  void  operator delete(void *p, size_t size) {
    if (p) {
      ALLOC<VAT> v;
      v.deallocate(static_cast<VAT*> (p), size);
    }
  }

  IT begin() { 
    return _vat.begin();
  }
  CONST_IT begin() const { 
    return _vat.begin();
  }
  IT end() { 
    return _vat.end();
  }
  CONST_IT end() const { 
    return _vat.end();
  }

  VS_IT begin_v() { 
    return _vids.begin();
  }
  CONST_VS_IT begin_v() const { 
    return _vids.begin();
  }
  VS_IT end_v() { 
    return _vids.end();
  }
  CONST_VS_IT end_v() const { 
    return _vids.end();
  }

  friend ostream& operator<< <>(ostream&, const VAT*);

  int size() const { 
    return _vat.size();
  }

  bool empty() const { 
    return _vat.empty();
  }

  const pair<int, ST<EVAT, ALLOC<EVAT> > >& back() const { 
    return _vat.back();
  }

  void insert_occurrence_tid(const int& tid, const pair<int, int>& new_occurrence) {
    ST<EVAT, ALLOC<EVAT> > new_evats;
    evat<ALLOC> new_evat;
    new_evat.push_back(new_occurrence);
    new_evats.push_back(new_evat);
    _vat.push_back(make_pair(tid, new_evats));
  }//insert_new_occurrence()


  void insert_occurrence_evat(const pair<int, int>& new_occurrence) {
    evat<ALLOC> new_evat;
    new_evat.push_back(new_occurrence);
    _vat.back().second.push_back(new_evat);
  }//insert_occurrence_evat()

  void insert_occurrence(const pair<int, int>& new_occurrence) {
    _vat.back().second.back().push_back(new_occurrence);
  }


  void insert_vid_hs(const int& vid) { 
    VSET vs;
    vs.insert(vid);
    _vids.back().push_back(vs);
  }


  void insert_vid(const int& vid) { 
    _vids.back().back().insert(vid);
  }


  void insert_vid_tid(const int& vid) {
    VSET vset;
    vset.insert(vid);
    vector<VSET, ALLOC<VSET> > vsets;
    vsets.push_back(vset);
    _vids.push_back(vsets);      
  }//insert_vid_tid()


  void copy_vats(const pair<int, ST<evat<ALLOC>, ALLOC<evat<ALLOC> > > >& v1, const int& offset, const int& sz, bool swap=0) {
    int i;
    for(i=0; i<sz; i++)
      if(swap)
        _vat.back().second[i].push_back(make_pair(v1.second[i][offset].second, v1.second[i][offset].first));
      else
        _vat.back().second[i].push_back(v1.second[i][offset]);
  }//copy_vats()
  

  void copy_vats_tid(const pair<int, ST<evat<ALLOC>, ALLOC<evat<ALLOC> > > >& v1, const int& offset, const int& sz, bool swap=0) {
    int i;
    ST<EVAT, ALLOC<EVAT> > new_entry;
  
    for(i=0; i<sz; i++) {
      evat<ALLOC> tmp_evat;
      if(swap)
        tmp_evat.push_back(make_pair(v1.second[i][offset].second, v1.second[i][offset].first));
      else
        tmp_evat.push_back(v1.second[i][offset]);
        new_entry.push_back(tmp_evat);
    }
    _vat.push_back(make_pair(v1.first, new_entry));
  }//copy_vats_tid()


  void copy_vids_hs(const VSET& v1_vids) {
    
    VSET vs;
    typename VSET::const_iterator it;
    for(it=v1_vids.begin(); it!=v1_vids.end(); it++)
      vs.insert(*it);
    _vids.back().push_back(vs);
  }//copy_vids()


  void copy_vids_tid(const VSET& v1_vids) {
    VSET vset;
    typename VSET::const_iterator it;
    for(it=v1_vids.begin(); it!=v1_vids.end(); it++)
      vset.insert(*it);
    vector<VSET, ALLOC<VSET> > vsets;
    vsets.push_back(vset);
    _vids.push_back(vsets);
  }//copy_vids_tid()


  /** Main vat intersection function;
      It also populates support argument passed */
  // NOTE: only one candidate is generated in a FkxF1 join of graphs, 
  // hence only the first value in cand_pats should be inspected
  template<typename PATTERN, typename PAT_SUP>
  static VAT** intersection(const VAT* v1, const VAT* v2, PAT_SUP** cand_sups, PATTERN** cand_pats, bool) {
#ifdef PRINT
    cout<<"VAT intersection entered with v1="<<v1<<endl;
    cout<<"v2="<<v2<<endl;
#endif

    tt_vat.start();

    // How it works
    // 1. determine which edge of v1 is being extended and get its evat1
    // 2. determine right-most-path of candidate (rmp)
    // 3. find common occurrences in v2 and evat1
    // 4. copy these common occurrences to new_vat, and also copy evats in 
    // rmp corresponding to these common occurrences

    VAT* cand_vat=new VAT;
    bool is_fwd;
    bool is_fwd_chain=false; // flag to denote whether edge appended by 
    // intersection is at the root (which is when flag=0)
    bool l2_eq=(cand_pats[0]->size()==3) && (cand_pats[0]->label(0)==cand_pats[0]->label(1)); // special case in evat intersection for L-2 with first edge 
    // with equal vertex labels

    int rmp_index; // index of last vid on rmp common to cand_pat and its 
    // parent's rmp
    int new_edge_state=-1; // flag to denote if the new edge to be added has 
    // same labeled vertices, of the form A-A (flag=0); or is of the form 
    // A-B (flag=1); or is not canonical at all, of the form B-A (flag=2).
    // evat intersection needs to take this into account

    int rvid=cand_pats[0]->rmost_vid();
    int edge_vid=-1; // vid of the other vertex (other than rvid) connected 
    // to rvid as to form the new edge

    typename PATTERN::CONST_EIT_PAIR eit_p=cand_pats[0]->out_edges(rvid);
    int back_idx=-1; // this is used only for back extensions. It holds the 
    // index of edge_vid in rmp of cand_pats[0]

    if(eit_p.second-eit_p.first>1)
      is_fwd=false; // last edge was fwd edge only if outdegree of last vid=1
    else
      is_fwd=true;

    // get other vertex's vid
    if(is_fwd) {
      edge_vid=eit_p.first->first;
      if(!edge_vid) // rvid is attached to the root
        is_fwd_chain=0;
      else
        is_fwd_chain=1;
    }
    else {
      //prev_vid=eit_p.first->first;
      eit_p.second--;
      edge_vid=eit_p.second->first;

      /// now determine the index of edge_vid on rmp of candidate. This is 
      /// used by back_intersect.
      // TO DO: this is currently a linear search through rmp, is there a 
      // more efficient way??
      const typename PATTERN::RMP_T& cand_rmp=cand_pats[0]->rmost_path();
      for(unsigned int i=0; i<cand_rmp.size(); i++) {
        if(cand_rmp[i]==edge_vid) {
          back_idx=i;
          break;
        }
      }

      if(back_idx==-1) {
        cerr<<"vat.intersect: back_idx not found for edge_vid="<<edge_vid<<" in rmp.size="<<cand_rmp.size()<<endl;
        tt_vat.stop();
        return 0;
      }
    }

    // now determine which of v1's evats need to be copied into cand_vat: 
    // if is_fwd, only evats till edge_vid need be copied
    // else all evats need to be copied

    if(is_fwd)
      rmp_index=cand_pats[0]->rmp_size()-2;
    else
      rmp_index=cand_pats[0]->rmp_size()-1;

    if(cand_pats[0]->label(edge_vid)==cand_pats[0]->label(rvid))
      new_edge_state=0;
    else
      if(is_fwd)
        new_edge_state=(cand_pats[0]->label(edge_vid)>cand_pats[0]->label(rvid))+1;
      else
        new_edge_state=(cand_pats[0]->label(rvid)>cand_pats[0]->label(edge_vid))+1;
      
    CONST_IT it_v1=v1->begin();
    CONST_IT it_v2=v2->begin();
      
    // find a common TID
    while(it_v1!=v1->end() && it_v2!=v2->end()) {
      if(it_v1->first<it_v2->first) {
        it_v1++;
        continue;
      }
  
      if(it_v1->first>it_v2->first) {
        it_v2++;
        continue;
      }
  
      // execution reaches here only if both TIDs are equal
      const EVAT* v1_evat;
      const EVAT* v2_evat=&(it_v2->second[0]);
  
      if(!is_fwd)    
        v1_evat=&(it_v1->second[rmp_index-1]);
      else {
        if(is_fwd_chain)
          v1_evat=&((it_v1->second)[rmp_index-1]);
        else
          v1_evat=&((it_v1->second)[0]);
      }
  
      /// we now have both evats, intersect them ///
      // the intersection routines are expected to fill in the new evat in 
      // cand_vat
      if(is_fwd) {
        tt_fwd_isect.start();
        evat<ALLOC>::template fwd_intersect<ST, VAT, ALLOC>(*v1, *v1_evat, *v2_evat, *cand_vat, 
                                         is_fwd_chain, rmp_index, new_edge_state, 
                                         it_v1-v1->begin(), l2_eq);
        tt_fwd_isect.stop();
        fwd_isect_cnt++;
      }
      else {
        tt_back_isect.start();
        evat<ALLOC>::template back_intersect<ST, VAT, ALLOC>(*v1, *v1_evat, *v2_evat, *cand_vat, back_idx, new_edge_state, it_v1-v1->begin());
        tt_back_isect.stop();
        back_isect_cnt++;
      }

      it_v1++;
      it_v2++;
    }//end while

    cand_sups[0]->set_sup(make_pair(cand_vat->size(), 0));

    VAT** cand_vats=new VAT*;
    cand_vats[0]=cand_vat;
    tt_vat.stop();
    return cand_vats;
  }//end intersect()


    unsigned long int byte_size() const{
      unsigned long int  b_size=0;
      CONST_IT it;
      CONST_EIT eit;
      b_size += sizeof(int);
      for (it = begin(); it!=end();++it){
        b_size += 2*sizeof(int); //tid, number of evats

        for (eit = it->second.begin(); eit != it->second.end(); eit++){
          b_size+=(1*sizeof(int))+eit->byte_size(); // n, e[0], e[1] .. e[n]
        }
      }
      // (VIDS GOES HERE)
      typename VSETS::const_iterator vit;
      b_size += sizeof(int);
      for (vit = begin_v(); vit != end_v(); vit++){
        b_size += sizeof(int);
        typename vector<VSET>::const_iterator vvsetit;
        for (vvsetit=vit->begin(); vvsetit!=vit->end(); vvsetit++){
          b_size += (vvsetit->size()+1) * sizeof(int);
        }//vvsetit
      } //vit
      return b_size;
    }

    void print(){
      int ITSZ=sizeof(int);
      CONST_IT it;
      CONST_EIT eit;
      int tid,evat_n,evat_sz;
      int gvat_sz=_vat.size();
      cout << "size:" <<gvat_sz << endl;
      for (it=begin();it!=end();++it){
        tid=it->first;
        evat_n=it->second.size();
        cout << tid << " " << evat_n << endl;
        for (eit=it->second.begin(); eit!=it->second.end(); ++eit){
          evat_sz = eit->size();
          cout << evat_sz << endl;
          eit->print();
        } //for eit
      }//it
      // Writing _vids goes here.
      typename VSETS::iterator vit;
      int vvsetn = _vids.size();
      cout << "Vids size: " << vvsetn << endl;
      for (vit=begin_v(); vit!=end_v(); vit++){
        typename vector<VSET>::iterator vvsetit;
        int vsetn = vit->size();
        cout << vsetn << endl;
        for (vvsetit=vit->begin(); vvsetit!=vit->end(); vvsetit++){
          typename VSET::iterator vsetit;
          int n = vvsetit->size();
          cout << "- " << n << endl;
          for (vsetit=vvsetit->begin(); vsetit!=vvsetit->end(); vsetit++){
            int v=*vsetit;
            cout << "-- " << v << endl;
          }//vsetit
        }//vvsetit
      }//vit

    }

    //writing a VAT to a binary file
    void write_file(ostream & output) const{
      //ostringstream output;
      int ITSZ=sizeof(int);
      CONST_IT it;
      CONST_EIT eit;
      int tid,evat_n,evat_sz;
      int gvat_sz=_vat.size();
      output.write(reinterpret_cast<const char *>(&gvat_sz), ITSZ);
      for (it=begin();it!=end();++it){
        tid=it->first;
        evat_n=it->second.size();
        output.write(reinterpret_cast<const char *>(&tid), ITSZ);
        output.write(reinterpret_cast<const char *>(&evat_n), ITSZ);
        for (eit=it->second.begin(); eit!=it->second.end(); ++eit){
          evat_sz = eit->size();
          output.write(reinterpret_cast<const char *>(&evat_sz), ITSZ);
          eit->write_file(output);
        } //for eit
      }//it
      // Writing _vids goes here.
      typename VSETS::const_iterator vit;
      int vvsetn = _vids.size();
      output.write(reinterpret_cast<const char *>(&vvsetn), ITSZ);
      for (vit=begin_v(); vit!=end_v(); vit++){
        typename vector<VSET>::const_iterator vvsetit;
        int vsetn = vit->size();
        output.write(reinterpret_cast<const char *>(&vsetn), ITSZ);
        for (vvsetit=vit->begin(); vvsetit!=vit->end(); vvsetit++){
          typename VSET::iterator vsetit;
          int n = vvsetit->size();
          output.write(reinterpret_cast<const char *>(&n), ITSZ);
          for (vsetit=vvsetit->begin(); vsetit!=vvsetit->end(); vsetit++){
            int v=*vsetit;
            output.write(reinterpret_cast<const char *>(&v), ITSZ);
          }//vsetit
        }//vvsetit
      }//vit
      //output_file.write(output.str().c_str(), output.str().size());
    } //end write_file
    
    void read_file (istream & input, unsigned long int size) {
      int ITSZ=sizeof(int);
      int buf_size=size/ITSZ;   
      int *buf = new int[buf_size];
      input.read((char *)buf, (size)); 
      int current=0;
      int vats_size=buf[current++], vats_seen=0;
      while(vats_seen++ < vats_size){
        int tid=buf[current++];
        int evat_n=buf[current++];
        int evats_seen=0;
        RMP_VATS edges;
        while(evats_seen++ < evat_n){
          evat<ALLOC> new_evat;
          int evat_sz=buf[current++];
          while(evat_sz-- > 0){
            int f1, f2;
            f1 = buf[current++];
            f2 = buf[current++];
            new_evat.push_back(make_pair(f1, f2));
          }
          edges.push_back(new_evat);
        }
        _vat.push_back(make_pair(tid, edges));
      }
      //Reading _vids goes here.

      int vids_size=buf[current++], vids_seen=0;
      while(vids_seen++ < vids_size){
        vector <VSET> new_vsetv;
        int vsetv_n=buf[current++];
        int vsets_seen=0;
        while(vsets_seen++ < vsetv_n){
          VSET new_vset;
          int vset_sz=buf[current++];
          while(vset_sz-- > 0){
            int i = buf[current++];
            new_vset.insert(i);
          } // evat_sz
          new_vsetv.push_back(new_vset);
        }//vsets_seen
        _vids.push_back(new_vsetv);
      }//vids_seen

      //this->print();
      input.clear();
      delete [] buf;
    } //read_file

  /** Returns true if vid occurs in any of the offset-th vids in tid-th vat */
  bool is_new_vertex(const int& vid, const int& tid, const int& offset) const {
    if(_vids[tid][offset].find(vid)==_vids[tid][offset].end()) {
      return true;
    }
    return false;
  }//end is_new_vertex()


  friend class evat<ALLOC>; // required for intersect functions in evat to work
  
 private:
  GVAT _vat;
  VSETS _vids;

}; //end class vat for graphs


template<typename PP, typename MP, template <typename> class ALLOC, template<typename, typename> class ST>
  ostream& operator<< (ostream& ostr, const vat<PP, MP, ALLOC, ST>* v) {
  typename vat<PP, MP, ALLOC, ST>::CONST_IT it;
  typename vat<PP, MP, ALLOC, ST>::RMP_VATS::const_iterator rit;

  ostr<<"VAT:"<<endl;
  for(it=v->begin(); it!=v->end(); it++) {
    ostr<<"tid="<<it->first<<endl;
    for(rit=it->second.begin(); rit!=it->second.end(); rit++)
      ostr<<*rit<<endl;
  }

  // These lines print out the vid-sets
  typename vat<PP, MP, ALLOC, ST>::VSETS::const_iterator vit1;
  // typename vector<typename vat<PP, MP, ALLOC, ST>::VSET, ALLOC<typename vat<PP, MP, ALLOC, ST>::VSET> >::const_iterator vit2;
  typename vector<typename vat<PP, MP, ALLOC, ST>::VSET >::const_iterator vit2;
  typename vat<PP, MP, ALLOC, ST>::VSET::const_iterator vit3;

  ostr<<"Vertices are"<<endl;
  for(vit1=v->begin_v(), it=v->begin(); vit1!=v->end_v(); vit1++, it++) {
    ostr<<"tid="<<it->first<<endl;
    for(vit2=vit1->begin(); vit2!=vit1->end(); vit2++) {
      for(vit3=vit2->begin(); vit3!=vit2->end(); vit3++)
        ostr<<*vit3<<" ";
        ostr<<endl;
    }
  }
      
  return ostr;
}//operator<< for vat*

#endif
