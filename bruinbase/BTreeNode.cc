#include "BTreeNode.h"

using namespace std;


// Constructor to allocate memory
BTLeafNode::BTLeafNode()
{ // [Done - Needs checking. Jon?] 
	// We use the 1024 byte array member variable buffer and set it initially to 0
	// Note the PageFile::PAGE_SIZE is simply the size of the buffer array defined
	// in the header file which is 1024 bytes

	memset(buffer,0,PageFile::PAGE_SIZE);

	// Global variables
	sizeTot = getKeyCount() * sizeRec;	//Size of each record
	sizeRec = sizeof(RecordId) + sizeof(int);	// Total Size
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ // [Done - Needs checking. Jon?] 
	//Using RC PageFile::Read (defined in PageFile Class)
	return pf.read(pid, buffer);
 }
   
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ // [Done - Needs checking. Jon?] 
	//Using RC PageFile::Write (again defined in PageFile Class)
	// The write function of PageFile automatically expands to include
	// a new page if the pid goes past the last page, so I think
	// we don't need to do any error checking for that?
	return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ 
	// PsuedoCode:
	// Each of our key is a int, so we can use sizeof(int) to determine
	// the number of bytes. Then couldn't we just get that number of bytes
	// from the buffer array, and shouldn't that just be the # of records?
	int tempStorage = 0;	
	memcpy(&tempStorage, &buffer, sizeof(int));
	return tempStorage;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ return 0; }

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ return 0; }

/*
 * Find the entry whose key value is larger than or equal to searchKey
 * and output the eid (entry number) whose key value >= searchKey.
 * Remeber that all keys inside a B+tree node should be kept sorted.
 * @param searchKey[IN] the key to search for
 * @param eid[OUT] the entry number that contains a key larger than or equalty to searchKey
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ // [Done - Needs checking. Jon?]
	// We want to traverse through the memory buffer array
	// to find a key that is >= searchKey

	//Create a char pointer that points to the first element in buffer
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the first element
	int tempStorage = 0;
	int IndexCursor = 0;
	
	while(pointer!=NULL)
	{
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison
		if(tempStorage >= searchKey)
		{
			eid = IndexCursor;
			return 0;
		}
		else
		{
			IndexCursor++;	//Increment entry number
			pointer += sizeRec;	// Goes to the next record/key pair and compare again
		}
		// If we still don't find it at this point
		return RC_NO_SUCH_RECORD;
		
	}
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ 
	// [Done - Needs checking. Jon?]
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the first element
	int IndexCursor = 0;

	pointer += sizeof(PageId);	//First pageID
	while(pointer != NULL)
	{
		if(IndexCursor >= IndexCursor)
			break;
		else{			// Traverse through the PageIds to find the corresponding eid
			pointer += sizeRec;
			IndexCursor++;
		}
	}
	if(pointer == NULL)
	{
		//Didn't find what we were looking for
		return RC_END_OF_TREE;
	}
	else{
		//Found it!
		memcpy(&key, pointer, sizeof(int));	//Copy the key value from array to output
		pointer += sizeof(int);	// Goes to the record
		memcpy(&rid, pointer, sizeof(RecordId));	//Copy the recordId to output
		return 0;
	}
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	// [Done - Needs checking. Jon?]
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the first element
	
	PageId nextPage;
	memcpy(&nextPage, pointer, sizeof(PageId));
	return nextPage;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ return 0; }

/*
 *
 *
 * NON-LEAF-NODE
 *
 *
 */

//Constructor to allocate memory, copied from the LeafNode Constructor
BTNonLeafNode::BTNonLeafNode()
{ // [Done - Needs checking. Jon?] 
	// We use the 1024 byte array member variable buffer and set it initially to 0
	// Note the PageFile::PAGE_SIZE is simply the size of the buffer array defined
	// in the header file which is 1024 bytes

	memset(buffer,0,PageFile::PAGE_SIZE);

	// Global variables
	sizeTot = getKeyCount() * sizeRec;	//Size of each record
	sizeRec = sizeof(RecordId) + sizeof(int);	// Total Size
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ // [Done - Needs checking. Jon?] 
	//Using RC PageFile::Read (defined in PageFile Class)
	return pf.read(pid, buffer);
 }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ 
	// [Done - Needs checking. Jon?] 
	//Using RC PageFile::Write (again defined in PageFile Class)
	// The write function of PageFile automatically expands to include
	// a new page if the pid goes past the last page, so I think
	// we don't need to do any error checking for that?
	return pf.write(pid, buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ return 0; }


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ return 0; }

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ 
	// [Done - Needs checking. Jon?] 
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the 1st element
	
	memcpy(pointer, &pid1, sizeof(PageId));	//Initialize first pageId
	pointer += sizeof(PageId);	// Increment by that amount
	memcpy(pointer, &key, sizeof(int));	//Initialize the key
	pointer += sizeof(int);	//Increment by that amount again
	memcpy(pointer, &pid2, sizeof(PageId));	//Create the 2nd PageId to insert behind the key
	
	return 0;

 }
