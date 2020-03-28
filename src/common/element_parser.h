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
/** \file element_parser.h - class to parse a set of characters into the 
 * specialized element type */

#ifndef _ELEMENT_PARSER
#define _ELEMENT_PARSER
#include<sstream>
#include<stdlib.h>
#include "helper_funs.h"

/** \brief Class represents a generic element_parser
 *
 * This is a dummy generic definition
 * It has to be partially specialized for each new element-type require
 */

template<typename P>
class element_parser {};

/** 
 * \brief Element parser class for parsing an integer, an specialization of element_parser class.
 */

template<>
class element_parser<int>
{

 public:

  typedef int OBJ_T; /**< element type */
  typedef int HASH_TYPE; // Input type for the hash function.
  typedef eqint COMP_FUNC; // Comparison function.

  /** \fn OBJ_T parse_element(char* word)
   * \brief parse characters in word to type OBJ_T
   * \param word input set of characters
   * return value is parsed element
   */
  static inline OBJ_T parse_element(char* word) { 
    return atoi(word);
  }

  static inline OBJ_T parse_element(const char* word) { 
    return atoi(word);
  }

  static const OBJ_T& convert(const int& i) {
 
    return i;
  }

  static bool notEq(const int& i1, const int& i2) {
    return !(i1 == i2);
  }

  static const HASH_TYPE& conv_hash_type(const OBJ_T& s) {
    return s;
  }


};//end clas element_parser<int>

/** 
 * \brief Element parser class for parsing a string, an specialization of element_parser class.
 */
template<>
class element_parser<std::string>
{

 public:

  typedef std::string OBJ_T; /**< element type */
  typedef const char* HASH_TYPE; // Input type for the hash function.
  typedef eqstr COMP_FUNC; // Comparison function.

  /** \fn OBJ_T parse_element(char* word)
   * \brief parse characters in word to type OBJ_T
   * \param word input set of characters
   * return value is parsed element
   */
  static inline OBJ_T parse_element(const char* word) { 
    return std::string(word);
  }

  static OBJ_T convert(const char* s) {
    return parse_element(s);
  }

  static OBJ_T convert(const int i) {
    std::ostringstream t_ss;
    t_ss << i;
 
    return t_ss.str();
  }

  static bool notEq(const std::string s1, const std::string s2) {
    return !(s1 == s2);
  }

  static HASH_TYPE conv_hash_type(const OBJ_T& s) {
    return s.c_str();
  }

};//end clas element_parser<std::string>

#endif
