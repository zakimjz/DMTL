/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:
 *      Added tokenizer properties -- Zaki, 5/8/06
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
#ifndef _GRAPH_TOKENIZER_H
#define _GRAPH_TOKENIZER_H

#include <fstream>
#include "graph_vat.h"
#include "element_parser.h"
#include "generic_classes.h"
#include "tokenizer_utils.h"

using namespace std;


/**
* \brief Graph tokenizer class by partial specialization of the generic tokenizer class.
 *
 * the template argument is instantiated with a pattern that has undirected pattern property(graph),
 * MINING_PROPS type of mining property, ST type of pattern storage and CC type of
 * canocial code.
 */
template<typename PP, typename MP, typename TP, typename PAT_ST, template<class, typename, typename, template <typename> class> class CC, 
template <typename> class ALLOC >
class tokenizer<GRAPH_PATTERN, DMTL_TKNZ_PROP, ALLOC >
{
public:
  typedef vat<GRAPH_PROP, V_Fk1_MINE_PROP, ALLOC, std::vector > VAT;
  
  tokenizer(const int max=LINE_SZ): MAXLINE(max) {} /**<constructor for tokenizer */
  
  /** \fn int parse_next_trans(ifstream& infile, pat_fam<PATTERN>& freq_pats, vat_db<PATTERN, VAT>& vat_hmap)
    * returns the TID of transaction read;
    * parses one transaction from input database, and collects VATS in vat_hmap
    * return value is -1 on end of stream
    */
  template<class SM_T>
  int parse_next_trans(ifstream& infile, pat_fam<GRAPH_PATTERN>& freq_pats, storage_manager<GRAPH_PATTERN, 
                       VAT, ALLOC, SM_T>& vat_hmap) {
    char* line=new char[MAXLINE];
    char word[MAXLINE];
    char* startline=line;
    
    int len;
    int count; //# of words parsed from line
    int tid=-1;
    int num_items=-1; //# of words to be read from this line
    int pos; //stores starting position of input stream's get pointer
    VAT* gvat;
    GRAPH_PATTERN* g1;
    
    map<int, typename GRAPH_PATTERN::VERTEX_T> vid_to_lbl; // map from vertex-id 
                                 // to its label
    typename map<int, typename GRAPH_PATTERN::VERTEX_T>::iterator tmp_it;
    
    while(1) {
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
      
      if(line[0]=='#') // comment line
        continue;
      
      if(!(line=parse_word()(line, word))) {
        //parse_word() failed
        delete[] startline;
        return -1;
      }
      
      if(word[0]=='t') { // this is the tid line
        if(tid!=-1) { // this is a new tid, stop here
          infile.seekg(pos);
          delete[] startline;
          return tid; // this is the line from where function should 
                // return on most calls 
        }
        
        line=parse_word()(line, word); // read in the '#'
        if(!line) {
          //parse_word() failed
          delete[] startline;
          return -1;
        }
        
        line=parse_word()(line, word); // read in the tid
        if(!line) {
          //parse_word() failed
          delete[] startline;
          return -1;
        }
        tid=atoi(word);
        
      }//if word[0]=='t'
      else if(word[0]=='v') { // this is a vid-line
        num_items=2; // 2 more words to be parsed from this line
        int vid=0;
        typename GRAPH_PATTERN::VERTEX_T v_lbl;
        
        while(count<num_items) {
          if(!(line=parse_word()(line, word))) {
            // parse_word() failed
            delete[] startline;
            return -1;
          }
          switch(count) {
            case 0: vid=atoi(word); break;
            case 1:
              v_lbl=el_prsr.parse_element(word); 
              /// INPUT-FORMAT: if the datafile format is to append 
              /// vertex labels with a letter (as is true for data 
              /// files in /dmtl/ascii_data on hd-01)
              /// then simply change the 
              /// above line to:
              /// v_lbl=el_prsr.parse_element(word+1); 
              vid_to_lbl.insert(make_pair(vid, v_lbl));
          }
          count++;
          
        }//while(count<..)
        
      }//if word[0]=='v'
      else if(word[0]=='e') { // undirected edge
                  /// INPUT-FORMAT: if running for files in /dmtl/ascii_data on hd-01
                  /// simply change the above line to:
                  ///     else if(word[0]=='u')
        int vid1=-1, vid2=-1;
        typename GRAPH_PATTERN::EDGE_T e_lbl;
        typename GRAPH_PATTERN::VERTEX_T v_lbl1, v_lbl2;
        num_items=3; // 3 more words to be parsed
        bool swap_vids; // flag=false if v_lbl1<v_lbl2
        
        while(count<num_items) {
          if(!(line=parse_word()(line, word))) {
            // parse_word() failed
            delete[] startline;
            return -1;
          }
          
          switch(count) {
            case 0: 
              vid1=atoi(word); 
              if((tmp_it=vid_to_lbl.find(vid1))==vid_to_lbl.end()) {
                cerr<<"graph_tokenizer.parse_next_trans: vid "<<vid1<<" not found in vid_to_lbl"<<endl;
                return -1;
              }
                v_lbl1=tmp_it->second;
              break;
              
            case 1: 
              vid2=atoi(word);
              if((tmp_it=vid_to_lbl.find(vid2))==vid_to_lbl.end()) {
                cerr<<"graph_tokenizer.parse_next_trans: vid "<<vid2<<" not found in vid_to_lbl"<<endl;
                return -1;
              }
                v_lbl2=tmp_it->second;
              break;
              
            case 2: 
              e_lbl=edge_prsr.parse_element(word); 
              /// INPUT-FORMAT: if the datafile format is to append 
              /// edge labels with a letter (as is true for data 
              /// files in /dmtl/ascii_data on hd-01)
              /// then simply change the 
              /// above line to:
              /// e_lbl=el_prsr.parse_element(word+1); 
              
              /// prepare pattern ///
              g1=new GRAPH_PATTERN;
              if(v_lbl1<=v_lbl2) {
                make_edge(g1, v_lbl1, v_lbl2, e_lbl);
                swap_vids=0;
              }
                else {
                  make_edge(g1, v_lbl2, v_lbl1, e_lbl);
                  swap_vids=1;
                }
                
                /// if g1's vat is present, check if this tid is also 
                /// present. If yes, then insert pair of vids. 
                /// If tid not present, create a new entry in vat and 
                /// insert it 
                
                if(!(gvat=vat_hmap.get_vat(g1))) { // vat not found
                  gvat=new VAT;
                  if(!swap_vids)
                    gvat->insert_occurrence_tid(tid, make_pair(vid1, vid2));
                  else
                    gvat->insert_occurrence_tid(tid, make_pair(vid2, vid1));
                  
                  gvat->insert_vid_tid(vid1);
                  gvat->insert_vid(vid2);
                  vat_hmap.add_vat(g1, gvat); // add pattern-vat mapping
                  freq_pats.push_back(g1); // this is the first time 
                               // this pattern has been encountered, so add it
                }
                else if(gvat->back().first!=tid) { // or, new tid
                  if(!swap_vids)
                    gvat->insert_occurrence_tid(tid, make_pair(vid1, vid2));
                  else
                    gvat->insert_occurrence_tid(tid, make_pair(vid2, vid1));
                  
                  gvat->insert_vid_tid(vid1);
                  gvat->insert_vid(vid2);
                  delete g1;
                }
                else { // assert: gvat->back().first=tid
                  if(!swap_vids)
                    gvat->insert_occurrence(make_pair(vid1, vid2));
                  else
                    gvat->insert_occurrence(make_pair(vid2, vid1));
                  
                  gvat->insert_vid_hs(vid1);
                  gvat->insert_vid(vid2);
                  delete g1;
                }
                
          }//switch
          count++;
        }//while(count<..)
        
      }//if(word[0]=='u')
      else {
        cerr<<"graph.tokenizer.parse_next_trans: Unidentifiable line="<<line<<endl;
        return -1;
      }
    }//while(1)
    
    return tid;
    
  }//parse_next_trans()
  
private:
  int MAXLINE; /**< max length of line to be parsed */
  element_parser<typename GRAPH_PATTERN::VERTEX_T> el_prsr; /**< parses an element of desired type */
  element_parser<typename GRAPH_PATTERN::EDGE_T> edge_prsr; /**< parses an element of desired type */
    
}; //end class tokenizer

#endif

