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
// tree VAT classes

#ifndef _TREE_VAT_H
#define _TREE_VAT_H

#include <vector>
#include <utility>
#include <sstream>

#include "pattern.h"
#include "pat_support.h"
#include "tree_instance.h"
#include "generic_classes.h"

using namespace std;


/*
template<typename PP, typename MP, template <typename> class ALLOC,
         template<typename, typename> class ST>
class vat<TREE_PROP, V_Fkk_EMB_MINE_PROP, ALLOC, ST>;

template<typename PP, typename MP, template <typename> class ALLOC,
         template<typename, typename> class ST>
class vat<TREE_PROP, V_Fkk_IND_MINE_PROP, ALLOC, ST>;
template<typename PP, typename MP, template <typename> class ALLOC,
         template<typename, typename> class VAT_ST>
ostream& operator<< (ostream& ostr, const vat<TREE_PROP, V_Fkk_EMB_MINE_PROP, ALLOC, VAT_ST>*);

template<typename PP, typename MP, template <typename> class ALLOC,
         template<typename, typename> class VAT_ST>
ostream& operator<< (ostream& ostr, const vat<TREE_PROP, V_Fkk_IND_MINE_PROP, ALLOC, VAT_ST>*);
*/
template<typename PP, typename MP, template <typename> class ALLOC,
         template<typename, typename> class VAT_ST>
ostream& operator<< (ostream& ostr, const vat<TREE_PROP, V_Fkk_MINE_PROP, ALLOC, VAT_ST>* );
/**
 * \brief TREE VAT calss for embedded mining by partial specialization of the generic VAT class.
 *
 * In this partial specialization, PP is fixed to directed, acyclic, indegree_lte_one 
 * (Tree property), MP is fixed to Fk X Fk, vert_mine, embedded (Vertical mining
 * with Fk X Fk for embedded patterns), ST is the VAT storage type.
 */
template<typename PP, typename MP, template <typename> class ALLOC, template<typename, typename> class VAT_ST>
class vat<TREE_PROP, V_Fkk_EMB_MINE_PROP, ALLOC, VAT_ST>
{
  
 public:
  template<typename T1, typename A>
    class ST: public VAT_ST<T1, A> {};

  typedef pattern_support<V_Fkk_EMB_MINE_PROP > PAT_SUP;
  typedef vat<TREE_PROP, V_Fkk_EMB_MINE_PROP, ALLOC, VAT_ST> VAT;
  typedef tree_instance<std::vector, PP> INSTANCE;
  typedef VAT_ST<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >, ALLOC<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > > > > IDLIST_T;
  typedef typename IDLIST_T::const_iterator CONST_IT;
  typedef typename IDLIST_T::iterator IT;
  typedef typename VAT_ST<INSTANCE, ALLOC<INSTANCE> >::const_iterator CONST_INST_IT;
  typedef VAT_ST<INSTANCE, ALLOC<INSTANCE> > INSTANCES;
  typedef typename VAT_ST<INSTANCE, ALLOC<INSTANCE> >::iterator INST_IT;
  typedef typename std::vector<int>::const_iterator CONST_STORE_IT;
  typedef typename std::vector<int>::iterator STORE_IT; 

  void* operator new(size_t size) {
    ALLOC<VAT> v;
    return (v.allocate(size));
  } 

  void  operator delete(void *p, size_t size) {
    if (p) {
      ALLOC<VAT> v;
      v.deallocate(static_cast<VAT*> (p), size);
    }
  }

  
  IT begin() {return _idlist.begin();}
  CONST_IT begin() const {return _idlist.begin();}
  IT end() {return _idlist.end();}
  CONST_IT end() const {return _idlist.end();}
  
  bool empty() const {return _idlist.empty();}
  int size() {return _idlist.size();}
  
  /** Appends new entry into vat */
  void push_back(const pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >& val) {
    _idlist.push_back(val);
  }

  unsigned long int byte_size() const{
    unsigned long int  b_size=0;
    CONST_IT it;
    CONST_INST_IT it_inst;
    //for each instance we will store 5 integer (1 for the vector size) and the vector
    for (it = begin(); it!=end();++it){
      b_size+=2*sizeof(int);
      for (it_inst=it->second.begin();it_inst!=it->second.end();++it_inst)
        b_size+=(it_inst->get_ml_size()+5)*sizeof(int);
    }
    return b_size;
  }

  //writing a VAT to a binary file
  void write_file(ostream & output_file) {
    int ITSZ=sizeof(int);
    ostringstream output;
    CONST_IT it;
    CONST_INST_IT it_inst;
    CONST_STORE_IT it_store;
    int tid,inst_sz,ml_sz,_lb,_ub,_depth,_type;
    for (it=begin();it!=end();++it){
      tid=it->first;
      inst_sz=it->second.size();
      output.write(reinterpret_cast<const char *>(&tid), ITSZ);
      output.write(reinterpret_cast<const char *>(&inst_sz), ITSZ);
      for (it_inst =it->second.begin(); it_inst!=it->second.end(); ++it_inst){
        ml_sz=it_inst->get_ml_size();
        output.write(reinterpret_cast<const char *>(&ml_sz), ITSZ);
        for (it_store=it_inst->get_ml_begin();it_store!=it_inst->get_ml_end();++it_store){
          output.write(reinterpret_cast<const char *>(&(*it_store)), ITSZ);
        }          
        _lb=it_inst->get_lb();  
        output.write(reinterpret_cast<const char *>(&_lb), ITSZ);
        _ub=it_inst->get_ub();  
        output.write(reinterpret_cast<const char *>(&_ub), ITSZ);
        _depth=it_inst->get_depth();  
        output.write(reinterpret_cast<const char *>(&_depth), ITSZ);
        _type=(int)it_inst->induced();  
        output.write(reinterpret_cast<const char *>(&_type), ITSZ);
      } //for it_inst
    }//it
     output_file.write(output.str().c_str(), output.str().size());
  }
    
  void read_file (istream & input, unsigned long int size) {
    //cout<<"reading from the the file"<<endl;
    int ITSZ=sizeof(int);
    int buf_size=size/ITSZ;   
    // cout<<buf_size;
    int *buf = new int[buf_size];
    input.read((char *)buf, (size));
    int current=0;
    while(current<buf_size){
      int tid=buf[current++];
      int inst_sz=buf[current++];
      INSTANCES _new_instlist;
      while (inst_sz-->0){
        int ml_sz=buf[current++];
        std::vector<int> ml;
        while(ml_sz-->0)
          ml.push_back(buf[current++]);
        _new_instlist.push_back(INSTANCE(ml, buf[current],buf[current+1],buf[current+2],buf[current+3]));
	current=current+4;
      }
      _idlist.push_back(make_pair(tid,_new_instlist));
    } 
    input.clear();
    delete [] buf;
  } 

  
  /** Main VAT intersection function, returns constructed VATs;
      also populates the pattern_support arguments passed 
   */
  template<class PAT>
  static VAT** intersection(const VAT*const& v1, const VAT*const& v2, PAT_SUP** cand_sups, PAT**, bool) {
    VAT** cand_vats=new VAT*[2]; // max of 2 candidates possible
    bool do_child, do_cousin;
    do_cousin=(cand_sups[0]? 1:0);
    do_child=(cand_sups[1]? 1:0);
    
    if(!do_child && !do_cousin) {
      cerr<<"tree_vat: neither child nor cousin intersection is valid"<<endl;
      exit(0);
    }
    
    if(do_cousin)
      cand_vats[0]=new VAT;
    
    if(do_child)
      cand_vats[1]=new VAT;
    
    CONST_IT it_v1=v1->begin(), it_v2=v2->begin();
    
    while(it_v1!=v1->end() && it_v2!=v2->end()) {
      if(it_v1->first < it_v2->first) {
        it_v1++;
        continue;
      }
      
      if(it_v1->first > it_v2->first) {
        it_v2++;
        continue;
      }
      
      // execution reaches here only if both TIDs are equal
      CONST_INST_IT inst_it_v1;
      CONST_INST_IT inst_it_v2;
      
      for(inst_it_v1=it_v1->second.begin(); inst_it_v1!=it_v1->second.end(); inst_it_v1++) {
        for(inst_it_v2=it_v2->second.begin(); inst_it_v2!=it_v2->second.end(); inst_it_v2++) {
          // check if same tree's same inst are being compared
          if(v1==v2 && it_v1==it_v2 && inst_it_v1==inst_it_v2)
            continue;
          
          // cousin test
          if(do_cousin && cousin_test(*inst_it_v1, *inst_it_v2)) {
            INSTANCE new_inst(*inst_it_v2, inst_it_v1->lower());
            
            // append new pair if this tid did not exist in vat
            if(cand_vats[0]->empty() || cand_vats[0]->back().first != it_v1->first) {
              VAT_ST<INSTANCE, ALLOC<INSTANCE> > new_st;
              new_st.push_back(new_inst);
              cand_vats[0]->push_back(make_pair(it_v1->first, new_st));
            }
            else
              cand_vats[0]->back().second.push_back(new_inst);
          }//end if cousin_test
          
          // child test
          if(do_child && inst_it_v1->child_test(*inst_it_v2)) {
            INSTANCE new_inst(*inst_it_v2, inst_it_v1->lower());
            
            // append new pair if this tid did not exist in vat
            if(cand_vats[1]->empty() || cand_vats[1]->back().first != it_v1->first) {
              VAT_ST<INSTANCE, ALLOC<INSTANCE> > new_st;
              new_st.push_back(new_inst);
              cand_vats[1]->push_back(make_pair(it_v1->first, new_st));
            }
            else
              cand_vats[1]->back().second.push_back(new_inst);
            
          }//end if child_test
          
        }//end for inst_it_v2
        
      } //end for inst_it_v1  
      
      // advance to next tid
      it_v1++;
      it_v2++;
      
    } //end while 
    
    // copy the support into cand_sups
    if(do_cousin)
      cand_sups[0]->set_sup(make_pair(cand_vats[0]->size(), 0));
    
    if(do_child)
      cand_sups[1]->set_sup(make_pair(cand_vats[1]->size(), 0));
    
    return cand_vats;
    
  }//end intersect()
  
  friend ostream& operator<< <>(ostream&, const VAT*);
  
 private:
  IDLIST_T _idlist;
  
  /** Returns a (non-const) reference to last entry in vat */
  pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >& back() {
    if(empty()) {
      cerr<<"tree_vat: call to back in empty vat"<<endl;
      exit(0);
    }
    return _idlist.back();
  }
  
};//end class vat-embedded tree

template<typename PP, typename MP, template<typename> class VAT_ST>
ostream& operator<< (ostream& ostr, const vat<TREE_PROP, V_Fkk_MINE_PROP, VAT_ST>* tvat) {
  typedef vat<TREE_PROP, V_Fkk_MINE_PROP, VAT_ST> VAT;
  typename VAT::CONST_IT it=tvat->begin();
  
  while(it!=tvat->end()) {
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
 * \brief TREE VAT class for induced mining by partial specialization of the generic VAT class.
 *
 *        In this partial specialization, PP is fixed to directed, acyclic, indegree_lte_one 
 *        (Tree property), MP is fixed to Fk X Fk, vert_mine, induced (Vertical mining
 *        with Fk X Fk for induced patterns), ST is the VAT storage type.
 */
template<typename PP, typename MP, template <typename> class ALLOC, template<typename, typename> class VAT_ST>
class vat<TREE_PROP, V_Fkk_IND_MINE_PROP, ALLOC, VAT_ST>
{
  
 public:
  template<typename T1, typename A>
    class ST: public VAT_ST<T1, A> {};
  
  typedef pattern_support<V_Fkk_IND_MINE_PROP> PAT_SUP;
  
  typedef vat<TREE_PROP, V_Fkk_IND_MINE_PROP, ALLOC, VAT_ST> VAT;
  typedef tree_instance<std::vector, PP> INSTANCE;
  typedef VAT_ST<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >, ALLOC<pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > > > > IDLIST_T;
  typedef typename IDLIST_T::const_iterator CONST_IT;
  typedef typename IDLIST_T::iterator IT;
  typedef typename VAT_ST<INSTANCE, ALLOC<INSTANCE> >::const_iterator CONST_INST_IT;
  typedef VAT_ST<INSTANCE, ALLOC<INSTANCE> > INSTANCES;
  typedef typename VAT_ST<INSTANCE, ALLOC<INSTANCE> >::iterator INST_IT;
  typedef typename std::vector<int>::const_iterator CONST_STORE_IT;
  typedef typename std::vector<int>::iterator STORE_IT; 
  IT begin() {return _idlist.begin();}
  CONST_IT begin() const {return _idlist.begin();}
  IT end() {return _idlist.end();}
  CONST_IT end() const {return _idlist.end();}
  
  bool empty() const {return _idlist.empty();}
  int size() {return _idlist.size();}
  
  /** Appends new entry into vat */
  void push_back(const pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >& val) {
    _idlist.push_back(val);
  }
  
          
   unsigned long int byte_size() const{
      unsigned long int  b_size=0;
      CONST_IT it;
      CONST_INST_IT it_inst;
      //for each instance we will store 5 integer (1 for the vector size) and the vector
      for (it = begin(); it!=end();++it){
        b_size+=2*sizeof(int);
        for (it_inst=it->second.begin();it_inst!=it->second.end();++it_inst)
           b_size+=(it_inst->get_ml_size()+5)*sizeof(int);
      }
      return b_size;
  }

  //writing a VAT to a binary file
  void write_file(ostream & output) {
      //cout<<"Writing to the file"<<endl;
      //cout<< *this ;
      //cout<<endl;
      int ITSZ=sizeof(int);
      CONST_IT it;
      CONST_INST_IT it_inst;
      CONST_STORE_IT it_store;
      int tid,inst_sz,ml_sz,_lb,_ub,_depth,_type;
      for (it=begin();it!=end();++it){
        tid=it->first;
        inst_sz=it->second.size();
        output.write(reinterpret_cast<const char *>(&tid), ITSZ);
        output.write(reinterpret_cast<const char *>(&inst_sz), ITSZ);
        for (it_inst =it->second.begin(); it_inst!=it->second.end(); ++it_inst){
          ml_sz=it_inst->get_ml_size();
          output.write(reinterpret_cast<const char *>(&ml_sz), ITSZ);
          for (it_store=it_inst->get_ml_begin();it_store!=it_inst->get_ml_end();++it_store){
            output.write(reinterpret_cast<const char *>(&(*it_store)), ITSZ);
          }          
          _lb=it_inst->get_lb();  
          output.write(reinterpret_cast<const char *>(&_lb), ITSZ);
          _ub=it_inst->get_ub();  
          output.write(reinterpret_cast<const char *>(&_ub), ITSZ);
          _depth=it_inst->get_depth();  
          output.write(reinterpret_cast<const char *>(&_depth), ITSZ);
          _type=(int)it_inst->induced();  
          output.write(reinterpret_cast<const char *>(&_type), ITSZ);
        } //for it_inst
      }//it

  }
    
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
       INSTANCES _new_instlist;
       while (inst_sz-->0){
         int ml_sz=buf[current++];
         std::vector<int> ml;
         while(ml_sz-->0)
           ml.push_back(buf[current++]);        
         _new_instlist.push_back(INSTANCE(ml, buf[current],buf[current+1],buf[current+2],buf[current+3]));
	 current=current+4;
          
       }
       _idlist.push_back(make_pair(tid,_new_instlist));
     } 
     input.clear();
     delete []  buf;
  } 


  /** Main VAT intersection function, returns constructed VATs;
      also populates the pattern_support arguments passed 
   */
  template<class PAT>
    static VAT** intersection(const VAT*const& v1, const VAT*const& v2, PAT_SUP** cand_sups, PAT**, bool is_l2) {
    VAT** cand_vats=new VAT*[2]; // max of 2 candidates possible
    bool do_child, do_cousin;
    do_cousin=(cand_sups[0]? 1:0);
    do_child=(cand_sups[1]? 1:0);
    int last_etid_cousin=-1, last_itid_cousin=-1, last_etid_child=-1, last_itid_child=-1;
    int d;
    bool iflag;
    
    if(!do_child && !do_cousin) {
      cerr<<"tree_vat: neither child nor cousin intersection is valid"<<endl;
      exit(0);
    }
    
    if(do_cousin)
      cand_vats[0]=new VAT;
    
    if(do_child)
      cand_vats[1]=new VAT;
    
    CONST_IT it_v1=v1->begin(), it_v2=v2->begin();
    
    while(it_v1!=v1->end() && it_v2!=v2->end()) {
      if(it_v1->first < it_v2->first) {
        it_v1++;
        continue;
      }
      
      if(it_v1->first > it_v2->first) {
        it_v2++;
        continue;
      }
      
      // execution reaches here only if both TIDs are equal
      CONST_INST_IT inst_it_v1;
      CONST_INST_IT inst_it_v2;
      
      for(inst_it_v1=it_v1->second.begin(); inst_it_v1!=it_v1->second.end(); inst_it_v1++) {
        if(!inst_it_v1->induced())
          continue;
        for(inst_it_v2=it_v2->second.begin(); inst_it_v2!=it_v2->second.end(); inst_it_v2++) {
          // check if same tree's same inst are being compared
          if(v1==v2 && it_v1==it_v2 && inst_it_v1==inst_it_v2)
            continue;
          
          // cousin test
          if(do_cousin && cousin_test(*inst_it_v1, *inst_it_v2)) {
            bool ind=(inst_it_v1->induced() && inst_it_v2->induced());
            INSTANCE new_inst(*inst_it_v2, inst_it_v1->lower(), inst_it_v2->depth(), ind);
            
            if(ind) {
              if(last_itid_cousin!=it_v1->first) {
                cand_sups[0]->incr_isup();
                last_itid_cousin=it_v1->first;
              }
            }
            else {
              if(last_etid_cousin!=it_v1->first) {
                cand_sups[0]->incr_esup();
                last_etid_cousin=it_v1->first;
              }
            }
            
            // append new pair if this tid did not exist in vat
            if(cand_vats[0]->empty() || cand_vats[0]->back().first != it_v1->first) {
              VAT_ST<INSTANCE, ALLOC<INSTANCE> > new_st;
              new_st.push_back(new_inst);
              cand_vats[0]->push_back(make_pair(it_v1->first, new_st));
            }
            else
              cand_vats[0]->back().second.push_back(new_inst);
          } //end if cousin_test
          
          // child test
          if(do_child && inst_it_v1->child_test(*inst_it_v2)) {
            
            if(is_l2) {
              d=inst_it_v1->depth_diff(*inst_it_v2);
              iflag=(d==_diff);
            }
            else {
              d=inst_it_v2->depth();
              iflag=(inst_it_v1->induced() && (inst_it_v1->depth_diff(*inst_it_v2)==_diff));
            }
            
            if(iflag) {
              if(last_itid_child!=it_v1->first) {
                cand_sups[1]->incr_isup();
                last_itid_child=it_v1->first;
              }
            }
            else {
              if(last_etid_child!=it_v1->first) {
                cand_sups[1]->incr_esup();
                last_etid_child=it_v1->first;
              }
            }
            
            INSTANCE new_inst(*inst_it_v2, inst_it_v1->lower(), d, iflag);
            
            // append new pair if this tid did not exist in vat
            if(cand_vats[1]->empty() || cand_vats[1]->back().first != it_v1->first) {
              VAT_ST<INSTANCE, ALLOC<INSTANCE> > new_st;
              new_st.push_back(new_inst);
              cand_vats[1]->push_back(make_pair(it_v1->first, new_st));
            }
            else
              cand_vats[1]->back().second.push_back(new_inst);
            
          }//end if child_test
          
        }//end for inst_it_v2
        
      }//end for inst_it_v1  
      
      // advance to next tid
      it_v1++;
      it_v2++;
      
    }//end while 
    
    return cand_vats;
    
  } //end intersect()
  
  friend ostream& operator<< <>(ostream&, const VAT*);
  
 private:
  IDLIST_T _idlist;
  static const int _diff=1; /** The depth difference required between two induced candidates */
  
  /** 
    Returns a (non-const) reference to last entry in vat 
   */
  pair<int, VAT_ST<INSTANCE, ALLOC<INSTANCE> > >& back() {
    if(empty()) {
      cerr<<"tree_vat: call to back in empty vat"<<endl;
      exit(0);
    }
    return _idlist.back();
  }
  
};//end class vat-induced tree

#endif
