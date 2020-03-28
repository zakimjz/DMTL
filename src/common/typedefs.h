/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *  Modifications:
 *      added IBM and FASTA tokenizers -- Zaki, 5/8/06
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
#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_


#include "properties.h"

// Mining properties #defines
#define ISET_PROP proplist<no_edges, proplist<uniq_label, PP > >  // PP stands for Pattern Property

#define MSET_PROP proplist<no_edges, PP >  // PP stands for Pattern Property

#define SEQ_PROP proplist<directed, proplist<acyclic, proplist<indegree_lte_one, proplist<outdegree_lte_one, PP > > > >

#define TREE_PROP proplist<directed, proplist<acyclic, proplist<indegree_lte_one, PP > > >

#define ORD_TREE_PROP proplist<directed, proplist<acyclic, proplist<indegree_lte_one, proplist<ordered, PP > > > >

#define UNORD_TREE_PROP proplist<directed, proplist<acyclic, proplist<indegree_lte_one, proplist<ordered, PP > > > >

#define GRAPH_PROP proplist<undirected, PP >

// Mining properties #defines 
#define V_Fkk_MINE_PROP proplist<Fk_Fk, proplist<vert_mine, MP > >  // MP stands for Mining property

#define V_Fkk_EMB_MINE_PROP proplist<Fk_Fk, proplist<vert_mine, proplist<embedded, MP > > >

#define V_Fkk_IND_MINE_PROP proplist<Fk_Fk, proplist<vert_mine, proplist<induced, MP > > >

#define V_Fk1_MINE_PROP proplist<Fk_F1, proplist<vert_mine, MP > >

// Tokenizer properties #defines
#define DMTL_TKNZ_PROP proplist<dmtl_format, TP> //TP stands for tokenizer property
#define FASTA_TKNZ_PROP proplist<fasta_format, TP> //TP stands for tokenizer property

// Itemset Mining #defines
#define ISET_PATTERN pattern<ISET_PROP, V_Fkk_MINE_PROP, PAT_ST, CC, ALLOC >  // PAT_ST is pattern storage type, CC is for Canonical Code

// Multiset Mining #defines
#define MSET_PATTERN pattern<MSET_PROP, V_Fkk_MINE_PROP, PAT_ST, CC, ALLOC >  // PAT_ST is pattern storage type, CC is for Canonical Code

// Sequence  Mining #defines
#define SEQ_PATTERN pattern<SEQ_PROP, V_Fkk_MINE_PROP, PAT_ST, CC, ALLOC > 

// Tree Mining #defines
#define TREE_PATTERN pattern<TREE_PROP, V_Fkk_MINE_PROP, PAT_ST, CC,ALLOC >

// Graph Mining #defines
#define GRAPH_PATTERN pattern<GRAPH_PROP, V_Fk1_MINE_PROP, PAT_ST, CC, ALLOC >
#endif
