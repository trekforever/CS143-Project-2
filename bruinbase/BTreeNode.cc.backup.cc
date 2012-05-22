#include "BTreeNode.h"
#include <iostream>	//Debugging purpsoes
//See BTreeNode.h for various other information, also the readme as well.

using namespace std;

// Constructor to allocate memory
BTLeafNode::BTLeafNode()
{	
	// [Done. Checked.]
	// We use the 1024 byte array member variable buffer and set it initially to 0
	// Note the PageFile::PAGE_SIZE is simply the size of the buffer array defined
	// in the header file which is 1024 bytes

	memset(buffer,0,PageFile::PAGE_SIZE);

	// Global variables
	/*	__________
	 *     |key,value |
	 *     |key,value |
	 *     |key,value |
	 *      ----------- => sizeRec */
    
    // QUESTION:
    // Size of a record is the size of the record * # of keys?
    
    
	sizeRec = sizeof(RecordId) + sizeof(int);	//Size of each record
	//cout << "Record Size: " << sizeRec << endl;
	sizeTot = getKeyCount() * sizeRec;	// Total Size of a leafnode
	//cout << "Total Size: " << sizeTot << endl;
	//cout << "Current Key Count: " << getKeyCount() << endl;

	/*
	*	Example of Buffer Layout
    *   First 4 bytes:
	*	[sizeof(int)]	[sizeof(PageId)]	[sizeRec]          [sizeof(int)]
	*	[Key Count]     [First PageId]		[First RecordId]	[key]
	*
	*
	*/
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{	
	// [Done. Checked.]
	// Using RC PageFile::Read (defined in PageFile Class)
	return pf.read(pid, buffer);
 }
   
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{	
	// [Done. Checked.]
	// Using RC PageFile::Write (again defined in PageFile Class)
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
{	// [Done. Checked.]
	// PsuedoCode:
	// Each of our key is an int, so we can use sizeof(int) to determine
	// the number of bytes. Then couldn't we just get that number of bytes
	// from the buffer array, and shouldn't that just be the # of records?
	// Note that the first four bytes stores the # of records of a page
	int tempStorage = 0;	
	//memcpy(destination,source,size)
	memcpy(&tempStorage, &buffer, sizeof(int));	//get the first 4 bytes, store it into tempStorage
	return tempStorage;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{	// [Almost Done. Need to implement error-checking. Need Testing. Need code review (may have bugs/typos). ] 
	//PUSEDOCODE
	/*
	* Check to see if the leaf node is full. If so return RC_NODE_FULL.
	* First want to iterate/traverse through the entire tree, and find the location to insert.
	* If node is initially empty or 0, good! Otherwise increment pointer until we find a value
	* that is greater than the key. Store the eid into a variable.
	* Now check the buffer array. We need to shift everything down.
	* So allocate a new temp buffer array with the new size.
	* memcpy everything before the insertion point to temp buffer.
	* now insert our new value to the temp buffer.
	* insert everything after that to the temp buffer.
	* now memcpy the array back to pointer.
	* free our temp variable.
	* and set our new key count.
	* simply getKeyCount()++ and store it back into the pointer array.
	*/
	//cout << "DEBUG: Attempting to insert " << key << " at Slot# " << rid.sid << "and PageId# " << rid.pid << endl;
	//Need error checking, check to see if node is full
	int tempSize = PageFile::PAGE_SIZE - sizeRec - sizeof(int) - sizeof(PageId);
    
	if(sizeTot >= tempSize)
		return RC_NODE_FULL;	//Full node when total size exceeds the size of we have
    
	//cout << "Current Key Counts: " << getKeyCount() << endl;
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the first element, skipping count
    
    //QUESTION: Wouldn't this get the First RecordID? 
    // If your'e adding sizeof(PageId), isn't that the end?
    
	pointer += sizeof(PageId); //First PageID
	int IndexCursor = 0;
	int tempStorage = 0;

	if(pointer!=NULL)
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison. Initial.

	while(pointer!=NULL && tempStorage < key)	//Find the key that is right before our insertion point
	{
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison
		pointer += sizeRec;	//Goes to the next record
		IndexCursor++;
		//Should exit once tempStorage >= Key. Do we need to check when tempStorage = 0?
		//Let's do it just *incase
		if(tempStorage==0)
			break;	//break out of while loop. Haven't tested to see if this line works/is needed.
	}
	//Now we found the location to insert ...
	//Shifting everything down by storing into temp buffer
	size_t newSize = PageFile::PAGE_SIZE * sizeof(char);	//Size of buffer
	char* tempBuffer = (char*) malloc(newSize);	//Allocate new buffer
	memset(tempBuffer,0,sizeTot - (sizeRec*IndexCursor));	//set new buffer to all 0s initially
	if(tempStorage != 0 && sizeTot > 0)
	{
		// copy the data from buffer (pointer) to our tempBuffer, and reallocate/reinitalize our pointer
		memcpy(tempBuffer, pointer, sizeTot - (sizeRec*IndexCursor));
		memset(pointer,0,sizeTot - (sizeRec*IndexCursor));
	}
	//Finished copying data to tempBuffer. Now let's insert.
	memcpy(pointer,&key,sizeof(int));	//copy new key to pointer
	pointer+=sizeof(int);	//increment pointer position
	memcpy(pointer,&rid,sizeof(RecordId));	//Copy new record
	pointer+=sizeof(RecordId);	//increment the record
    
	if(tempStorage != 0 && sizeTot > 0)
	{
		//Copy the data from tempBuffer back
		memcpy(pointer,tempBuffer,sizeTot - (sizeRec*IndexCursor));
	}
	
	//Set new count and store it
	pointer = &buffer[0];	//Goes back to the beginning to set new keyCount
	int newCount = getKeyCount() + 1;
	memcpy(pointer,&(newCount),sizeof(int));
	free(tempBuffer);
    
    
	return 0; //NEEDS ERROR CHECKING & TESTING
 }

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
{ 
//Function won't compile - convertToLeafEntry and convertToChar Not defined --Xin
		// Define the leaf entry size and the maximum leaf entires
	/*int leafSize= sizeof(RecordId) + sizeof(int);
	int maxLeafEntries = (PageFile::PAGE_SIZE - sizeof(PageId)) / leafSize;
	
    // The id of the last before the split
    int spid = (int) (maxLeafEntries / 2 + 1);

	// Create 
    int oKey, eKey = -1;
    RecordId oRid, eRid = {-1, -1};
    
    for (int x = spid*leafSize; x < maxLeafEntries*leafSize; x += leafSize)
    {
	    convertToLeafEntry(buffer, x, oKey, oRid);
	    
	    // Copy entires into sibling
	    sibling.insert(oKey, oRid);
        
	    // Assign sibling key
	    if (x == spid*leafSize)
	        siblingKey = oKey;
        
		// Create a buffer for the leaf
        char buf[leafSize];
        convertToChar(eKey, eRid, buf);
        
		// Make entry in the leaf empty
        for (int y = x; y < (x + leafSize); y++)
            buffer[y] = buf[y-x];
    }
    
    // Set the next pointer for the sibling
    sibling.setNextNodePtr(getNextNodePtr()); 
    
    // Insert the record
	if (key >= siblingKey)
		sibling.insert(key,rid);
	else
		insert(key,rid);
		*/
    return 0;
	
}

/*
 * Find the entry whose key value is larger than or equal to searchKey
 * and output the eid (entry number) whose key value >= searchKey.
 * Remeber that all keys inside a B+tree node should be kept sorted.
 * @param searchKey[IN] the key to search for
 * @param eid[OUT] the entry number that contains a key larger than or equalty to searchKey
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{	
	// [Done. Checked.]
	// We want to traverse through the memory buffer array
	// to find a key that is >= searchKey

	//Create a char pointer that points to the first element in buffer
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the first element, skipping the count
	int tempStorage = 0;
	int IndexCursor = 0;
	
	while(pointer!=NULL)
	{
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison
		if(tempStorage >= searchKey)
		{
			eid = IndexCursor;	//Found it!
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
	// [Done.]
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the first element
	int IndexCursor = 0;

	pointer += sizeof(PageId);	//First pageID
	while(pointer != NULL)
	{
		if(IndexCursor < eid)
		{			// Traverse through the PageIds to find the corresponding eid
			pointer += sizeRec;
			IndexCursor++;
		}
		else
			break;
	}
	if(pointer == NULL)
	{
		//Didn't find what we were looking for
		return RC_END_OF_TREE;
	}
	else{
		//Found it!
		memcpy(&key, pointer, sizeof(int));	//Copy the key value from array to output
		pointer += sizeof(int);	// Goes to the beginning of the record
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
	// [Done. Checked. ]
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
{ 
	// [Done.]
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the first element, skipping count
	memcpy(pointer,&pid,sizeof(PageId));	//Copy PageId to pointer
	return 0;	//Error Checking Needed
}

/*
 *
 *
 * NON-LEAF-NODE
 *
 *
 */

//Constructor to allocate memory, copied from the LeafNode Constructor
BTNonLeafNode::BTNonLeafNode()
{ 
	// [Done. Checked.] 
	// We use the 1024 byte array member variable buffer and set it initially to 0
	// Note the PageFile::PAGE_SIZE is simply the size of the buffer array defined
	// in the header file which is 1024 bytes

	memset(buffer,0,PageFile::PAGE_SIZE);

	// Global variables
	sizeRec = sizeof(RecordId) + sizeof(int);	//Size of each record
	sizeTot = getKeyCount() * sizeRec;	// Total Size
	//See LeafNode constructor for more information
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ 
	// [Done. Checked.] 
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
	// [Done. Checked.] 
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
{	
	// [Done. Checked].
	//Copied from LeafNode::getKeyCount
	int tempStorage = 0;	
	memcpy(&tempStorage, &buffer, sizeof(int));	//First 4 bytes of page is count info
	return tempStorage;
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{
	//[Done.]
	//cout << "Key to Insert: " << key << endl;
	//Similar to the BTLeafNode::insert (to an similar extent) 
	//So some codes are copied from that function respectively
	char* pointer = &buffer[0]+sizeof(int)+sizeof(PageId);	//First page
	//Want to iterate through the pages until we find the one to insert beforehands
	int IndexCursor = 0;
	int tempStorage = 0;	//Storage comparison
	// Need an error checking, check to see if node is full
	int tempSize = PageFile::PAGE_SIZE - sizeRec;
	if(sizeTot >= tempSize)
		return RC_NODE_FULL;	//If our total size exceeds our size, return full
	if(pointer!=NULL)
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison. Initial.
	while(pointer!=NULL && tempStorage < key)	//Find the key that is right before our insertion point
	{
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison
		pointer += sizeRec;	//Goes to the next record
		IndexCursor++;
		//Should exit once tempStorage >= Key. Do we need to check when tempStorage = 0? Let's just do it just incase.
		if(tempStorage==0)
			break;	//Needs testing and checking
	}
	
	if(tempStorage == key)
	{
		//Found the key already exist, update pageId. No need to add new key.
		pointer+=sizeof(int);	//goes to the beginning of pageId
		memcpy(pointer,&pid,sizeof(PageId));
	}
	
	//Shifting everything down, blah blah blah, same as LeafNode::insert
	size_t newSize = PageFile::PAGE_SIZE * sizeof(char);
	char* tempBuffer = (char*) malloc(newSize);
	memset(tempBuffer,0,sizeTot - (sizeRec*IndexCursor));
    
	if(tempStorage != 0 && sizeTot > 0)
	{
		// copy the data from buffer (pointer) to our tempBuffer, and reallocate/reinitalize our pointer
		memcpy(tempBuffer, pointer, sizeTot - (sizeRec*IndexCursor));
		memset(pointer,0,sizeTot - (sizeRec*IndexCursor));
	}
	memcpy(pointer,&key,sizeof(int));	//copy new key
	pointer+=sizeof(int);	//increment pointer position
	memcpy(pointer,&pid,sizeof(PageId));	//Copy new pageId
	pointer+=sizeof(PageId);
	if(tempStorage != 0 && sizeTot > 0)
	{	//Valid Data!
		//Copy the data from tempBuffer back
		memcpy(pointer,tempBuffer,sizeTot - (sizeRec*IndexCursor));
	}
	//Set new count and store it
	pointer = &buffer[0];	//Goes back to the beginning
	int newCount = getKeyCount() + 1;	//Increment count
	memcpy(pointer,&(newCount),sizeof(int));
	
	return 0;

}

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
{ 
	// [Done. ] 
	char* pointer = &buffer[0]+sizeof(int);;	//First element
	int tempStorage = 0;	//storage for comparison
	memcpy(&tempStorage, pointer, sizeof(int));	//copy first key to tempStorage for comparison. initial
	while(pointer != NULL && tempStorage < searchKey)
	{
		memcpy(&tempStorage, pointer, sizeof(int));	//copy first key to tempStorage for comparison. 
		pointer+=sizeof(int)+sizeRec;
		//exit once found a key that is >= searchKey
	}
	//Located
	pointer-=sizeof(PageId);	//Back a page, want to add before
	memcpy(&pid,pointer,sizeof(PageId));
	return 0;	//Needs ERROR CHECKING

}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ 
	// [Done. Needs Error Checking.]
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the 1st element
	
	memcpy(pointer, &pid1, sizeof(PageId));	//Initialize first pageId
	pointer += sizeof(PageId);	// Increment by that amount
	memcpy(pointer, &key, sizeof(int));	//Initialize the key
	pointer += sizeof(int);	//Increment by that amount again
	memcpy(pointer, &pid2, sizeof(PageId));	//Create the 2nd PageId to insert behind the key
	pointer = &buffer[0];	//Modify Count
	int count = getKeyCount()+1;
	memcpy(pointer,&count,sizeof(int));	//Incrementing count
	
	return 0;

 }
