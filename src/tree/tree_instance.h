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
// struct for one instance i.e. occurrence of tree

#ifndef _TREE_INSTANCE_H
#define _TREE_INSTANCE_H

#include <vector>
#include <sstream>
using namespace std;

template<template<typename, typename> class MATCH_LABEL_T, class order>
class tree_instance;

template<template<typename, typename> class ST, typename T>
bool cousin_test(const tree_instance<ST, T>& i1, const tree_instance<ST, T>& i2);

template<template<typename, typename> class MATCH_LABEL_T, class order>
ostream& operator<< (ostream& ostr, const tree_instance<MATCH_LABEL_T, order>& inst);

/** Returns true if cousin test holds on the scope for unordered tree */
template<template<typename, typename> class ST, typename T>
bool cousin_test(const tree_instance<ST, proplist<ordered, T> >& i1, const tree_instance<ST, proplist<ordered, T> >& i2) {
  if(!i1.match_label(i2._ml))
    return false;
  
  if(i1.less_than(i2))
    return true;
  
  return false;
}//end cousin_test() for unordered trees

/**
 * \brief tree_instance class represents one tree_instance in a transaction
 * 
 * This class is part of tree VAT. A instance of this class represents exactly one tree
 * instance in a transaction. It takes two template arguments. MATCH_LABEL_T denotes the
 * data structure that is used to make up the tree match label. Usually a vector of int
 * is used. order denotes a class that determine whether the tree is ordered/unordered.
 */
//template<template<typename> class MATCH_LABEL_T, class order>
template<template<typename, typename> class MATCH_LABEL_T, class order>
class tree_instance
{
 public:
  typedef MATCH_LABEL_T<int, std::allocator<int> > ML;
  typedef tree_instance<MATCH_LABEL_T, order> TREE_INSTANCE;
  
  tree_instance() {} // default constructor
  
  tree_instance(const int& lx, const int& ux, const int& d=0, bool ind=0): _lb(lx), _ub(ux), _depth(d), _is_induced(ind) {}
  tree_instance(ML& ml, const int& lx, const int& ux, const int& d=0, bool ind=0): _ml(ml), _lb(lx), _ub(ux), _depth(d), _is_induced(ind) {}
  tree_instance(const TREE_INSTANCE& i2, const int& lx, const int& d=0, bool ind=0): _ml(i2._ml), _lb(i2._lb), _ub(i2._ub), _depth(d), _is_induced(ind)
    { _ml.push_back(lx);}//end constructor
  
  const int& lower() const {return _lb;}
  const int& upper() const {return _ub;}

  /** Returns true if scope of *this is strictly less than that of i2 */
  bool less_than(const TREE_INSTANCE& i2) const {
    if (_ub<i2._lb)
      return true;
    
    return false;
  }//end less_than()

  /** Returns true if scope of *this contains that of i2 */
  bool contains(const TREE_INSTANCE& i2) const {
    if(_lb<i2._lb && _ub>=i2._ub)
      return true;
    
    return false;
  }//end contains()
  
  /** Returns true if this->ml is equal to ml2 */
  bool match_label(const ML& ml2) const {
    if(_ml.size()!=ml2.size())
      return false;
    
    typename ML::const_iterator it1=_ml.begin(), it2=ml2.begin();
    
    while(it1!=_ml.end() && it2!=ml2.end()) {
      if(*it1 != *it2)
        return false;
      it1++;
      it2++;
    }
    
    return true;
  }//end match_label()
  
  friend bool cousin_test<>(const TREE_INSTANCE&, const TREE_INSTANCE&);
  
  friend ostream& operator<< <>(ostream&, const TREE_INSTANCE&);
  
  /** Returns true if child test holds on *this and i2 */
  bool child_test(const TREE_INSTANCE& i2) const {
    if(!match_label(i2._ml))
      return false;
    
    if(contains(i2))
      return true;
    
    return false;
  }//end child_test()
  
  /** Returns true if depth difference trees makes them induced */
  int depth_diff(const TREE_INSTANCE& i2) const
    { return(i2._depth - _depth);}

  bool induced() const { return _is_induced;}

  const int& depth() const { return _depth;}
  int get_lb()  const{ return _lb;}
  int get_ub() const { return _ub;}
  int get_depth() const {return _depth;}
  
  typename ML::const_iterator  get_ml_begin() const {return _ml.begin();}
  typename ML::const_iterator  get_ml_end() const {return _ml.end();}
   
  int get_ml_size() const {return _ml.size();}
  private:
  ML _ml;
  int _lb;
  int _ub;
  int _depth;
  bool _is_induced;

};//end class tree_instance

/** Returns true if cousin test holds on the scope for ordered tree */
template<template<typename, typename> class ST, typename T>
bool cousin_test(const tree_instance<ST, T>& i1, const tree_instance<ST, T>& i2) {
  if(!i1.match_label(i2._ml))
    return false;
  
  if(i1.less_than(i2) || i2.less_than(i1))
    return true;
  
  return false;
}//end cousin_test() for ordered trees

template<template<typename, typename> class MATCH_LABEL_T, class order>
ostream& operator<< (ostream& ostr, const tree_instance<MATCH_LABEL_T, order>& inst) {
  ostr<<"[("<<inst._lb<<", "<<inst._ub<<") ";
  typename MATCH_LABEL_T<int, std::allocator<int> >::const_iterator it=inst._ml.begin();
  while(it!=inst._ml.end())
    ostr<<*it++<<" ";
  ostr<<inst._depth<<" "<<inst._is_induced;
  ostr<<"]";
  return ostr;
}//end friend operator<<

#endif
