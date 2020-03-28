/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu>, Rensselaer Polytechnic Institute
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

#ifndef _ADJ_LIST
#define _ADJ_LIST

#include <ext/hash_map>
#include <algorithm>
#include <map>
#include <sstream>
#include <iostream>

using namespace std;


template<typename VERTEX_T, typename EDGE_T, template <typename> class ALLOC> struct vertex_info;
template<typename VERTEX_T, typename EDGE_T, template <typename> class ALLOC>
ostream& operator<< (ostream&, const vertex_info<VERTEX_T, EDGE_T, ALLOC>&);

/// class to store all info associated with a vertex.
template<typename VERTEX_T, typename EDGE_T, template <typename> class ALLOC=std::allocator>
struct vertex_info
{

  typedef pair<int, EDGE_T> EDGE_P;
  typedef vector<EDGE_P, ALLOC<EDGE_P> > EDGES;
  typedef typename EDGES::iterator EIT;
  typedef typename EDGES::const_iterator CONST_EIT;
  
  vertex_info(const VERTEX_T& vert, const int& idval): v(vert), id(idval) {}
  /// Returns an iterator pointing to the begining of the list of out_edges.
  EIT out_begin() {return out_edges.begin();}
  /// Returns a const_iterator pointing to the begining of the list of out_edges.
  CONST_EIT out_begin() const {return out_edges.begin();}
  /// Returns an iterator pointing to the end of the list of out_edges.
  EIT out_end() {return out_edges.end();}
  /// Returns a  const_iterator pointing to the end of the list of out_edges.
  CONST_EIT out_end() const {return out_edges.end();}
  
  /// Returns an iterator pointing to the begining of the list of in_edges.
  EIT in_begin() {return in_edges.begin();}
  /// Returns a cont_iterator pointing  to the begining of the list of in_edges.
  CONST_EIT in_begin() const {return in_edges.begin();}
  /// Returns an iterator pointing to the end of the list of in_edges.
  EIT in_end() {return in_edges.end();}
  /// Returns a const_iterator pointing to the end of the list of in_edges.
  CONST_EIT in_end() const {return in_edges.end();}
  /// Adds an out_edge to the list of the out_edges.
  void add_out_edge(const int& dest, const EDGE_T& e)
  {
    //out_edges.insert(make_pair(dest, e));
    out_edges.push_back(make_pair(dest, e));
  }
  /// Adds an out_edge to the list of the out_edges.
  void add_in_edge(const int& src, const EDGE_T& e)
  {
    //in_edges.insert(make_pair(src, e));
    in_edges.push_back(make_pair(src, e));
  }

  /** 
   * Returns true if there exists an out-edge from this vertex to dest
   * and populates edge label in e 
   */
  bool out_edge(const int& dest, EDGE_T& e) const {
    CONST_EIT it;
    for(it=out_begin(); it!=out_end(); it++)
      if(it->first==dest) {
        e=it->second;
        return true;
    }
    return false;
  }//out_edge()
   
  /** Returns true if there exists an in-edge from src to this vertex
      and populates edge label in e */
  bool in_edge(const int& src, EDGE_T& e) const {
    CONST_EIT it;
    for(it=in_begin(); it!=in_end(); it++)
      if(it->first==src) {
        e=it->second;
        return true;
      }
    return false;
  }//in_edge()
  ///Returns true if this vertex is less than vertex2 
  bool operator< (const vertex_info<VERTEX_T, EDGE_T, ALLOC>& vertex2) const {

     if(v < vertex2.v)
        return true;
     else
        return false;
  }

  /// Outputs a vertex_info object  to the stream. This is a global function, not a member function.
  friend ostream& operator<< <>(ostream&, const vertex_info<VERTEX_T, EDGE_T, ALLOC>&);

  /// data members ///
  VERTEX_T v; //vertex object
  int id; //id of this vertex
  EDGES out_edges; //stores all edges for an undirected graph
  EDGES in_edges; //calls to this member should be made only for digraphs

}; //end struct vertex_info

// overloaded extraction over a pair - used by following hash_map extraction
template<typename E_T>
ostream& operator<< (ostream& ostr, const std::pair<int, E_T>& p) {
  ostr<<"("<<p.first<<" "<<p.second<<")";
  return ostr;
}

// overloaded extraction over the edgelist map
template<typename E_T>
ostream& operator<< (ostream& ostr, const vector<pair<int, E_T> >& hm) {
  typename vector<pair<int, E_T> >::const_iterator it;
  for(it=hm.begin(); it!=hm.end(); it++)
    std::cout<<*it<<" ";
  return ostr;
}


//friend extraction over output streams
template<typename V_T, typename E_T>
ostream& operator<< (ostream& ostr, const vertex_info<V_T, E_T>& vi) {
  ostr<<"["<<vi.id<<"|"<<vi.v<<"] OUT: ";
  typename vertex_info<V_T, E_T>::CONST_EIT it;
  ostr<<vi.out_edges;
  ostr<<" IN: ";
  ostr<<vi.in_edges;
  ostr<<endl;
  return ostr;
}//end operator<<

template<typename V_T, typename E_T, template <typename> class ALLOC >
class adj_list;
  
template<typename V_T, typename E_T, template <typename> class ALLOC >
ostream& operator<< (ostream&, const adj_list<V_T, E_T, ALLOC>&);

/**
 * \brief core adjacency list class to store the pattern.
 *
 * the template arguments are vertex_type and edge_type.
 */

template<typename V_T, typename E_T, template <typename> class ALLOC=std::allocator>
class adj_list
{

 public:
  typedef V_T VERTEX_T;
  typedef E_T EDGE_T;
  typedef vertex_info<VERTEX_T, EDGE_T, ALLOC> VERTEX_INFO;
  typedef adj_list<V_T, E_T, ALLOC > ADJ_L;

  template<typename T>
  class VERTEX_LIST: public std::vector<T, ALLOC<T> > {};//each vertex and its info is stored as a vector, for fast lookup since we'll know its unique id

  typedef VERTEX_LIST<VERTEX_INFO> ADJ_LIST;

  typedef typename ADJ_LIST::iterator IT;
  typedef typename ADJ_LIST::const_iterator CONST_IT;
  typedef typename VERTEX_INFO::EIT EIT;
  typedef typename VERTEX_INFO::CONST_EIT CONST_EIT;
  typedef std::pair<EIT, EIT> EIT_PAIR;
  typedef std::pair<CONST_EIT, CONST_EIT> CONST_EIT_PAIR;

  void* operator new(size_t size) {
    ALLOC<ADJ_L> aa;
    return aa.allocate(size);
  }

  void  operator delete(void *p, size_t size) {
    if (p) {
      ALLOC<ADJ_L> aa;
      aa.deallocate(static_cast<ADJ_L*> (p), size);
    }
  }
 
  //default constructor
  adj_list() {}
    
  IT begin() {return _alist.begin();}
  CONST_IT begin() const {return _alist.begin();}
  IT end() {return _alist.end();}
  CONST_IT end() const {return _alist.end();}

  inline
  int size() const {return _alist.size();} /**< Returns number of vertices */
  void clear() {_alist.clear();}
  void push_back(const VERTEX_INFO& vi) {_alist.push_back(vi);}

  /** Returns the info associated with this vertex id */
  IT vertex_vals(const int&);

  CONST_IT vertex_vals(const int& idval) const {
    CONST_IT it=_alist.begin();
    if(idval>size()-1) {
  std::cerr<<"adj_list.vertex_vals: out of range vertex id, "<<idval<<endl;
  exit(0);
    }
    it+=idval;
    return it;
  }// end vertex_vals() const

  /** Returns a pair of iterators, the first of the pair points to the first 
      entity in the set of out-edges of idval, the second to the end of edges*/
  std::pair<EIT, EIT> out_edges(const int& idval) {
    IT it=vertex_vals(idval);
    return make_pair(it->out_begin(), it->out_end());
  }//end out_edges()
  
  std::pair<CONST_EIT, CONST_EIT> out_edges(const int& idval) const {
    CONST_IT it=vertex_vals(idval);
    return make_pair(it->out_begin(), it->out_end());
  }//end out_edges() const

  /** Returns a pair of iterators, the first of the pair points to the first 
      entity in the set of in-edges of idval, the second to the end of edges*/
  std::pair<EIT, EIT> in_edges(const int& idval) {
    IT it=vertex_vals(idval);
    return make_pair(it->in_begin(), it->in_end());
  }//end in_edges()
  
  std::pair<CONST_EIT, CONST_EIT> in_edges(const int& idval) const {
    CONST_IT it=vertex_vals(idval);
    return make_pair(it->in_begin(), it->in_end());
  }//end in_edges() const

  /** Returns size of out-neighbors of vid */
  int out_nbr_size(const int& vid) const {
    pair<CONST_EIT, CONST_EIT> out_pit=out_edges(vid);
    return out_pit.second-out_pit.first;
  }

  /** Returns size of in-neighbors of vid */
  int in_nbr_size(const int& vid) const {
    pair<CONST_EIT, CONST_EIT> in_pit=in_edges(vid);
    return in_pit.second-in_pit.first;
  }

  /** Adds given vertex object and returns its id
      As is evident, these ids are generated in increasing order */
  int add_vertex(const VERTEX_T& v) {
    _alist.push_back(VERTEX_INFO(v, size()));
    return size()-1;
  } // end add_vertex()
  
  /** Adds edge FROM src TO dest */
  void add_out_edge(const int& src, const int& dest, const EDGE_T& e) {
    if((src>size()-1) || (dest>size()-1)) {
      std::cerr<<"adj_list::add_out_edge:out of bound vertex IDs, src="<<src<<" dest="<<dest<<" size()="<<_alist.size()<<endl;
      exit(0);
    }
    
    IT it=vertex_vals(src);
    it->add_out_edge(dest, e);
    
  } // end add_out_edge()

  /** Adds in-edge FROM src TO dest */
  void add_in_edge(const int& dest, const int& src, const EDGE_T& e) {
    if((src>size()-1) || (dest>size()-1)) {
      std::cerr<<"adj_list::add_in_edge:out of bound vertex IDs, src="<<src<<" dest="<<dest<<" size()="<<_alist.size()<<endl;
      exit(0);
    }
    
    IT it=vertex_vals(dest);
    it->add_in_edge(src, e);
    
  } // end add_in_edge()


  /** Returns true if there is an out-edge b/w specified vertices, 
      populates e with edge label */
  bool get_out_edge(const int& src, const int& dest, EDGE_T& e) const {
    CONST_IT it=vertex_vals(src);
    return it->out_edge(dest, e);
  }//end get_edge()      

  /** Returns true if there is an in-edge b/w specified vertices, 
      populates e with edge label */
  bool get_in_edge(const int& src, const int& dest, EDGE_T& e) const {
    CONST_IT it=vertex_vals(src);
    return it->in_edge(dest, e);
  }//end get_edge()      

  // friend output extraction
  friend ostream& operator<< <>(ostream&, const adj_list<V_T, E_T, ALLOC>&);
  
 private:
  ADJ_LIST _alist;
  //int _sz;
  
}; //end class adj_list


// friend extraction over output stream
template<typename V_T, typename E_T, template <typename> class ALLOC>
ostream& operator<< (ostream& ostr, const adj_list<V_T, E_T, ALLOC>& al) {
  typename adj_list<E_T, V_T, ALLOC>::CONST_IT it=al.begin();
  while(it!=al.end()) {
    ostr<<*it;
    it++;
  }
  ostr<<"---";
  return ostr;
}

template<typename V_T, typename E_T, template <typename> class ALLOC >
typename adj_list<V_T, E_T, ALLOC>::IT adj_list<V_T, E_T, ALLOC>::vertex_vals(const int& idval) {
  typename adj_list<V_T, E_T, ALLOC>::IT it=_alist.begin();
  if(idval>size()-1) {
    std::cerr<<"adj_list.vertex_vals: out of range vertex id, "<<idval<<endl;
    exit(0);
  }
  it+=idval;
  return it;
}// end vertex_vals()


#endif
