/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"
const int GLOBALpsize = PageFile::PAGE_SIZE;
using namespace std;

Node::Node(PageId pid) {
	//Set pid
	this->pid = pid;
}

PageId Node::getPid(){
	//Return pid value
	return this->pid;
}

//Linked List Structure
DoubleLinkedList::DoubleLinkedList() {
	//First the list is empty
	head = 0;
	tail = 0;
	size = 0;

}

int DoubleLinkedList::insert(PageId pid)
{
	if(size==0)
	{
		//Empty list
		head = new Node(pid);
		tail = head;	//tail is the head
		tail->next = NULL;
		tail->last = NULL;
		size++;
	}
	else
	{	
		//We create a new node at the end
		Node* newNode = new Node(pid);
		//Set the existing tail node cto point to this
		tail->next = newNode;
		//Have this node point to NULL (since now it is at the end)
		newNode->next = NULL;
		//Have this last ptr point to the one before, which is tail
		newNode->last = tail;
		//Now make tail the new node we just created
		tail = newNode;

	}
}

int DoubleLinkedList::destory()
{
	while(head!=tail)
	{
		Node* tobedeleted = tail->last;
		delete tail;
		tail = tobedeleted;
	}
	delete head;
	return 0;	//success
}

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    	rootPid = -1;
	treeHeight = 0;	//initially 0 count. Needed to know whether reached leaf level
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
	//set variable in class
	method = mode;
	//Try to open the pageFile
	if(pf.open(indexname,mode) != 0)
		return RC_FILE_OPEN_FAILED;

	//Writing
	if(method=='w')
		return 0;
	else
	{	
	//Reading
		if(pf.read(0, bufferArr)!=0)
			return RC_FILE_READ_FAILED;	//Unable to read during read mode
	}

	char* pointer = &bufferArr[0];
	memcpy(&rootPid,pointer,sizeof(PageId));	//Copy the rootPid from buffer containing the index to output
	//cout << "Root is " << rootPid << endl;
	pointer+= sizeof(PageId);
	memcpy(&treeHeight,pointer,sizeof(int));		//Copy height to the beginnning
	//cout << "Height is " << height << endl;
    	return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
	if(method == 'r')
		return pf.close(); //if reading, we can just close it
	else
	{	//Writing Mode
		char* pointer = &bufferArr[0];
		//Allocate the pointer to 0 initially
		memset(pointer,0,GLOBALpsize);
		//Copy rootPid to our buffer
		memcpy(pointer,&rootPid, sizeof(PageId));
		//Increment the pointer (note rootPid is a PageId, so we do sizeof(PageId)
		pointer+=sizeof(PageId);
		//Copy the height to there as well)
		memcpy(pointer,&treeHeight,sizeof(int));
		if(pf.write(0,bufferArr) != 0)	//We write the rootPid and height info to pid 0
			return RC_FILE_WRITE_FAILED;
	    	return pf.close();
	}
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    PageId newPid;
    BTNonLeafNode sib, nonLeaf; // sibling
    BTLeafNode leaf; // QUESTION: When do constructors get called again?
    int newKey;
    
    // If empty tree
    if (treeHeight == 0) {
        // Increase tree height
        treeHeight++;
        
        // Insert to the leaf
        leaf.insert(key, rid);
        
        // This is for hte return value
        rootPid = pf.endPid();
        
        return leaf.write(rootPid, pf);
    } else {
		// FIXME: what's wrong with this? Doesn't compile.
        //return insertHelp(key, rid, rootPid, newKey, newPid, 1);
    }
  
    // FIXME: Error checking?

    return 0; // Success
}

RC BTreeIndex::insertHelp(int key, RecordId& rid, PageId pid, int& nKey, PageId& nPid, int lvl)
{
	BTLeafNode leaf, sib; 					// Leaf node, sibling
	BTNonLeafNode nonLeaf, nonLeafSib; 		// Nonleafnode / nonleafsibling
	PageId sPid, sibPid;					// Split/Sibling pid
	int sKey, midKey; 						// Sibling key, middle key
	RC rc; 									// return value
	
	// Base case, where level is the tree height (leaf)
	if (lvl == treeHeight){
		leaf.read(pid, pf);
		
		// If insert isn't successful (full)
		if (leaf.insert(key, rid) != 0){
			// Insert and split
			leaf.insertAndSplit(key, rid, sib, sKey);
			
			// Write the sibling node
			sPid = pf.endPid();
			
			leaf.write(pid, pf);
		} else { // Successful
		
			leaf.write(pid, pf);
			
			return 0; // Success
		}
	} else { // Else it's a non-leaf node
		// Initialize nonLeaf
		nonLeaf.initializeRoot(EMPTY, EMPTY, EMPTY);
		nonLeaf.read(pid, pf);
		nonLeaf.locateChildPtr(key, nextPid);
		
		// Recursively insert
		rc = this->insertHelp(key, rid, nextPid, sPid, sKey, lvl+1);
		
		// If we had to split
		if (rc == RC_NODE_FULL){
			if (nonLeaf.insert(sKey, sPid) == 0) {
				nonLeaf.write(pid, pf);
				return rc;
			} else {
				// It's full
				rc = RC_NODE_FULL;
				
				// Initialize
				nonLeafSib.initializeRoot(EMPTY, EMPTY, EMPTY);
				
				// Insert and split
				nonLeaf.insertAndSplit(sKey, sPid, nonLeafSib, midKey);
				
				sibPid = pf.endPid();
				nonLeafSib.write(sibPid, pf);
				
			}
		} else 
			return rc;
	}
	
	nKey = sKey;	// Set return key
	nPid = nPid;	// Set return pid
	
	return rc;
}
/*
 * Find the leaf-node index entry whose key value is larger than or 
 * equal to searchKey, and output the location of the entry in IndexCursor.
 * IndexCursor is a "pointer" to a B+tree leaf-node entry consisting of
 * the PageId of the node and the SlotID of the index entry.
 * Note that, for range queries, we need to scan the B+tree leaf nodes.
 * For example, if the query is "key > 1000", we should scan the leaf
 * nodes starting with the key value 1000. For this reason,
 * it is better to return the location of the leaf node entry 
 * for a given searchKey, instead of returning the RecordId
 * associated with the searchKey directly.
 * Once the location of the index entry is identified and returned 
 * from this function, you should call readForward() to retrieve the
 * actual (key, rid) pair from the index.
 * @param key[IN] the key to find.
 * @param cursor[OUT] the cursor pointing to the first index entry
 *                    with the key value.
 * @return error code. 0 if no error.
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
	//Base case is when height is 0 or 1
    if(treeHeight==0)
	{
		rootPid = 1;
		cursor.pid = 1;
		cursor.eid = 0;
		treeHeight++;
	}
	if(treeHeight==1)
	{
		int indexCursor = 0;
		BTLeafNode temp;
		if(temp.read(rootPid,pf) != 0)
			return RC_FILE_READ_FAILED;	//Unable to read
		if(temp.locate(searchKey,indexCursor) != 0)
			return RC_NO_SUCH_RECORD;	//Unable to find
		//Found and stored in IndexCusor
		cursor.pid = rootPid;
		cursor.eid = indexCursor;
	}
	else
	{
		BTNonLeafNode nonleaf;
		if(!nonleaf.read(rootPid, pf) != 0)
			return RC_FILE_READ_FAILED;
		int depth = 1;
		while(depth < treeHeight) {
			//We want to insert the next pid to our linked list
			if(depth == 1)
			{
				structure.insert(rootPid);
			}
			else
			{
				structure.insert(nextPid);
			}
			if(nonleaf.locateChildPtr(searchKey,nextPid) != 0)
				return RC_NO_SUCH_RECORD;
			else if(nonleaf.read(nextPid,pf) != 0)
				return RC_FILE_READ_FAILED;
			depth++;
		}
		//At this point, we located the nextPid
		cursor.pid = nextPid;
		cursor.eid = 0;
	}
	return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	//Create a new node and attempt to fill this empty temp with some
	//useful information by reading it from cursor
	BTLeafNode temp;

	//Reading error checks
	if(temp.read(cursor.pid, pf) != 0)
	{
		//Error in reading page
		return RC_FILE_READ_FAILED;
	}
	else
		//Store key and rid into the output value given the input eid
		temp.readEntry(cursor.eid, key, rid);	

	//If you get here, we now want to move the cursor to the next entry
	if(cursor.eid >= temp.getKeyCount() - 1)
	{
		cursor.eid = 0;
		int next = temp.getNextNodePtr();
		if(next == 0)
			return RC_END_OF_TREE;	//Next node is 0, empty tree
		else
		{
			cursor.pid = next;	//set the cursor to our next entry
			return 0;		
		}
	}
	else
	{
		//if our position is less than where it suppose to be at, increment it
		cursor.eid++;
		return 0;
	}

	
    return 0;
}
