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
#ifndef _PATTERN_SUPPORT_H_
#define _PATTERN_SUPPORT_H_

#include <iostream>
#include "properties.h"

// frequency is an indicator of whether the pattern is frequent 
// for its definition

// validity of a pattern is an indicator of whether it should be stored for 
// sake of completion of candidate generation. All frequent patterns shall be 
// valid but vice versa does not hold true for Fk_Fk induced/unordered mining

template<class MP>
class pattern_support;

template<class MP>
std::ostream& operator<< (std::ostream&, const pattern_support<MP>&);


/**
 * \brief Generic Pattern Support Class that takes care of support counting of a pattern.
 *
 * Templated with Mining property, since support counting depends on different mining properties.
 */
template<class MP>
class pattern_support
{

public:
  //constructor
  inline
  pattern_support(const int& s=0, bool v=0): _sup(s), _valid(v) {}

  inline
  bool is_valid(const int& minsup) const {return _sup>=minsup;}
  
  inline
  bool is_freq(const int& ms) const {return _sup>=ms;}
  
  // parimi: commented out b'cos it is not clear what it should do
  int get_sup() const { return _sup;}

  void set_vals(const pattern_support<MP>* s) {_sup=s->_sup; _valid=s->_valid;}

  void set_sup(const std::pair<int,int>& s) { 
     // Ignore the second element in the pair - used for induced mining.
     _sup=s.first;
  }

  // friend extraction
  friend std::ostream& operator<< <>(std::ostream&, const pattern_support<MP>&);

  // NOTE: validity and frequency are synonymous for generic patterns

 private:
  int _sup;
  bool _valid;

};//end class pattern_support


template<typename MP>
std::ostream& operator<< (std::ostream& ostr, const pattern_support<MP>& ps) {
  ostr<<"Support: "<<ps._sup<<std::endl;
  return ostr;
}

template<class T>
std::ostream& operator<< (std::ostream& ostr, const pattern_support<proplist<Fk_Fk, 
              proplist<vert_mine, proplist<induced, T > > > >& ps) {

  //ostr<<"Support: "<<ps._isup << " (" << ps._esup << " )" << std::endl;
  ostr<<"Support: "<<ps._isup << std::endl;
  return ostr;
}

/**
 * \brief Pattern Support class specialized for Fk X Fk join type and induced mining.
 *
 */
template<class T>
class pattern_support<proplist<Fk_Fk, proplist<vert_mine, proplist<induced, T > > > >
{

public:
  //constructor
  pattern_support(const int& is=0, const int& s=0, bool v=0): _isup(is), _esup(s) {}

  bool is_valid(const int& minsup) const {
    return (_isup>=minsup || (_esup>=minsup));
  }

  bool is_freq(const int& minsup) const { 
     return _isup >= minsup;
  }

  int get_sup() const { return _isup;}

  void incr_isup() { _isup++;}

  void incr_esup() { _esup++;}

  void set_vals(const pattern_support<proplist<Fk_Fk, proplist<vert_mine, proplist<induced, T> > > >* const& s) {
    _esup=s->_esup; _isup=s->_isup;
  }

  void set_sup(const std::pair<int, int>& s) {
    _isup=s.first;
    _esup = s.second;
  }

  // friend extraction
  friend std::ostream& operator<< <>(std::ostream&, const pattern_support<proplist<Fk_Fk, proplist<vert_mine, proplist<induced, T > > > >&);

 private:
  int _isup; //induced support
  int _esup; //ONLY embedded support
}; //end pattern_support for induced

#endif
