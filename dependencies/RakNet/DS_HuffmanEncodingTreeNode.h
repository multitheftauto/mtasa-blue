/// \file
/// \brief \b [Internal] A single node in the Huffman Encoding Tree.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __HUFFMAN_ENCODING_TREE_NODE
#define __HUFFMAN_ENCODING_TREE_NODE

struct HuffmanEncodingTreeNode
{
	unsigned char value;
	unsigned weight;
	HuffmanEncodingTreeNode *left;
	HuffmanEncodingTreeNode *right;
	HuffmanEncodingTreeNode *parent;
};

#endif
