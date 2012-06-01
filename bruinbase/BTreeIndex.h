/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#ifndef BTREEINDEX_H
#define BTREEINDEX_H

#include "Bruinbase.h"
#include "PageFile.h"
#include "RecordFile.h"

#define EMPTY -1

/**
 * The data structure to point to a particular entry at a b+tree leaf node.
 * An IndexCursor consists of pid (PageId of the leaf node) and 
 * eid (the location of the index entry inside the node).
 * IndexCursor is used for index lookup and traversal.
 */
typedef struct {
    // PageId of the index entry
    PageId  pid;  
    // The entry number inside the node
    int     eid;  
} IndexCursor;

/*
 * Since we are allowed to use STL, we shall implement the structure of the BTreeIndex 
 * as a DoubleLinkedList. The DoubleLinkedList will simply consist of nodes.
 * The node structure is defined as below:
 */

// Node can be leaf or non-leaf
class Node {
public:
	Node(PageId pid);
	PageId getPid(); //public function to retrieve the pageID
	PageId pid;	//pid of the node (stored from constructor)
	Node* next;	//Next Node
	Node* last;	//Last node
    
};

class DoubleLinkedList {
public:
	DoubleLinkedList();
	int insert(PageId pid);	// Insert to the linked list
	int destory();	//Destory List
private:
	Node* head;
	Node* tail;
	int size;
    
};

/**
 * Implements a B-Tree index for bruinbase.
 * 
 */
class BTreeIndex {
public:
    BTreeIndex();
    
    /**
     * Open the index file in read or write mode.
     * Under 'w' mode, the index file should be created if it does not exist.
     * @param indexname[IN] the name of the index file
     * @param mode[IN] 'r' for read, 'w' for write
     * @return error code. 0 if no error
     */
    RC open(const std::string& indexname, char mode);
    
    /**
     * Close the index file.
     * @return error code. 0 if no error
     */
    RC close();
    
    /**
     * Insert (key, RecordId) pair to the index.
     * @param key[IN] the key for the value inserted into the index
     * @param rid[IN] the RecordId for the record being inserted into the index
     * @return error code. 0 if no error
     */
    RC insert(int key, const RecordId& rid);
    
    /**
     * InsertHelp(key, RecordId) pair to the index.
     * Used recursively to insert.
     * @param key[IN] the key for the value inserted into the index
     * @param rid[IN] the RecordId for the record being inserted into the index
     * @param pid[IN] the current pid
     * @param nKey[OUT] key of sibling node if split
     * @param nPid[OUT] pid of sibling node if split
     * @param lvl[IN] level of tree
     * @return error code. 0 if no error
     */
    RC insertHelp(int key, const RecordId& rid, PageId currPid, int& nKey, PageId& nPid, int lvl);
    
    /**
     * Find the leaf-node index entry whose key value is larger than or
     * equal to searchKey and output its location (i.e., the page id of the node
     * and the entry number in the node) as "IndexCursor."
     * IndexCursor consists of pid (page id of the node that contains the 
     * searchKey) and eid (the entry number inside the node)
     * to indicate the location of a particular index entry in the B+tree.
     * Note that, for range queries, we need to scan the B+tree leaf nodes.
     * For example, if the query is "key > 1000", we should scan the leaf
     * nodes starting with the key value 1000. For this reason,
     * this function returns the location of the leaf node entry
     * for a given searchKey, instead of returning the RecordId
     * associated with the searchKey.
     * Using the returned "IndexCursor", you will have to call readForward()
     * to retrieve the actual (key, rid) pair from the index.
     * @param key[IN] the key to find
     * @param cursor[OUT] the cursor pointing to the first index entry
     * with the key value
     * @return error code. 0 if no error.
     */
    RC locate(int searchKey, IndexCursor& cursor);
    
    /**
     * Read the (key, rid) pair at the location specified by the index cursor,
     * and move foward the cursor to the next entry.
     * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
     * @param key[OUT] the key stored at the index cursor location
     * @param rid[OUT] the RecordId stored at the index cursor location
     * @return error code. 0 if no error
     */
    RC readForward(IndexCursor& cursor, int& key, RecordId& rid);
    
    int getSmallestKey();
    
private:
    PageFile pf;         /// the PageFile used to store the actual b+tree in disk
    
    PageId   rootPid;    /// the PageId of the root node
    PageId	nextPid;
    int      treeHeight; /// the height of the tree
    /// Note that the content of the above two variables will be gone when
    /// this class is destructed. Make sure to store the values of the two 
    /// variables in disk, so that they can be reconstructed when the index
    /// is opened again later.
    
	char bufferArr[PageFile::PAGE_SIZE]; //ACt as buffer. (global variable, makes life easier)
	char method; //keep track of mode (needed for close)
    int smallKey;
};

#endif /* BTREEINDEX_H */
