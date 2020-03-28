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
#ifndef _SEQ_VAT_H_
#define _SEQ_VAT_H_

#include "pattern.h"
#include "seq_instance.h"
#include "generic_classes.h"

#include "typedefs.h"

/**
* \brief SEQUENCE VAT calss for embedded mining by partial specialization of the generic VAT class.
 *
 * In this partial specialization, PP is fixed to directed, acyclic, indegree_lte_one, 
 * outdegree_lte_one (Tree property), MP is fixed to Fk X Fk, vert_mine, embedded (Vertical mining with Fk X Fk for embedded patterns), ST is the VAT storage type.
 */
template<class PP, class MP, template <typename> class ALLOC, template <typename, typename> class VAT_ST >
ostream& operator<< (ostream& ostr, const vat<SEQ_PROP, V_Fkk_MINE_PROP, ALLOC, VAT_ST>& svat);

template<class PP, class MP, template <typename> class ALLOC, template <typename, typename> class VAT_ST >
class vat<SEQ_PROP, V_Fkk_EMB_MINE_PROP, ALLOC, VAT_ST> 
{
public:  
  typedef pattern_support<V_Fkk_EMB_MINE_PROP> PAT_SUP;
  typedef vat<SEQ_PROP, V_Fkk_EMB_MINE_PROP, ALLOC, VAT_ST> VAT;
  typedef seq_instance<V_Fkk_EMB_MINE_PROP> INSTANCE;
  typedef VAT_ST<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >, ALLOC<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > > > > IDLIST_T;
  typedef typename IDLIST_T::const_iterator CONST_IT;
  typedef typename IDLIST_T::iterator IT;
  
  typedef VAT_ST<INSTANCE, ALLOC<INSTANCE> > INSTANCES;
  typedef typename INSTANCES::const_iterator CONST_INST_IT;/**< constant iterator over instances in a tid */
    
    void* operator new(size_t size) {
      ALLOC<VAT> va;
      return va.allocate(size);
    }  
    
    void  operator delete(void *p, size_t size) {
      if (p) {
        ALLOC<VAT> va;
        va.deallocate(static_cast<VAT*> (p), size);
      }
    }
    
    inline IT begin() {return _idlist.begin();}
    inline CONST_IT begin() const {return _idlist.begin();}
    inline IT end() {return _idlist.end();}
    inline CONST_IT end() const {return _idlist.end();}
    
    bool empty() const {return _idlist.empty;}
    int size() const{return _idlist.size();}
    
    void clear() {_idlist.clear();}
    
    void push_back(pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > > const& inst) {
      _idlist.push_back(inst);
    }
    
    // Return the size of the Vat in bytes  
    unsigned long int byte_size() const{
      unsigned long int  b_size=0;
      CONST_IT it;
      for (it = begin(); it!=end();++it){
        b_size+=(2+it->second.size())*sizeof(int);
      }
      return b_size;
    }
    
    //serializing a VAT to an output stream
    void write_file(ostream & output_file) {
      int ITSZ=sizeof(int);
      ostringstream output;
      CONST_IT it;
      CONST_INST_IT it_inst;
      int tid,inst_sz,ts;
      for (it=begin();it!=end();it++){
        tid=it->first;
        inst_sz=it->second.size();
        output.write(reinterpret_cast<const char *>(&tid), ITSZ);
        output.write(reinterpret_cast<const char *>(&inst_sz), ITSZ);
        for (it_inst =it->second.begin(); it_inst!=it->second.end(); ++it_inst){
          ts=it_inst->get_ts();  
          output.write(reinterpret_cast<const char *>(&ts), ITSZ);
        } //for it_inst
      }//it
      output_file.write(output.str().c_str(), output.str().size());
    } //end write_file
    
    
    //deserialize a vat from an input stream 
    void read_file (istream & input, unsigned long int size) {
      //cout<<"reading from the the file"<<endl;
      int ITSZ=sizeof(int);
      int buf_size=size/ITSZ;   
      int *buf = new int[buf_size];
      input.read((char *)buf, (size)); 
      int current=0;
      while(current<buf_size){
        int tid=buf[current++];
        int inst_sz=buf[current++];
        int inst_end=current+inst_sz;
        INSTANCES new_tidlist;
        while(current <inst_end){
          new_tidlist.push_back(INSTANCE(buf[current++]));
        }
        _idlist.push_back(make_pair(tid, new_tidlist));
      }      
      input.clear();
      delete [] buf;      
    } //read_file
    
    /**
      * Method performs intersection of two VATs. Returns the new candidate VATs.
     * Like the join of the patterns, a VAT intersection can result in more than one new candidate VAT.
     * But for itemsets, just one candidate VAT is generated.
     */
    template<typename PATTERN>
      static VAT** intersection(const VAT* const& vat_i, const VAT* const& vat_j, PAT_SUP** cand_sups, PATTERN**, bool& is_l2) {
        
        VAT** cand_vats = new VAT*[2]; // Max of 2 candidates possible for sequence extension.
        
        // Determine how many candidate vats.
        // If P->A join P->A then results in only one candidate vat.
        bool self_join_only = (cand_sups[1] == 0);   
        
        cand_vats[0]=new VAT; 
        if(!self_join_only)
          cand_vats[1]=new VAT;
        
        CONST_IT it_i=vat_i->begin(), it_j=vat_j->begin();
        
        if(it_i == vat_i->end() || it_j == vat_j->end())
          return cand_vats;
        
        while(it_i != vat_i->end() && it_j != vat_j->end()) {
          if(it_i->first < it_j->first) {
            it_i++;
            continue;
          }
          if(it_i->first > it_j->first) {
            it_j++;
            continue;
          }
          //execution reaches this point only if both TIDs are equal
          INSTANCES seqi_inst, seqj_inst;
          bool seqi_ind = false, seqj_ind = false, seqi_emb = false, seqj_emb = false;
          
          //intersect the instances for this tid
          INSTANCE::seq_join_seq(it_i->second, it_j->second, seqi_inst, seqj_inst, seqi_ind, 
                         seqj_ind, seqi_emb, seqj_emb, self_join_only);
          
          if(!seqi_inst.empty())
            cand_vats[0]->push_back(make_pair(it_i->first, seqi_inst));
          
          if(!seqj_inst.empty() && !self_join_only)
            cand_vats[1]->push_back(make_pair(it_i->first, seqj_inst));
          
          it_i++;
          it_j++;
          
        } //end while (it_i &* it_j)
        
        cand_sups[0]->set_sup(make_pair(cand_vats[0]->size(), cand_vats[0]->size()));
        if(!self_join_only)
          cand_sups[1]->set_sup(make_pair(cand_vats[1]->size(), cand_vats[1]->size()));
        
        return cand_vats;
      }
    friend ostream& operator<< <>(ostream&, const VAT&);
private:
      IDLIST_T _idlist;
};

template<class PP, class MP, template <typename> class ALLOC, template <typename, typename> class VAT_ST >
ostream& operator<< (ostream& ostr, const vat<SEQ_PROP, V_Fkk_MINE_PROP, ALLOC, VAT_ST>& svat) {
  typedef vat<SEQ_PROP, V_Fkk_MINE_PROP, ALLOC, VAT_ST> VAT;
  typename VAT::CONST_IT it=svat.begin();
  
  while(it!=svat.end()) {
    ostr<<it->first<<" ";
    typename VAT::CONST_INST_IT inst_it=it->second.begin();
    while(inst_it!=it->second.end())
      ostr<<(*inst_it++)<<" ";
    ostr<<endl;
    it++;
  }
  
  return ostr;
}//end friend operator<<




/**
* \brief SEQUENCE VAT calss for induced mining by partial specialization of the generic VAT class.
 *
 * In this partial specialization, PP is fixed to directed, acyclic, indegree_lte_one, 
 * outdegree_lte_one (Tree property), MP is fixed to Fk X Fk, vert_mine, induced(Vertical mining with Fk X Fk for induced patterns), ST is the VAT storage type.
 */
template<class PP, class MP, template <typename> class ALLOC, template <typename, typename> class VAT_ST >
class vat<SEQ_PROP, V_Fkk_IND_MINE_PROP, ALLOC, VAT_ST> {
public:
  
  typedef pattern_support<V_Fkk_IND_MINE_PROP> PAT_SUP;
  typedef vat<SEQ_PROP, V_Fkk_IND_MINE_PROP, ALLOC, VAT_ST> VAT;
  typedef seq_instance<V_Fkk_IND_MINE_PROP> INSTANCE;
  typedef VAT_ST<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >, ALLOC<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > > > > IDLIST_T;
  typedef typename IDLIST_T::const_iterator CONST_IT;
  typedef typename IDLIST_T::iterator IT;
  typedef typename IDLIST_T::reverse_iterator RIT;
  
  typedef VAT_ST<INSTANCE, ALLOC<INSTANCE> > INSTANCES;
  typedef typename INSTANCES::const_iterator CONST_INST_IT;/**< constant iterator over instances in a tid */
    
    
    void* operator new(size_t size) {
      ALLOC<VAT> va;
      return va.allocate(size);
    }
    
    void  operator delete(void *p, size_t size) {
      if (p) {
        ALLOC<VAT> va;
        va.deallocate(static_cast<VAT*> (p), size);
      }
    }
    
    IT begin() {return _idlist.begin();}
    CONST_IT begin() const {return _idlist.begin();}
    IT end() {return _idlist.end();}
    CONST_IT end() const {return _idlist.end();}
    bool empty() const {return _idlist.empty;}
    int size() const {return _idlist.size();}
    void clear() {_idlist.clear();}
    void push_back(pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > > const& inst) {
      _idlist.push_back(inst);
    }
    
    // Return the size of the Vat in bytes  
    unsigned long int byte_size() const{
      unsigned long int  b_size=0;
      CONST_IT it;
      for (it = begin(); it!=end();++it){
        b_size+=(2+it->second.size()*2)*sizeof(int);
      }
      return b_size;
    }
    
    //serializing a VAT to an output stream
    void write_file(ostream & output_file) {
      int ITSZ=sizeof(int);
      ostringstream output;
      CONST_IT it;
      CONST_INST_IT it_inst;
      int tid,inst_sz,induced,pos;
      for (it=begin();it!=end();it++){
        tid=it->first;
        inst_sz=it->second.size();
        output.write(reinterpret_cast<const char *>(&tid), ITSZ);
        output.write(reinterpret_cast<const char *>(&inst_sz), ITSZ);
        for (it_inst =it->second.begin(); it_inst!=it->second.end(); ++it_inst){
          induced=(int)it_inst->get_induced();
          pos=it_inst->get_pos();  
          output.write(reinterpret_cast<const char *>(&pos), ITSZ);
          output.write(reinterpret_cast<const char *>(&induced), ITSZ);
        } //for it_inst
      }//it
      output_file.write(output.str().c_str(), output.str().size());
    } //end write_file
    
    //deserialize a vat from an input stream 
    void read_file (istream & input, unsigned long int size) {
      //cout<<"reading from the the file"<<endl;
      int ITSZ=sizeof(int);
      int buf_size=size/ITSZ;   
      int *buf = new int[buf_size];
      input.read((char *)buf, (size)); 
      int current=0;
      while(current<buf_size){
        int tid=buf[current++];
        int inst_sz=buf[current++];
        int inst_end=current+inst_sz*2;
        INSTANCES new_tidlist;
        while(current <inst_end){
          new_tidlist.push_back(INSTANCE(buf[current], (buf[current+1])?true:false));
          current= current+2;

        }
        _idlist.push_back(make_pair(tid, new_tidlist));
      }      
      input.clear();
      delete [] buf;      
    } //read_file
    
    
    /**
      * Method performs intersection of two VATs. Returns the new candidate VATs.
     * Like the join of the patterns, a VAT intersection can result in more than one new candidate VAT.
     * But for itemsets, just one candidate VAT is generated.
     */
    template<typename PATTERN>
    static VAT** intersection(const VAT* const& vat_i, const VAT* const& vat_j, PAT_SUP** cand_sups, PATTERN**, bool& is_l2) {
      VAT** cand_vats = new VAT*[2]; // Max of 2 candidates possible for sequence extension.
                       // Determine how many candidate vats.
                       // If P->A join P->A then results in only one candidate vat.
      bool skip0 = (cand_sups[0] == 0);
      bool skip1 = (cand_sups[1] == 0);   

      cand_vats[0] = 0;
      cand_vats[1] = 0;

      if (!skip0)
	cand_vats[0]=new VAT; 
      if(!skip1)
        cand_vats[1]=new VAT;
      
      if (skip0 && skip1){
	return cand_vats;
      }
      
      CONST_IT it_i=vat_i->begin(), it_j=vat_j->begin();
      
      if(it_i==vat_i->end() || it_j==vat_j->end())
        return cand_vats;
      
      while(it_i!=vat_i->end() && it_j!=vat_j->end()) {
        if(it_i->first<it_j->first) {
          it_i++;
          continue;
        }
        if(it_i->first>it_j->first) {
          it_j++;
          continue;
        }
        //execution reaches this point only if both TIDs are equal
        INSTANCES seqi_inst, seqj_inst;
        bool seqi_ind = false, seqj_ind = false, seqi_emb = false, seqj_emb = false;
        
        //intersect the instances this tid
        INSTANCE::seq_join_seq(it_i->second, it_j->second, seqi_inst, seqj_inst, seqi_ind, 
			       seqj_ind, seqi_emb, seqj_emb, skip1, skip0);
        
	if (!skip0){
	  if(seqi_ind)
	    cand_sups[0]->incr_isup();
	  // else 
	  if(seqi_emb)
	    cand_sups[0]->incr_esup();
	}
	
        if(!skip1) {
          if(seqj_ind)
            cand_sups[1]->incr_isup();
          // else
          if(seqj_emb)
            cand_sups[1]->incr_esup();
        }
        
        if(!seqi_inst.empty() && !skip0)
          cand_vats[0]->push_back(make_pair(it_i->first, seqi_inst));
        
        if(!seqj_inst.empty() && !skip1)
          cand_vats[1]->push_back(make_pair(it_i->first, seqj_inst));
        
        it_i++;
        it_j++;
        
      } //end while (it_i &* it_j)
      
      //cout << "ISECT I : " << *vat_i << endl;
      //cout << "ISECT J : " << *vat_j << endl;
      //if (!skip0)
      //cout << "CAND A : " << *cand_vats[0] << endl;  
      //if (!skip1)
      //  cout << "CAND B: " << *cand_vats[1] << endl;        
      return cand_vats;
    }
    friend ostream& operator<< <>(ostream&, const VAT&);

private:
      IDLIST_T _idlist;
};
#endif

