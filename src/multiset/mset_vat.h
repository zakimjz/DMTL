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
#ifndef _MSET_VAT_H_
#define _MSET_VAT_H_

#include <iterator>
#include "pattern.h"
#include "pat_support.h"
#include "generic_classes.h"
#include "typedefs.h"

template<class PP, class MP, template <typename> class ALLOC, template <typename P, typename> class ST >
ostream& operator<< (ostream& , const vat<PP, MP, ALLOC, ST >&);
 
template<class PP, class MP, template <typename> class ALLOC, template <typename P, typename> class ST >
istream& operator>> (istream& , vat<PP, MP, ALLOC, ST >&);


/**
 * \brief Itemset VAT class by partial specialization of the generic VAT class.
 *
 * In this partial specialization, PP is fixed to no_edges (itemset property),
 * MP is fixed to Fk X Fk and vert_mine (vertical mining with Fk X Fk),
 * ST is the storage type, what needs a base type information, here templatized with P
 */

template<class PP, class MP, template <typename> class ALLOC, template <typename, typename> class ST >
class vat<MSET_PROP, V_Fkk_MINE_PROP, ALLOC, ST >
{
 public:
  typedef pattern_support<V_Fkk_MINE_PROP> PAT_SUP;
  typedef vat<MSET_PROP, V_Fkk_MINE_PROP, ALLOC, ST > VAT;
  
  typedef ST<pair<int, int>, ALLOC<pair<int, int> > > IDLIST_T; // Definition of a itemset VAT.
  typedef typename IDLIST_T::const_iterator CONST_IT;
  typedef typename IDLIST_T::iterator IT;
 

  void* operator new(size_t size) {
    ALLOC<VAT> a;
    return a.allocate(size);
  }

  void  operator delete(void *p, size_t size) {
    if (p) {
      ALLOC<VAT> a;
      a.deallocate(static_cast<VAT*> (p), size);
    }
  }
 
  vat() {}
  
  IT begin() {
    return _idlist.begin();
  }

  CONST_IT begin() const {
    return _idlist.begin();
  }

  IT end() {
    return _idlist.end();
  }

  CONST_IT end() const {
    return _idlist.end();
  }

  bool empty() const {
    return _idlist.empty;
  }

  int size() {
    return _idlist.size();
  }
  
  void make_empty() { 
    _idlist.clear();
  }        

  /** Appends new entry into vat */
  void push_back(const pair<int, int>& val) {
    _idlist.push_back(val);
  }
    
  /** Creating an array operator to work over VAT **/
  pair<int, int>& operator[](int n) {
    return *(begin()+n);
  } 

  /** Resizes the vector  **/
  void resize(int x) {
    _idlist.resize(x);
  }

  //clear the vector
  void clear() {
    _idlist.clear();
  }
  
  
  /**
   * \brief Vat Intersection for Itemset patterns.
   * \param[in] vat_i A pointer to the VAT corresponding to the first pattern.
   * \param[in] vat_j A pointer to the VAT corresponding to the second pattern.
   * \param[out] cand_sup An array of pointers to pattern_support class instances that is modified in this routine to hold actual support value.
   * \param[in] is_l2 A boolean to designate whether current VAT intersection is a Level 2 vat_join.
     * \return A static array of VAT* to hold the pointer to the new VATs generated through the intersection.
     * \pre cand_sups array holds pointer to pattern_support instances with NO actual support value associated to it.
     * \post cand_sups array holding pattern_support instances HAS acutal support value associated to it.
     */
  template<class PAT>
    static VAT** intersection(const VAT* const& vat_i, const VAT* const& vat_j, PAT_SUP** cand_sups, PAT**, bool& is_l2) {
    
    VAT** cand_vats = new VAT*[1]; // This memory is reclaimed in count() routine of count_support.h file
    cand_vats[0] = new VAT();      // This memory is reclaimed in count() routine of count_support.h file, only if the 
    // corresponding pattern is NOT frequent, otherwise, Storage Manager get a hold of this
    // VAT.
    
    if(cand_sups[0] == 0) { // No pattern_support class associated to return support value, ERROR
      delete cand_vats[0];
      delete[] cand_vats;
      
      cerr << "mset_vat: No candidates VATs to be generated." << endl;
      return NULL;
    }
    
    CONST_IT it_i=vat_i->begin(), it_j=vat_j->begin();
    
    while(it_i!=vat_i->end() && it_j!=vat_j->end()) {
      if((*it_i).first < (*it_j).first) {
        it_i++;
        continue;
      }
      
      if((*it_j).first < (*it_i).first) {
        it_j++;
        continue;
      }
     
      if(vat_i == vat_j) {  // Intersecting the same pattern.
        if((*it_i).second > 1)
          cand_vats[0]->push_back(make_pair((*it_i).first, (*it_i).second-1));
      } else {
        cand_vats[0]->push_back(make_pair((*it_i).first, (*it_j).second));
      }

      it_i++;
      it_j++;
    }//end while
    
    // setting the support value that will be checked in the count method of count_support class.
    cand_sups[0]->set_sup(make_pair(cand_vats[0]->size(), cand_vats[0]->size()));
    
    return cand_vats;
  }
  
  /** \brief Return Vat length
   * 
   */

  unsigned long size() const {
    return _idlist.size();
  }

  /**
   * read from a file
   */
  vat(const VAT &orig) {
    *this= orig;
  }

  ~vat(){
    _idlist.clear();
  }
  
  /**
   * Overloaded = operator; avoids self assignment.
   */
  const VAT &operator=(const VAT &right) {
    if ( &right != this ) {         // avoid self assignment
      _idlist=right._idlist;
    }
    else
      cout << "Attempted assignment of a Vat to itself\n";
    
    return *this;   // enables cascaded assignments
  }
  
  // Return Vat length
  unsigned long int byte_size() const {
    
    return _idlist.size()* sizeof(int);
  }
 
/** 
  // Serialize an Itemset Vat to an Output stream
  void write_file(ostream & output) const {
    //cout<<"writing this:"<<*this;                                                                                              
    CONST_IT iter;
    iter =_idlist.begin();
    for (;iter!=_idlist.end();iter++){
      // output.write( reinterpret_cast<const char *>( & (*iter) ), sizeof(T));                                                 
      output.write( reinterpret_cast<const char *>( & (*iter) ), sizeof(int));
    }
  } // //end write_file

  //De-Serialize an Itemset Vat from an Input stream
  void read_file (istream & input, unsigned long int size) {
    // TODO: This should be the element type in the vat.                                                                      
    int temp;
    unsigned long int i;
    //unsigned long int j=size/sizeof(T);                                                                                     
    unsigned long int j=size/sizeof(int);
    _idlist.clear();
    for ( i=0;i<j;i++){
      //input.read(reinterpret_cast<char *>( & (temp) ), sizeof(T));                                                          
      input.read(reinterpret_cast<char *>( & (temp) ), sizeof(int));
      _idlist.push_back(temp);
    }
  }
**/ 

  friend ostream& operator<< <>(ostream& , const VAT&);
  
  friend istream& operator>> <>(istream& , VAT&);
  
 private:
  IDLIST_T _idlist;
  
};


template<class T, class MP, template <typename> class ALLOC, template <typename P, typename> class ST >
ostream& operator<<(ostream& output, vat<T, MP, ALLOC, ST> const& orig) {
  std::ostream_iterator<int> out(output, " ");
  std::copy(orig.begin(), orig.end(), out);
  output <<'\n';
  return output;   // enables cascading
}

template<class T, class MP, template <typename> class ALLOC, template <typename P, typename> class ST >
istream& operator>>(istream& input,  vat<T, MP, ALLOC, ST >& orig) {
  const unsigned int MAXLINE=20000;
  char line[MAXLINE];
  input.getline(line,MAXLINE);
  stringstream s(line);
  std::istream_iterator<int> in(s);
  std::istream_iterator<int> eof;
  
  orig._idlist.clear();
  std::copy (in,eof, std::back_inserter(orig._idlist));
  return input;   // enables cascading
}

#endif
