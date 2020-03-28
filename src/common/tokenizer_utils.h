/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:
 *            Added LINE_SZ -- zaki 5/15/06
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
#ifndef _TOKENIZER_UTILS_H_
#define _TOKENIZER_UTILS_H_

#define LINE_SZ 10000 //number of characters per line in database file

struct parse_word 
{

  /**
    * Reads in a delim-delimited set of characsters from line into word, 
    * and returns pointer to next char that needs to be read
    */
  char* operator() (char* line, char* word, char delim=' ') const {

    while(*line && *line!=delim) {
      *word=*line;
      word++;
      line++;
    }
    *word='\0';

    if(*line==delim) {
      line++;
    }

    return line;
  } //end parse_word()
};

#endif
