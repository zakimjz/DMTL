/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:
 *    Added tokenizer properties & FASTA tokenizer -- Zaki, 5/8/06
 *      Added sequence position for induced occurrences -- zaki, 5/11/06
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
#ifndef _TOKENIZER
#define _TOKENIZER

#include "seq_can_code.h"
#include "adj_list.h"

#include "generic_classes.h"
#include "tokenizer_utils.h"
#include "element_parser.h"
#include "seq_instance.h"
#include "typedefs.h"

/* NOTE: the parsing scheme reads atmost the first MAXLINE chars of a line
this can perhaps be improved towards a better one */

/**
* \brief Sequence tokenizer class by partial specialization of the generic tokenizer class.
 *
 * the template argument is instantiated with a pattern that has directed, acyclic, indegree_lte_one, outdegree_lte_one pattern property(tree),
 * MINING_PROPS type of mining property, ST type of pattern storage and CC type of
 * canocial code.
 */
template<class PP, typename MP, typename TP, typename PAT_ST, template<typename, typename, typename, template <typename> class > class CC, 
template <typename> class ALLOC >
class tokenizer<SEQ_PATTERN, DMTL_TKNZ_PROP, ALLOC >
{
  
public:
  typedef pattern_support<V_Fkk_MINE_PROP> PAT_SUP;
  typedef vat<SEQ_PROP, V_Fkk_MINE_PROP, ALLOC, std::vector > VAT;
  typedef seq_instance <V_Fkk_MINE_PROP> INSTANCE;
  typedef typename SEQ_PATTERN::VERTEX_T V_T;
  typedef typename SEQ_PATTERN::EDGE_T E_T;
  
  
  tokenizer(int max=LINE_SZ): MAXLINE(max) {} /**< default constructor */
  
  
  /**
    * returns the TID of transaction read;
   * parses one transaction from input database, and collects VATS in vat_hmap
   * return value is -1 on end of stream
   */
  template<class SM_T>
    int parse_next_trans(ifstream& infile, pat_fam<SEQ_PATTERN>& freq_pats, storage_manager<SEQ_PATTERN, VAT, ALLOC, SM_T>& vat_hmap ) {
      
      char* line=new char[MAXLINE];
      char word[MAXLINE];
      char* startline=line;
      
      int len;
      int count; //# of words parsed from line
      int tid=-1, ts=0;
      int num_items=3; //# of itemsets on this transaction
      int pos; //stores starting position of input stream's get pointer
      int sequence_pos = 0; // position in the sequence
      
      VAT* svat;
      
      do {
        pos=infile.tellg();
        line=startline;
        *line='\0';
        infile.getline(line, MAXLINE-1);
        len=strlen(line);
        if(!len || !line) {
          delete[] startline;
          return tid;
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
              //this is tid/oid
              if(tid!=-1 && tid!=atoi(word)) {
                // this line is next transaction
                infile.seekg(pos);
                delete[] startline;
                return tid;
              }
              tid=atoi(word); 
              break;              
            case 2:               
              ts=atoi(word); //this is timestamp
              sequence_pos++; //this is the position in the seq
              break;        
            case 3:
              //this is # of elements on line
              num_items=atoi(word);
              break;
              
            default:
              //this is an element, insert/append to its VAT
              SEQ_PATTERN* p = new SEQ_PATTERN();
	      //cout << "WORD " << tid << " " << ts << " " 
	      //   << num_items << " " << word << endl;
              V_T v =el_prsr.parse_element(word);
              
              // Add vertex and update the canonical code.
              p->add_vertex(v);
              p->init_canonical_code(v);
              
              //if p contains a vat in vat_hmap, append tid/ts to the entry
              //else create a new vat and insert it into vat_hmap,
              //and add p to freq_pats
              svat=vat_hmap.get_vat(p);
              //if(vat_hmap.find(p))
              if(svat != NULL) {
                //vat found, check if this tid exists in it
                
                typename VAT::IT vit=svat->end()-1;
                if(vit->first!=tid)
                  vit=svat->end();
                
                
                if(vit!=svat->end())
                  //tid found
                  vit->second.push_back(INSTANCE(ts,sequence_pos));
                else {
                  //tid not found
                  typename VAT::INSTANCES new_tidlist;
                  new_tidlist.push_back(INSTANCE(ts,sequence_pos));
                  svat->push_back(make_pair(tid, new_tidlist));
                }
                
                delete p;
                
              }//end if(vat_hmap.find())
                else {
                  //create a new vat & insert it
                  svat=new VAT();
                  typename VAT::INSTANCES new_tidlist;
                  new_tidlist.push_back(INSTANCE(ts,sequence_pos));
                  svat->push_back(make_pair(tid, new_tidlist));
                  if(!vat_hmap.add_vat(p, svat)) {
                    cerr<<"tokenizer.get_length_one: add_vat failed"<<endl;
                    return -1;
                  }
                  freq_pats.push_back(p);
                }//end else
                
          }//end switch
          
        }//end while
        
      }while(true);
      
      return -1;
    }//end parse_next_trans()
  
private:
    int MAXLINE; /**< max length of line to be parsed */
  element_parser<V_T> el_prsr; /**< parses an element of desired type */
  
}; //end class seq_tokenizer

template<class PP, typename MP, typename TP, typename PAT_ST, template<typename, typename, typename, template <typename> class > class CC, 
template <typename> class ALLOC >
class tokenizer<SEQ_PATTERN, FASTA_TKNZ_PROP, ALLOC >
{
  
public:
  typedef pattern_support<V_Fkk_MINE_PROP> PAT_SUP;
  typedef vat<SEQ_PROP, V_Fkk_MINE_PROP, ALLOC, std::vector > VAT;
  typedef seq_instance <V_Fkk_MINE_PROP> INSTANCE;
  typedef typename SEQ_PATTERN::VERTEX_T V_T;
  typedef typename SEQ_PATTERN::EDGE_T E_T;
  
  
  tokenizer(int max=LINE_SZ): MAXLINE(max) {} /**< default constructor */
  
  
  /**
    * returns the TID of transaction read;
   * parses one transaction from input database, and collects VATS in vat_hmap
   * return value is -1 on end of stream
   */
  template<class SM_T>
  int parse_next_trans(ifstream& infile, pat_fam<SEQ_PATTERN>& freq_pats, 
                       storage_manager<SEQ_PATTERN, VAT, ALLOC, SM_T>& vat_hmap ) {
    
    char* line=new char[MAXLINE];
    char word[MAXLINE];
    char* startline=line;
    
    int i=0, len, seqlen=0;
    static int tid=-1;
    int pos; //stores starting position of input stream's get pointer
    VAT* svat;
    bool first = true; //first line of new fasta seq in the file
    
    do {
      pos=infile.tellg();
      line=startline;
      *line = '\0';
      infile.getline(line, MAXLINE-1);
      //len=infile.gcount();
      len = strlen(line);
      
      if(len == 0){
        if (infile.eof()) {
          tid= -1;
          delete[] startline;
          return tid;
        }
        else continue; //just a blank line, skip
      }
      
      
      if (line[0] == '>'){
        if (first){
          tid++; // increment the seq id
          first = false;
          continue; //go onto next line
        }
        else{
          infile.seekg(pos); //reset the file pos to beginning of 
                     //line for next seq
          delete[] startline;
          return tid;
        }
      }
      
      //read the fasta seq
      for (i=0; i < len; ++i, ++seqlen){
        //read each char and insert into VAT
        //this is an element, insert/append to its VAT
        SEQ_PATTERN* p = new SEQ_PATTERN();
        V_T v = string(1,line[i]);
        
        
        // Add vertex and update the canonical code.
        p->add_vertex(v);
        p->init_canonical_code(v);
        
        //if p contains a vat in vat_hmap, append tid/ts to the entry
        //else create a new vat and insert it into vat_hmap,            //and add p to freq_pats
        svat=vat_hmap.get_vat(p);
        //if(vat_hmap.find(p))
        if(svat != NULL) {
          //vat found, check if this tid exists in it
          
          typename VAT::IT vit=svat->end()-1;
          if(vit->first!=tid)
            vit=svat->end();      
          
          if(vit!=svat->end())
            //tid found
            vit->second.push_back(INSTANCE(seqlen, seqlen));
          else {
            //tid not found
            typename VAT::INSTANCES new_tidlist;
            new_tidlist.push_back(INSTANCE(seqlen, seqlen));
            svat->push_back(make_pair(tid, new_tidlist));
          }
          
          delete p;
          
        }//end if(vat_hmap.find())
        else {
          //create a new vat & insert it
          svat=new VAT();
          typename VAT::INSTANCES new_tidlist;
          new_tidlist.push_back(INSTANCE(seqlen, seqlen));
          svat->push_back(make_pair(tid, new_tidlist));
          if(!vat_hmap.add_vat(p, svat)) {
            cerr<<"tokenizer.get_length_one: add_vat failed"<<endl;
            return -1;
          }
          freq_pats.push_back(p);
        }//end else
      }
      
    }while(true);
    
    return -1;
  }//end parse_next_trans()
  
private:
    int MAXLINE; /**< max length of line to be parsed */
  
}; //end class seq_tokenizer

#endif

