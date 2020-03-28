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
#ifndef _GRAPH_OPERATORS_H_
#define _GRAPH_OPERATORS_H_


template<class PP, class MP, class ST, template<class, typename, typename, template <typename> class > class CC, 
         template <typename> class ALLOC >
class pattern;

class undirected;

template<class prop, class next_property>
class proplist;


template<class PP, class MP, class PAT_ST, template<class, typename, typename, template <typename> class > class CC,
         template <typename> class ALLOC >
ostream& operator<< (ostream& ostr, const GRAPH_PATTERN* p) {
  ostr<<"Canonical code: \n"<<p->_canonical_code;
  ostr<<p->_pat_sup;
  return ostr;
}

#endif
