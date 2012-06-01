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
    
    
	//sizeRec = getKeyCount() * sizeRec;	//Size of each record
	//cout << "Record Size: " << sizeRec << endl;
    
    sizeRec = sizeof(RecordId) + sizeof(int);  
	sizeTot = getKeyCount()*sizeRec;                    // Total Size of a leafnode
    sizeMax = (PageFile::PAGE_SIZE - sizeof(PageId) - sizeof(int)) / sizeRec;     // Maximum # of rid/key pairs    
    sizeCount = sizeof(int);
    
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
	// we don't need to do any error checking for that?]
    return pf.write(pid,buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{	
//    // [Done. Checked.]
//	// PsuedoCode:
//	// Each of our key is an int, so we can use sizeof(int) to determine
//	// the number of bytes. Then couldn't we just get that number of bytes
//	// from the buffer array, and shouldn't that just be the # of records?
//	// Note that the first four bytes stores the # of records of a page

	int tempStorage = 0;	
	//memcpy(destination,source,size)
	memcpy(&tempStorage, &buffer, sizeCount);	//get the first 4 bytes, store it into tempStorage
	return tempStorage;
    
    /* General idea:
     *  Keep iterating until you find an empty entry.
     *  Each iteration adds to the count.
     *  Return the count.
     */
//    
//    int countKeys = 0;                                // number of keys
//    int lim = PageFile::PAGE_SIZE - sizeof(PageId); // max number
//    int x = 0;
//    int key;
//    RecordId rid;
//    
//    // Go through all, 4 last bytes are pointer to next leaf
//    do {
////        rid.pid = (int) buffer[0*x]; 
////        rid.sid = (int) buffer[4*x];
////        key = (int) buffer[8*x];
////        
//        int* temp = (int*) buffer;              // Your current buffer
//        rid.pid = *(temp + x/sizeof(int));      // Offset of x/4
//        rid.sid = *(temp + x/sizeof(int) + 1);  // Offset of x/4 + 1
//        key = *(temp + x/sizeof(int) + 2);      // Offset of x/4 + 2
//        
//        countKeys++; // Count keys
//        
//        x++;
//        
//        // While it's not an empty entry
//        // FIXME: If the whole thing is empty, it'll still count 1.
//        
//    } while (x < lim || (key != EMPTY || rid.sid != EMPTY || rid.pid != EMPTY));
//
//    return countKeys;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{	
    /* General idea:
     *  Get the number of keys starting with the current node
     *  Try to look for the key, if found, good. If not, set the temp to the end.
     *  From the last leaf node, shift the buffer over by 1 entry until your location
     *  Set that location to the pid and sid from @param rid 
     *      - FIXME: Writing an int into a char buffer can cause problems, I think? Jon.
     *  Write into the buffer.
     */ 
    
    const int numKey = getKeyCount(); // Number of keys
    
    if (numKey >= sizeMax)
        return RC_NODE_FULL;    // Return if it's full
    
    int tempId = 0;
    int tempKey = 0;
    
    // Locate spot to insert using locate()    
    // If unable to locate a spot to open, add to the end
    if (locate(key, tempId)) // Will return 0 if it does locate, 0 if doesn't
        tempId = numKey;	// Will not run if located
    
    // Starting from the last leaf, shift the character buffer over by 1 entry
	// Possible ERROR here:
    for (int x = sizeTot - sizeof(int); x >= sizeRec * tempId; x--)
		memcpy(buffer+x, buffer+x-sizeRec, sizeRec);
//        buffer[x+sizeRec] = buffer[x];
    
    // Make the (key, rid) a character array
    // FIXME: Writing integers to a character buffer may cause problems
	int offSet = sizeof(int) + tempId * sizeRec;
	memcpy(buffer+offSet, &rid, sizeof(RecordId));
	memcpy(buffer+offSet+sizeof(RecordId), &key, sizeof(int));

	//Set new count and store it
	int newCount = numKey + 1;
	// Set the count at the beginning of the buffer
	memcpy(buffer, &newCount ,sizeof(int));

//    char tempBuf[sizeTot];
//    tempBuf[0] = rid.pid;
//    tempBuf[1*sizeof(int)] = rid.sid;
//    tempBuf[2*sizeof(int)] = key;
    
//    // Finally, insert
//    for (int x = sizeTot * tempId; x < sizeTot * (tempId+1); x++)
//        buffer[x] = tempBuf[x - sizeTot * tempId];
//    
	return 0; 
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
    /* General idea:
     *  Check to see if there's too many nodes.
     *  If it is, return, if not:
     *  Get the Id of the last entry
     *  Start from the last entry of the current node to the very end:
     *  Read from the character buffer and copy the entry into sibling
     *  "Erase" the current entry with the empty one created.
     *  Set the sibling's next pointer
     *  Insert the record
     */
    
    // Check size, we shouldn't be splitting here
	int numKey = getKeyCount();
    if (numKey < sizeMax)
        return RC_NODE_FULL; 

	// Check if sibling is empty before it's called
	if (sibling.getKeyCount() > 0)
		return RC_NODE_FULL; 
	
    // Id of last entry to split
    int lastPid = sizeMax / 2;
	int keysToCopy = numKey - lastPid;

	// Create a temp key and an empty key, a is temp, b is empty.
    int bKey = EMPTY;
    RecordId bRid = {EMPTY, EMPTY};
    
	// Insert into sibling node
	for (int x = 0; x < keysToCopy; x++){
		// Get the key startin from middle
		int tempKey = 0;
		memcpy(&tempKey, buffer+sizeCount+(lastPid+x)*sizeRec+sizeof(RecordId), sizeof(int));
		//Get the Rid starting from the middle
		RecordId tempRid;
		memcpy(&tempRid, buffer+sizeCount+(lastPid+x)*sizeRec, sizeof(RecordId));
		
		if (x==0)
			siblingKey = tempKey;
		
		// Insert it into the sibling
		sibling.insert(tempKey, tempRid);
		
		memcpy(buffer+sizeCount+(lastPid+x)*sizeRec, &bRid, sizeof(RecordId));
		memcpy(buffer+sizeCount+(lastPid+x)*sizeRec+sizeof(RecordId), &bKey, sizeof(int));
	}

//    // Start from the middle
//    for (int x = sizeof(int)+lastPid*sizeRec; x < sizeMax*sizeRec; x += sizeRec)
//    {	
//        // Get integers from the char buffer
//        int* temp = (int*) buffer;              // Your current buffer
//        aRid.pid = *(temp + x/sizeof(int));        // Get first int
//        aRid.sid = *(temp + x/sizeof(int) + 1);    // 2nd int
//        aKey = *(temp + x/sizeof(int) + 2);        // 3rd int
//        
//	    // Copy entires into sibling
//	    sibling.insert(aKey, aRid);
//        
//	    // Assign sibling key
//	    if (x == lastPid*sizeTot)
//	        siblingKey = aKey;
//        
//		// Create an empty buffer (for the last)        
//        char tempBuf[sizeTot];
//        tempBuf[0] = bRid.pid;
//        tempBuf[1*sizeof(int)] = bRid.sid;
//        tempBuf[2*sizeof(int)] = bKey;
//        
//		// Clear entry
//        for (int y = x; y < (x + sizeTot); y++)
//            buffer[y] = tempBuf[y-x];
//    }
    
	// Get the current pageID from the current node
	PageId temp = getNextNodePtr();
	
	// Set the current pageID to point to our new sibing node
	// Assuming the nextNodePtrs point to the next pageid
	setNextNodePtr(sibling.getNextNodePtr()); //is this right

    // Set the next pointer for the sibling
    sibling.setNextNodePtr(temp);
    
    // Insert record depending on the key
	if (key >= siblingKey)
		sibling.insert(key,rid);
	else
		insert(key,rid);
		
    return 0; // Success
	
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
    /* General idea:
     *  Get the number of keys
     *  Iterate through the nodes 
     *  Search the character buffer until you hit your searchKey
     *  If found, set the @param eid to that iteration
     *  If not found, return error.
     */
    
	char* pointer = &buffer[0] + sizeof(int) + sizeof(RecordId);	//Pointing to the first element, skipping the count
	int tempStorage = 0;
	int numKey = getKeyCount();

	for (int x=0; x < numKey; x++)
	{
		//Copy it to temp storage for comparison
		memcpy(&tempStorage, pointer, sizeof(int));
		
		if(tempStorage >= searchKey)
		{
			eid = x;	//Found it!
			return 0;
		}
		else
			// Maybe : pointer = sizeof(int) + x*sizeRec; if this doesn't work
			pointer += sizeRec;	// Goes to the next record/key pair and compare again
	}
	
	// If we still don't find it at this point
	return RC_NO_SUCH_RECORD;

//    int numKey = getKeyCount();
//    int key;
//    RecordId rid;
//    
//    // Go through the number of keys
//    for (int x = 0; x < numKey; x++){
//		int temp = 0;
//		//memcpy(dest,src,size);
//		memcpy 
//
//        // Get integers from the char buffer
//        int* temp = (int*) buffer;              // Your current buffer
//        rid.pid = *(temp + (x*sizeTot)/sizeof(int));        // Get first int
//        rid.sid = *(temp + (x*sizeTot)/sizeof(int) + 1);    // 2nd int
//        key = *(temp + (x*sizeTot)/sizeof(int) + 2);        // 3rd int
//        
//        if (key >= searchKey){
//            eid = x;        // Which entry it is
//            return 0;       // Success!
//        }
//    }
//    
//    // Unable to locate
//    return RC_NO_SUCH_RECORD;
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
    /* General idea:
     *  Make sure you're reading a valid key
     *  Read that part of the buffer
     */
    
    // Check valid eid to read
    if (eid >= getKeyCount() || eid < 0)
        return RC_INVALID_CURSOR;
    
    // Get integers from the char buffer
	memcpy(&key, buffer+sizeCount+eid*sizeRec+sizeof(RecordId), sizeof(key));
	memcpy(&rid, buffer+sizeCount+eid*sizeRec, sizeof(RecordId));
	
//    int* temp = (int*) buffer;              // Your current buffer
//    rid.pid = *(temp + (eid*sizeTot)/sizeof(int));      // Offset of x/4
//    rid.sid = *(temp + (eid*sizeTot)/sizeof(int) + 1);  // Offset of x/4 + 1
//    key = *(temp + (eid*sizeTot)/sizeof(int) + 2);      // Offset of x/4 + 2
    
    return 0; // Success
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{
    /* General idea:
     *  Get the next pointer
     */
    char* pointer = &buffer[0] + sizeCount + sizeTot;
	//Set the pointer to point to the pageID at the end of the leaf
	
	PageId nextPage;
	memcpy(&nextPage, pointer, sizeof(PageId));
	//Copy that pageId to nextPage and return it
	return nextPage;
    
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
    /* General idea:
     *  Set the next pointer
     */
    
    char* pointer = &buffer[0] + sizeCount + sizeTot;
	//Set the pointer to point to the pageID element in the leaf
	memcpy(pointer, &pid, sizeof(PageId));
	//Copy the input pid to that location
	return 0;
	
	// Error checking
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
	sizeRec = sizeof(NonLeaf);                           // Total Size
	sizeTot = getKeyCount() * sizeRec;                                  //Size of each record
    sizeMax = (PageFile::PAGE_SIZE - sizeof(PageId) - sizeof(int))/sizeof(NonLeaf);   // Max # of NonLeaf nodes
    sizeCount = sizeof(int);
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
//	// [Done. Checked].
//	//Copied from LeafNode::getKeyCount

	int tempStorage = 0;	
	memcpy(&tempStorage, &buffer, sizeCount);	// First 4 bytes of page is count info
	return tempStorage;		// Number of keys
    
//    int countKeys = 0;  // number of keys
//    
//    char tempBuf[4]; 
//    // copy first 4 bytes
//    for (int x = 0; x < 4; x++)
//        tempBuf[x] = buffer[x];
//    PageId firstPid = (PageId)tempBuf;                              // PID at the beginning
//    NonLeaf* myNonLeaf = (NonLeaf*)(buffer + sizeof(PageId));    // Start from the first key
//    int lim = PageFile::PAGE_SIZE - sizeof(PageId);                 // max number
//    
//    if (firstPid == EMPTY)
//        return 0;
//    
//    // Go through the nonleafnode
//    for (int x = 0; x < lim; x++){
//        countKeys++;    // count keys
//        
//        if(myNonLeaf->next == EMPTY)
//            break;
//        
//        myNonLeaf += sizeof(NonLeaf);    // go to the next PId
//    }
//    
//    return countKeys;   // return number of keys
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{
    /* General idea (from previous insert.. not all of this applies):
     *  Get the number of keys starting with the current node
     *  Try to look for the key, if found, good. If not, set the temp to the end.
     *  From the last leaf node, shift the buffer over by 1 entry until your location
     *  Set that location to the pid and sid from @param rid 
     *      - FIXME: Writing an int into a char buffer can cause problems, I think? Jon.
     *  Write into the buffer.
     */ 
    
    const int numKey = getKeyCount(); // Number of keys left
    
    // Return if it's full
    if (numKey >= sizeMax)
        return RC_NODE_FULL;
    
    int countKeys = 0;
    int tempId = 0;
    int tempKey = 0;
    int lim = PageFile::PAGE_SIZE - sizeof(PageId);              // max number
    
    NonLeaf* myNonLeaf = (NonLeaf*)(buffer + sizeCount);    // Start from the first key

    // Go through the nonleafnode
    for (int x = 0; x < lim; x++){
        
        if (myNonLeaf->next == EMPTY)
            break;
        
        myNonLeaf += sizeof(NonLeaf);    // go to the next Pid
        countKeys++;
    }

    if (myNonLeaf->next != EMPTY){ 
        // if it's not empty, shift it
        for (int x = sizeMax - countKeys - 1; x > 0; x--){
            NonLeaf* last = (NonLeaf*)(buffer + x);
            NonLeaf* curr = (NonLeaf*)(buffer + x-1);
            
            // Copy values
            last->key = curr->key;
            last->next = curr->next;
        }
    }
    
    // Finally, insert
    myNonLeaf->key = key;
    myNonLeaf->next = pid;

	//Set new count and store it
	int newCount = getKeyCount() + 1;
	// Set the count here
	memcpy(buffer, &newCount, sizeof(int));
    
	return 0; // Success!
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
{ 
    
    /* General idea (from previous insertAndSplit.. not all of this applies):
     *  Check to see if there's too many nodes.
     *  If it is, return, if not:
     *  Get the Id of the last entry
     *  Start from the last entry of the current node to the very end:
     *  Read from the character buffer and copy the entry into sibling
     *  "Erase" the current entry with the empty one created.
     *  Set the sibling's next pointer
     *  Insert the record
     */
    
    int numKey = getKeyCount();
    
    // Check size
    if (numKey < sizeMax)
        return RC_NODE_FULL; // is this correct?

	// Check if sibling is empty before it's called
	if (sibling.getKeyCount() > 0)
		return RC_NODE_FULL; 
    
    int mid = getKeyCount() / 2; // Takes the floor
    
    NonLeaf* myNonLeaf = (NonLeaf*)(buffer + sizeof(PageId));    // Start from the first key
    NonLeaf* midNonLeaf = myNonLeaf + mid;      // Increment by mid * sizeof(NonLeaf)
    
    // Set the middle key
    midKey = midNonLeaf -> key;
    
    // Initialize the sibling as empty
    sibling.initializeRoot(midNonLeaf->next, EMPTY, EMPTY);
    
    // Starting from the middle until the end
    for (int x = mid; x < numKey; x++){
        int currKey = (myNonLeaf+x)->key;
        int currPid = (myNonLeaf+x)->next;
        
        // Insert and check
        if(sibling.insert(currKey, currPid))
            // If unable to insert
            return RC_NO_SUCH_RECORD; // Not sure what error to return
        
        (myNonLeaf+x)->key = EMPTY;
        (myNonLeaf+x)->next = EMPTY;
    }
    
    // Clear middle
    (myNonLeaf+mid)->key = EMPTY;
    (myNonLeaf+mid)->next = EMPTY;
    
    // Finally, insert
	if (key >= midKey)
		sibling.insert(key,pid);
	else
		insert(key,pid);
    
    return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
    // FIXME: Maybe needs error checking

    NonLeaf* myNonLeaf = (NonLeaf*)(buffer + sizeof(PageId));    // Start from the first key
    int numKey = getKeyCount();
    
    int x;
    for (x = 0; x < numKey; x++){
        
        // Keep looking until you find a key greater than your searchkey
        if ((myNonLeaf+x)->key > searchKey){
            if (x == 0)
                // If it's the first, we send the beginning pid
                memcpy(&pid, buffer, sizeof(int));
            else
                pid = (myNonLeaf+x-1)->next;    // Go back one and return the pid
            
            return 0; // Success!
        }
    }
    
    // If it doesn't find it, return the last PId
    pid = (myNonLeaf+x-1)->next;
    
    return 0;

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
    NonLeaf* myNonLeaf = (NonLeaf*)(buffer + sizeof(PageId));    // Start from the first key
    
    for (int x = 0; x < sizeMax; x++){
        // Empty it out
        (myNonLeaf+x)->key = EMPTY;
        (myNonLeaf+x)->next = EMPTY;
    }
    
    myNonLeaf->key = key;
    myNonLeaf->next = pid2;
	memcpy(&pid1, buffer, sizeof(PageId));
	
//    PageId* temp = (PageId*)buffer; // Typecast the first pid from the buffer
//    *temp = pid1;                   // Write pid1 to the beginning of the buffer
	
	return 0;

 }
