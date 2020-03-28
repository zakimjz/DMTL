/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications: 
 *        added separate induced and embedded sequence instances -- zaki, 5/11/06
 *        fixed bug with induced and embbeded sequence mining -- zaki, 5/11/06
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
#ifndef _SEQ_INSTANCE_H_
#define _SEQ_INSTANCE_H_

#include "typedefs.h"
/**
* \brief generic sequence instance class.
 *
 */
template <class MP>
class seq_instance{};

template <class MP>
ostream& operator<< (ostream& ostr, const seq_instance<V_Fkk_EMB_MINE_PROP>& rhs);

/**
 * \brief Class represents precisely one instance i.e. occurrence of a sequence in the dataset.
 *        This class is for embedded sequence mining.
 */
template <class MP>
class seq_instance<V_Fkk_EMB_MINE_PROP>
{
  
public:
  
  seq_instance(): ts(-1) {} //default defunct constructor
  
  seq_instance(const int timestamp, const int position): ts(timestamp) {} /**< parameterized constructor */

  seq_instance(const int timestamp): ts(timestamp) {} /**< parameterized constructor */

  int get_ts() const {return ts;}
  
  /** \fn static void seq_join_seq(const ST<seq_instance>& i1, const ST<seq_instance>& i2, ST<seq_instance>& evnt_inst, ST<seq_instance>& seqi_inst, ST<seq_instance>& seqj_inst, bool do_evnt, bool& indsup_evnt, bool& indsup_seqi, bool& indsup_seqj)
  * performs intersection of instance collections of two seq atoms
  * \param evnt_inst instance collection of event occurrences
  * \param seqi_inst instance collection of first seq occurrences
  * \param seqj_inst instance collection of second seq occurrences
  * \param indsup_evnt is true if induced event occurrences found
  * \param indsup_seqi true if induced occurrences found for first seq
  * \param indsup_seqj true if induced occurrences found for second seq
  */
  template<template<typename, typename > class ST, template <typename> class ALLOC>
  static void seq_join_seq(const ST<seq_instance, ALLOC<seq_instance> >& i1,             const ST<seq_instance, ALLOC<seq_instance> >& i2,
               ST<seq_instance, ALLOC<seq_instance> >& seqi_inst, 
               ST<seq_instance, ALLOC<seq_instance> >& seqj_inst, 
               bool& seqi_ind, bool& seqj_ind, 
               bool& seqi_emb, bool& seqj_emb, 
			   const bool self_join_only) 
  {
    typedef ST<seq_instance, ALLOC<seq_instance> > INSTANCES;
    typedef typename INSTANCES::const_iterator CONST_INST_IT;/**< constant iterator over instances in a tid */
      CONST_INST_IT it1, it2, it3;

      // Results in seq atom with seqi prefix
      it1 = i1.begin();
      it2 = i2.end()-1;
      if (it1->ts < it2->ts){
	for(it1=i1.begin(), it2=i2.begin(); it1 != i1.end() && it2 != i2.end();) {
	  //search for it2 after it1
	  if (it1->ts < it2->ts){
	    //copy all remaining i2 values since they must be < it1
	    for (it3 = it2; it3 != i2.end(); ++it3){
	      seqi_inst.push_back(seq_instance(it3->ts));
	    }
	    seqi_emb = true;
	    break; //we are done with this sequence.
	  }
	  else ++it2;
	}
      }
      
      
      // Only if the join is going to result in 2 different vats.
      if(!self_join_only) {
        it1 = i1.end()-1;
        it2 = i2.begin();
        if (it2->ts < it1->ts){
          for(it1=i1.begin(), it2=i2.begin(); it1 != i1.end() && it2 != i2.end();) {
            //search for it1 after it2
            if (it2->ts < it1->ts){
              //copy all remaining i1 values since they must be < it2
              for (it3 = it1; it3 != i1.end(); ++it3){
                seqj_inst.push_back(seq_instance(it3->ts));
              }
              seqj_emb = true;
              break; //we are done with this sequence.
            }
            else ++it1;        
          }
        }
      }
      
  }//end seq_join_seq()    

  friend ostream& operator<< <>(ostream& ostr, const seq_instance& rhs); /**< Friend extraction operator for output */
private:
  int ts; /**< timestamp of the occurrence */
}; //end of seq_instance

template<class MP>
ostream& operator<< (ostream& ostr, const seq_instance<V_Fkk_EMB_MINE_PROP>& rhs){
  ostr << "[ " << rhs.ts << " ] ";
  return ostr;
}

template <class MP>
ostream& operator<< (ostream& ostr, const seq_instance<V_Fkk_IND_MINE_PROP>& rhs);

/**
 * \brief Class represents precisely one instance i.e. occurrence of a sequence in the dataset.
 *        This class is for induced sequence mining.
 */
template <class MP>
class seq_instance<V_Fkk_IND_MINE_PROP>
{
  
public:
  
  seq_instance(): pos(-1), induced(false) {} //default defunct constructor
  
  seq_instance(const int timestamp, const int position): pos(position), induced(true) {} /**< parameterized constructor */
  
  seq_instance(const int position, const bool ind): pos(position), induced(ind) {} /**< parameterized constructor */

  
  /** \fn static void seq_join_seq(const ST<seq_instance>& i1, const ST<seq_instance>& i2, ST<seq_instance>& evnt_inst, ST<seq_instance>& seqi_inst, ST<seq_instance>& seqj_inst, bool do_evnt, bool& indsup_evnt, bool& indsup_seqi, bool& indsup_seqj)
  * performs intersection of instance collections of two seq atoms
  * \param evnt_inst instance collection of event occurrences
  * \param seqi_inst instance collection of first seq occurrences
  * \param seqj_inst instance collection of second seq occurrences
  * \param indsup_evnt is true if induced event occurrences found
  * \param indsup_seqi true if induced occurrences found for first seq
  * \param indsup_seqj true if induced occurrences found for second seq
  */
  template<template<typename, typename > class ST, template <typename> class ALLOC>
  static void seq_join_seq(const ST<seq_instance, ALLOC<seq_instance> >& i1, const ST<seq_instance, ALLOC<seq_instance> >& i2,
               ST<seq_instance, ALLOC<seq_instance> >& seqi_inst, 
               ST<seq_instance, ALLOC<seq_instance> >& seqj_inst, 
               bool& seqi_ind, bool& seqj_ind, 
               bool& seqi_emb, bool& seqj_emb, 
			   const bool self_join_only, const bool skip0=false) 
  {
    typedef ST<seq_instance, ALLOC<seq_instance> > INSTANCES;
    typedef typename INSTANCES::const_iterator CONST_INST_IT;/**< constant iterator over instances in a tid */
      CONST_INST_IT it1, it2;
      
      if (!skip0){
	bool seen_valid_occurrence = false;
	// Results in seq atom with seqi prefix
	for(it1=i1.begin(), it2=i2.begin(); it1 != i1.end() && it2 != i2.end();) {
	  
	  if (it1->pos < it2->pos){
	    seen_valid_occurrence = true;
	    if (it1->is_induced() && it2->pos - it1->pos == 1){
	      seqi_inst.push_back(seq_instance(it2->pos, true));
	      seqi_ind = true;    
	      ++it2;
	    }
	    ++it1;
	  }
	  else{
	    if (seen_valid_occurrence){
	      seqi_inst.push_back(seq_instance(it2->pos, false));    
	      seqi_emb=true;
	    }
	    ++it2;
	  }
	}
	//copy over any remaining occurrences in it2
	for (; it2 != i2.end(); ++it2){
	  if (seen_valid_occurrence){
	    seqi_inst.push_back(seq_instance(it2->pos, false));    
            seqi_emb=true;
	  }
	}
      }
      
      // Only if the join is going to result in 2 different vats.
      if(!self_join_only) {
        bool seen_valid_occurrence = false;
        // Results in seq atom with seqj prefix
        for(it1=i1.begin(), it2=i2.begin(); it1 != i1.end() && it2 != i2.end();) {
          
          if (it2->pos < it1->pos){
            seen_valid_occurrence = true;
            if (it2->is_induced() && it1->pos - it2->pos == 1){
              seqj_inst.push_back(seq_instance(it1->pos, true));
              seqj_ind = true;    
              ++it1;
            }
            ++it2;
          }
          else{
            if (seen_valid_occurrence){
              seqj_inst.push_back(seq_instance(it1->pos, false));    
              seqj_emb=true;
            }
            ++it1;
          }
        }
        //copy over any remaining occurrences in it1
        for (; it1 != i1.end(); ++it1){
          if (seen_valid_occurrence){
            seqj_inst.push_back(seq_instance(it1->pos, false));    
            seqj_emb=true;
          }
        }        
      }
      
  }//end seq_join_seq()    
  
  friend ostream& operator<< <>(ostream& ostr, const seq_instance& rhs); /**< Friend extraction operator for output */
  int get_pos() const {return pos;}
  
  bool get_induced() const {
    return induced;
  } // Returns true if this instance is an induced occurrence
  
private:
  int pos; /**< position of the occurrence in the sequence */
  bool induced; /**< indicates if the occurrence is boolean */
  bool is_induced() const{
    return induced;
  }
}; //end of seq_instance

template <class MP>
ostream& operator<< (ostream& ostr, const seq_instance<V_Fkk_IND_MINE_PROP>& rhs){
  ostr << "[ " << rhs.pos << ", " << rhs.induced << " ] ";
  return ostr;
}


#endif

