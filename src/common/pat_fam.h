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
/** \file pat_fam.h - generic pattern family class */

#ifndef _PAT_FAM
#define _PAT_FAM

#include <iostream>
#include <vector>

// this container shall be eventually buffered //

template<typename P, template<typename T, typename = std::allocator<T> > class ST>
class pat_fam;
template<typename P, template<typename T, typename = std::allocator<T> > class ST>
std::ostream& operator<< (std::ostream&, const pat_fam<P, ST>&);

/** \class pat_fam
 * \brief Pattern Family class, denotes a collection of patterns. No constraint is imposed on the comprising patterns
 *
 * P is the pattern class, ST is the storage container type
 * e.g. P=pattern<itemset<int> >, ST=vector
 */
template<typename P, template<typename T, typename = std::allocator<T> > class ST=std::vector>
class pat_fam: public ST<P*>
{
  public:
  typedef typename ST<P*>::iterator IT; /**< iterator over the pattern family */
  typedef typename ST<P*>::const_iterator CONST_IT; /**< constant iterator over the pattern family */

  /** \fn friend ostream& operator<< <>(ostream& ostr, const pat_fam<P, ST>& pats)
   * \brief friend function defining the extraction operator
   * \param ostr Ostream object
   * \param pats pat_fam object
   */
  friend std::ostream& operator<< <>(std::ostream&, const pat_fam<P, ST>&);
};

template<class P>
std::ostream& operator<< (std::ostream& ostr, const pat_fam<P, std::vector>& pats)
{
  typename pat_fam<P, std::vector>::CONST_IT it;
  for(it=pats.begin(); it!=pats.end(); it++)
    ostr<<*it;
  return ostr;
}

#endif
