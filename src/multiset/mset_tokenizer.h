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
#ifndef _MSET_TOKENIZER_H_
#define _MSET_TOKENIZER_H_

#include <fstream>

#include "mset_can_code.h"
#include "adj_list.h"
#include "generic_classes.h"
#include "tokenizer_utils.h"
#include "typedefs.h"

#include "element_parser.h"
#include "pattern.h"
#include "pat_fam.h"

/* NOTE: the parsing scheme reads atmost the first MAXLINE chars of a line
   this can perhaps be improved towards a better one */

/**
 * \brief Itemset tokenizer class by partial specialization of the generic tokenizer class.
 *
 * the template argument is instantiated with a pattern that has no_edge pattern property,
 * Fk X Fk and vert_mine mining property, PAT_ST type of pattern storage and CC type of
 * canocial code.
 */
template<class PP, typename MP, typename TP, typename PAT_ST, template<typename, typename, typename, template <typename> class> class CC, 
         template <typename> class ALLOC >
class tokenizer<MSET_PATTERN, DMTL_TKNZ_PROP, ALLOC >
{

 public: 
  typedef pattern_support<V_Fkk_MINE_PROP> PAT_SUP;
  typedef vat<MSET_PROP, V_Fkk_MINE_PROP, ALLOC, std::vector > VAT;
  typedef typename MSET_PATTERN::VERTEX_T V_T;
  typedef typename MSET_PATTERN::EDGE_T E_T;

  typedef int VAT_T; /**< VAT id-type for itemset */
  
  tokenizer(int max=LINE_SZ): MAXLINE(max) {} /**< default constructor */
  


  /**
   * Returns the TID of transaction read;
   * parses one transaction from input database, and collects VATS in vat_hmap
   * return value is -1 on end of stream
   */
  template<class SM_T>
  int parse_next_trans(std::ifstream& infile, pat_fam<MSET_PATTERN>& freq_pats, storage_manager<MSET_PATTERN, VAT, ALLOC, SM_T>& vat_hmap) {

    char* line=new char[MAXLINE];
    char word[MAXLINE];
    char* startline=line;
    
    int len;
    int count; //# of words parsed from line
    int tid=-1, ts;
    int num_items=3; //# of itemsets on this transaction
    
    VAT* ivat;

    *line='\0';
    infile.getline(line, MAXLINE-1);
    len=strlen(line);
    if(!len || !line) {
      delete[] startline;
      return -1;
    }

    line[len++]='\0';
    count=0;
    while(count<num_items+3 && line<(startline+len)) {

      if(!(line=parse_word()(line, word))) {
        //parse_word() failed
        delete[] startline;
        return -1;
      }
      count++;

      switch(count) {
        case 1:
          //this is tid
          tid=atoi(word); break;
      
        case 2: 
          //this is timestamp
          ts=atoi(word); break;

        case 3:
          //this is # of elements on line
          num_items=atoi(word);
          break;
 
        default:
          //this is an element, insert/append to its VAT
          MSET_PATTERN* p = new MSET_PATTERN();
          // Add vertex to the empty graph.
          V_T v = el_prsr.parse_element(word);
          p->add_vertex(v);
          p->init_canonical_code(v);

            //if p contains a vat in vat_hmap, append tid to the entry
            //else create a new vat and insert it into vat_hmap,
            //and add p to freq_pats
            ivat=vat_hmap.get_vat(p);
            if(vat_hmap.find(p)) {
            //vat found

            if(tid == (*ivat)[ivat->size()-1].first) {
              (*ivat)[ivat->size()-1].second++;
            } else {
              ivat->push_back(make_pair(tid, 1));
            }
            delete p;
          }
          else {
            //create a new vat & insert it
            ivat=new VAT();
            ivat->push_back(make_pair(tid, 1));

            if(!vat_hmap.add_vat(p, ivat)) {
              cerr<<"tokenizer.get_length_one: add_vat failed"<<endl;
              return -1;
            }

            freq_pats.push_back(p);
          } //end else

      } //end switch

    } //end while(count<..)
     
    delete[] startline;

    return tid;
  } //end parse_next_trans()



 private:
    int MAXLINE; /**< max length of line to be parsed */
    element_parser<V_T> el_prsr; /**< parses an element of desired type */

}; //end class tokenizer<itemset>

#endif
