/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:
 *    Added tokenizer properties -- Zaki, 5/8/06
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
#ifndef _PROPERTIES_H_
#define _PROPERTIES_H_


class property {};                           /**< Root property class */

class pat_property {};                       /**< Root Pattern property class */
class directed: public property {};          /**< Dummy property class, directed */
class undirected: public property {};        /**< Dummy property class, undirected */
class no_edges: public property {};          /**< Dummy property class, no_edges */
class uniq_label: public property {};        /**< Dummy property class, uniq_label */
class cyclic: public property {};            /**< Dummy property class, cyclic */
class acyclic: public property {};           /**< Dummy property class, acyclic */
class indegree_lte_one: public property {};  /**< Dummy property class, indegree_lte_one */
class outdegree_lte_one: public property {}; /**< Dummy property class, outdegree_lte_one */
class ordered: public property {};           /**< Dummy property class, ordered */
class unordered: public property {};         /**< Dummy property class, unordered */
class null_prop: public property {};         /**< Null property class, used to terminate the proplist */

// end pattern property definitions //


// mining property class definitions //
class mining_prop: public property {};       /**< Root Mining property class */
class embedded: public mining_prop {};       /**< Dummy mining property class for embedded mining */
class induced: public mining_prop {};        /**< Dummy mining property class for induced mining */
class Fk_Fk: public mining_prop {};          /**< Dummy mining property class for Fk_Fk join */
class Fk_F1: public mining_prop {};          /**< Dummy mining property class for Fk X F1 join */
class vert_mine: public mining_prop {};      /**< Dummy mining property class for vertical mining */
class horiz_mine: public mining_prop {};     /**< Dummy mining property class for horizontal mining */

// end mining pros //

//tokenizer property class definitions//
class tokenizer_prop: public property {}; /**< Root Tokenizer property class */
class dmtl_format: public tokenizer_prop {}; /**< Dummy tokenizer class for default DMTL formatted input files */
class fasta_format: public tokenizer_prop {}; /**< Dummy tokenizer class for FASTA formatted sequence files */


/**
* \brief Property list Class to list the properties for generic pattern mining algorithm
 *
 * Property list class takes two template arguments, the first one is a property class and the second one is 
 * the next property class and so on. To terminate the list, null_prop class is used.
 */
template<class prop, class next_property=null_prop>
class proplist
{
public:
  typedef prop FIRST;
  typedef next_property SECOND;
};

#endif
