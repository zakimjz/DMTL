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
#ifndef _MSET_OPERATORS_H_
#define _MSET_OPERATORS_H_

#include<ostream>
using namespace std;

#include "typedefs.h"

template<class PP, class MP, class ST, template<class, typename, typename, template <typename> class> class CC, 
         template <typename> class ALLOC >
class pattern;

template<class PP, class MP, class PAT_ST, template<typename, typename, typename, template <typename> class> class CC,
         template <typename> class ALLOC >
ostream& operator<< (ostream& ostr, const MSET_PATTERN* p) {
  typename MSET_PATTERN::CONST_IT it;

  for(it=p->begin(); it!=p->end(); it++) {

    if(it == p->begin()) {
      ostr << (*it).v;
    } else {
      ostr << " " << (*it).v;
    }
  }

   ostr <<" -- "<<p->_pat_sup; 

   return ostr;
}

#include "pattern.h"


template<class PP, class MP, class PAT_ST, template<class, typename, typename, template <typename> class> class CC,
         template <typename> class ALLOC >
bool pattern<PP,MP,PAT_ST,CC, ALLOC>::operator< (const pattern<PP,MP,PAT_ST,CC, ALLOC> rhs) const {

  CONST_IT it=begin(), rhs_it=rhs.begin();

  if(size()!=rhs.size()) {
    cerr<<"Sequences are not of the same size."<<endl;
    exit(0);
  }

  while(it!=end() && rhs_it!=rhs.end()) {
    if((*it).v != (*rhs_it).v)
      return (*it) < (*rhs_it);

    it++;
    rhs_it++;
  }//end outer while
  
  //if execuion reaches this point then sequences are equal
  return false;
}

#endif
