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
// tokenizer for trees

#ifndef _TREE_TOKENIZER_H
#define _TREE_TOKENIZER_H

#include "pattern.h"
#include "pat_fam.h"
#include "tree_vat.h"
#include "element_parser.h"
#include "generic_classes.h"
#include "tokenizer_utils.h"

/**
* \brief Tree tokenizer class by partial specialization of the generic tokenizer class.
 *
 * the template argument is instantiated with a pattern that has directed, acyclic, indegree_lte_one pattern property(tree),
 * MINING_PROPS type of mining property, ST type of pattern storage and CC type of
 * canocial code.
 */
template<typename PP, typename MP, typename TP, typename PAT_ST, template<typename, typename, typename, template <typename> class > class CC,
template <typename> class ALLOC >
class tokenizer<TREE_PATTERN, DMTL_TKNZ_PROP, ALLOC >
{
  
public:
  typedef vat<TREE_PROP, V_Fkk_MINE_PROP, ALLOC, std::vector> VAT;
  typedef tree_instance<std::vector, PP > TREE_INSTANCE;
  
  
  tokenizer(const int max=LINE_SZ): MAXLINE(max) {}
  
  
  /**
    * returns the TID of transaction read;
   *
   * parses one transaction from input database, and collects VATS in vat_hmap
   * return value is -1 on end of stream
   */
  template<class SM_T>
    int parse_next_trans(ifstream& infile, pat_fam<TREE_PATTERN>& freq_pats, storage_manager<TREE_PATTERN, VAT, ALLOC, SM_T>& vat_hmap) {
      char* line=new char[MAXLINE];
      char word[MAXLINE];
      char* startline=line;
      const int BK_TRK=-1; // backtrack symbol for trees
      
      int len;
      int count; //# of words parsed from line
      int tid=-1;
      int dfs_id=-1;
      int num_items=3; //# of objects on this transaction
      VAT* tvat;
      TREE_PATTERN* p;
      int depth; // maintains current depth in tree
      vector<pair<typename TREE_PATTERN::VERTEX_T, pair<int, bool> > > obj_data;
      // stores info for scope computation for generating level-1 vats
      // the first int in 2nd pair is current scope for pattern
      // bool is if the object is still in scope
      
      typedef element_parser<typename TREE_PATTERN::VERTEX_T> V_P;
      
      *line='\0';
      infile.getline(line, MAXLINE-1);
      len=strlen(line);
      if(!len || !line) {
        delete[] startline;
        return -1;
      }
      
      line[len++]='\0';
      count=0;
      depth=0;
      //obj_data.reserve(MAXLINE-1);
      
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
            //this is cid
            break;        
          case 3:
            //this is # of elements on line
            num_items=atoi(word);
            break;
            
          default:
            typename TREE_PATTERN::VERTEX_T v=el_prsr.parse_element(word);
            
            if(V_P::notEq(v, V_P::convert(BK_TRK))) {
              dfs_id++;
              depth++;
              for(unsigned int i=0; i<obj_data.size(); i++)
                if(obj_data[i].second.second)
                  obj_data[i].second.first++;
              obj_data.push_back(make_pair(v, make_pair(0, 1)));
            }
              else {
                depth--;
                for(unsigned int i=0; i<obj_data.size(); i++) {
                  obj_data[i].second.first--;
                  if(obj_data[i].second.second && (obj_data[i].second.first<0)) {
                    // add this pattern & scope to VAT
                    obj_data[i].second.second=0;
                    p=new TREE_PATTERN;
                    p->add_vertex(obj_data[i].first);
                    p->init_canonical_code(obj_data[i].first);
                    
                    if((tvat=vat_hmap.get_vat(p))) {
                      // vat found, check if this tid occurs
                      typename VAT::IT it=tvat->end()-1;
                      if(tvat->size() && it->first==tid)
                        it->second.push_back(TREE_INSTANCE(i, dfs_id, depth, 1));
                      else {
                        typename VAT::template ST<TREE_INSTANCE, ALLOC<TREE_INSTANCE> > vec_inst;
                        vec_inst.push_back(TREE_INSTANCE(i, dfs_id, depth, 1));
                        tvat->push_back(make_pair(tid, vec_inst));
                      }
                      delete p;
                    }
                    else {
                      // vat not found, create new one and insert it
                      tvat=new VAT;
                      typename VAT::template ST<TREE_INSTANCE, ALLOC<TREE_INSTANCE> > vec_inst;
                      vec_inst.push_back(TREE_INSTANCE(i, dfs_id, depth, 1));
                      tvat->push_back(make_pair(tid, vec_inst));
                      if(!vat_hmap.add_vat(p, tvat)) {
                        cerr<<"tokenizer.get_length_one(tree): add_vat failed"<<endl;
                        return -1;
                      }
                      freq_pats.push_back(p);
                    }
                    
                  }//end if obj_data..<0
                }//end for i
              }//end else v!=BK_TRK
              
        }//end switch
        
      }//end while(count<..)
      
      // special case: root of tree has not been added yet
      if(!obj_data[0].second.second) {
        cerr<<"tokenizer.get_length_one(tree): root of tree has invalid scope"<<endl;
        return -1;
      }
      
      // add its pattern & scope to VAT
      obj_data[0].second.second=0;
      p=new TREE_PATTERN;
      p->add_vertex(obj_data[0].first);
      p->init_canonical_code(obj_data[0].first);
      
      if((tvat=vat_hmap.get_vat(p))) {
        // vat found, check if this tid occurs
        typename VAT::IT it=tvat->end()-1;
        if(tvat->size() && it->first==tid)
          it->second.push_back(TREE_INSTANCE(0, dfs_id, 0, 1));
        else {
          typename VAT::template ST<TREE_INSTANCE, ALLOC<TREE_INSTANCE> > vec_inst;
          vec_inst.push_back(TREE_INSTANCE(0, dfs_id, 0, 1));
          tvat->push_back(make_pair(tid, vec_inst));
        }
        delete p;
      }
      else {
        // vat not found, create new one and insert it
        tvat=new VAT;
        typename VAT::template ST<TREE_INSTANCE, ALLOC<TREE_INSTANCE> > vec_inst;
        vec_inst.push_back(TREE_INSTANCE(0, dfs_id, 0, 1));
        tvat->push_back(make_pair(tid, vec_inst));
        if(!vat_hmap.add_vat(p, tvat)) {
          cerr<<"tokenizer.get_length_one(tree): add_vat failed"<<endl;
          return -1;
        }
        freq_pats.push_back(p);
      }
      
      //cout<<"tokenizer(trees).parse_next_trans exit"<<endl;      
      delete[] startline;
      return tid;
      
    }//end parse_next_trans()
  
private:
    const int MAXLINE;
  element_parser<typename TREE_PATTERN::VERTEX_T> el_prsr;
  
};//end class tokenizer for trees

#endif
