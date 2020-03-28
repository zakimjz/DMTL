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
#ifndef _GENERIC_CLASSES_H_
#define _GENERIC_CLASSES_H_

#include <vector>

/**
 * \brief Class represents a generic canonical code.
 *
 * A canonical code is a unique (minimum or maximum) code for a pattern
 * from amongst all the codes for the pattern.
 * It takes 3 template arguments, the first is the pattern_properties,
 * the second is the vertex_tpe and the third is the edge_type
 */

template<class pattern_props, typename vertex_t, typename edge_t, template <typename> class ALLOC >
class canonical_code {};

/** 
 * \class vat iset_vat.h "src/itemset/iset_vat.h"
 * \brief Class represents a generic VAT(Vertical Attribute Table)
 *
 * It needs three template arguments;
 * PP: Pattern Properties, 
 * MP: Mining properties,
 * ST<typename P>: Storage_type, default is std::vector, P is the
 * underlying element type of ST storage type.
 */
template<class PP, class MP, template <typename> class ALLOC=std::allocator, template <typename P, typename A> class ST=std::vector >
  class vat {};

/**
 * \brief Class represent a generic tokenizer.
 *
 * This class defines key functions used by db_parser in order to parse input db
 */
template<typename, typename, template <typename> class A>
class tokenizer {};

/**
 * \brief A generic count support class.
 *
 */
template<class PP, class TRANS, class st, template<typename, typename, typename, template <typename> class > class cc, 
         template <typename> class alloc, class sm_type >
class count_support {
};

/**
 * \brief A generic Storage type class for back-end Storage.
 *
 * A generic storage type for back-end. If you want to add new storage type for back-end, extend this
 * class, as it is done for memory_storage class.
 */
 class storage_type { 
 };

/**
 * \brief Memory based Storage Manager class, extended from base storage type.
 *
 */
class memory_storage: public storage_type{
};

/**
 * \brief File based Storage Manager class, extended from base storage type.
 *
 */
class file_storage: public storage_type{
};

/**
 * \brief A Generic Storage Manager class.
 *
 * Storage_Manager class manages the VAT storage. It has 3 template arguments.
 * PAT: The stored pattern type, VAT: The VAT type, ST_TYPE: The storage type(memory, file etc.)
 */
template <typename PAT, typename VAT, template <typename> class ALLOC, typename ST_TYPE>
class storage_manager {
};


#endif
