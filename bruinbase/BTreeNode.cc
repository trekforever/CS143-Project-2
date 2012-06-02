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
    
    sizeRec = sizeof(RecordId) + sizeof(int);  
    sizeCount = sizeof(int);
    sizeMax = (PageFile::PAGE_SIZE - sizeof(PageId) - sizeof(int)) / sizeRec;     // Maximum # of rid/key pairs    
    
	//cout << "Total Size: " << sizeTot << endl;
	//cout << "Current Key Count: " << getKeyCount() << endl;

	/*
	*	Example of Buffer Layout
    *   First 4 bytes:
	*	[sizeof(int)]	[sizeof(PageId)]	[sizeRec]          [sizeof(int)]
	*	[Key Count]     [First PageId]		[First RecordId]	[key]
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
    // [Done. Checked.]
	// PsuedoCode:
	// Each of our key is an int, so we can use sizeof(int) to determine
	// the number of bytes. Then couldn't we just get that number of bytes
	// from the buffer array, and shouldn't that just be the # of records?
	// Note that the first four bytes stores the # of records of a page

	int tempStorage = 0;	
	//memcpy(destination,source,size)
	memcpy(&tempStorage, &buffer, sizeCount);	//get the first 4 bytes, store it into tempStorage
	return tempStorage;

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
    
    // [Done. Should be good]
	sizeTot = getKeyCount()*sizeRec;                    // Total Size of a leafnode

    const int numKey = getKeyCount(); // Number of keys
    
    if (numKey >= sizeMax)
        return RC_NODE_FULL;    // Return if it's full
    
    int eid = 0;
    int temp;
    
    // Locate spot to insert using locate()    
    // If unable to locate a spot to open, add to the end
    if (locate(key, eid)){ // Will return 0 if it does locate, 0 if doesn't
        eid = numKey;
    }
    
    // Starting from the last leaf, shift the character buffer over by 1 entry
	// Possible ERROR here:
    for (int x = numKey; x >= eid; x--){
        int curOff = sizeCount + x*sizeRec;
        int oldOff = sizeCount + (x-1)*sizeRec;
        memcpy(buffer+curOff, buffer+oldOff, sizeRec);
    }
    
    // Insert the new entry
	int offSet = sizeof(int) + eid * sizeRec;
    int newOffSet = offSet + sizeof(RecordId);
//    printf("eid: %d, key: %d, offset: %d, newoffset: %d\n", eid, key, offSet, newOffSet);
    
	memcpy(&buffer[0]+offSet, &rid, sizeof(RecordId));
	memcpy(&buffer[0]+newOffSet, &key, sizeof(int));
    
    memcpy(&temp, buffer+newOffSet, sizeof(int));

	//Set new count and store it
	int newCount = numKey + 1;
	// Set the count at the beginning of the buffer
	memcpy(&buffer[0], &newCount ,sizeof(int));
    
//    printf("count: %d\n", newCount);

//    printf("Number of keys: %d\n", numKey+1);

//    printf("Key 1: %d\n", temp);
//    if (numKey > 30)
//    printf("Key added: %d\n", temp);
//    for (int x = 0; x < numKey+1; x++){    
//        memcpy(&temp, buffer+sizeof(int)+x*sizeRec+sizeof(RecordId), sizeof(int));
//        printf("Key %d: %d\n", x, temp);
//    }
//    printf("\n");
    
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
    
    // [Done. Should work.]
	sizeTot = getKeyCount()*sizeRec;                    // Total Size of a leafnode
    
    // Check size, we shouldn't be splitting here
	int numKey = getKeyCount();
    if (numKey < sizeMax){
        return RC_NODE_FULL; 
    }

	// Check if sibling is empty before it's called
	if (sibling.getKeyCount() > 0){
		return RC_NODE_FULL; 
    }
	
    // Id of last entry to split
    int lastPid = sizeMax / 2;
	int keysToCopy = numKey - lastPid;
    
//    printf("lastpid: %d, keystocopy: %d\n", lastPid, keysToCopy);

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
		
//        printf("Inserted: %d into sibling!\n", tempKey);
        
		// Insert it into the sibling
		sibling.insert(tempKey, tempRid);
            
        
		memcpy(buffer+sizeCount+(lastPid+x)*sizeRec, &bRid, sizeof(RecordId));
		memcpy(buffer+sizeCount+(lastPid+x)*sizeRec+sizeof(RecordId), &bKey, sizeof(int));
	}
    
    // Edit the count of the old buffer
    memcpy(buffer, &lastPid, sizeCount);
    
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
		
//    printf("\n");
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
    // [Done. Tested]
	char* pointer = &buffer[0] + sizeof(int) + sizeof(RecordId);	//Pointing to the first element, skipping the count
	int curKey = 0; // Current stored key
	int numKey = getKeyCount();
    
	for (int x=0; x < numKey; x++)
	{
		//Copy it to temp storage for comparison
		memcpy(&curKey, pointer, sizeof(int));
		if(curKey >= searchKey)
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
    
    return 0; // Success
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{
    /* General idea:
     *  Get the next pointer (last 4 bytes)
     */
    
    sizeTot = sizeMax*sizeRec;
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
     *  Set the next pointer (last 4 bytes)
     */
    
    sizeTot = sizeMax*sizeRec;
    
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
    sizeCount = sizeof(int);
	sizeRec = sizeof(PageId) + sizeof(int);                           // Total Size
    sizeMax = (PageFile::PAGE_SIZE - sizeof(PageId) - sizeof(int))/sizeRec - 1;   // Max # of NonLeaf nodes
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
    
    //[Done. Should be okay]
	//cout << "Key to Insert: " << key << endl;
	//Similar to the BTLeafNode::insert (to an similar extent) 
	//So some codes are copied from that function respectively
    
	char* pointer = &buffer[0]+sizeof(PageId)+ sizeof(int);	//First page
    const int numKey = getKeyCount(); // Number of keys 
    
	//Want to iterate through the pages until we find the one to insert beforehands
	int eid = 0;    // Location to insert the key
	int tempStorage = 0;	//Storage comparison
    
    // Return if it's full
    if (numKey >= sizeMax){
        return RC_NODE_FULL;
    }
    
    // Start with the first key
	if (pointer!=NULL)
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison. Initial.
    
    // Search for location to insert key
	while(pointer!=NULL && key > tempStorage)	//Find the key that is right before our insertion point
	{
		memcpy(&tempStorage, pointer, sizeof(int));	//Copy it to temp storage for comparison
		pointer += sizeRec;	//Goes to the next record
        
		eid++;
        
		//Should exit once tempStorage >= Key. Do we need to check when tempStorage = 0? Let's just do it just incase.
		if(tempStorage==0)
			break;	//Needs testing and checking
	}
    
    // Shift over so you can insert
    for (int x = numKey; x >= eid; x--){
        // offset by count + 1st pageid + key/Pageid pair
        int curOff = sizeCount*2 + x*sizeRec;
        int oldOff = sizeCount*2 + (x-1)*sizeRec;
        memcpy(buffer+curOff, buffer+oldOff, sizeRec);
    }
    
    // Need to offset because of the whileloop
    if (eid != 0)
        eid--;
    
    int offSet = sizeof(int)*2 + eid * sizeRec; // location to insert key
    int newOffSet = offSet + sizeof(int); // location to insert pid
    
//    printf("NONLEAF: eid: %d, key: %d, offset: %d, newoffset: %d\n", eid, key, offSet, newOffSet);
    
    memcpy(&buffer[0]+offSet, &key, sizeof(int));
	memcpy(&buffer[0]+newOffSet, &pid, sizeof(PageId));
    
	//Set new count and store it
	pointer = &buffer[0];	//Goes back to the beginning
	int newCount = getKeyCount() + 1;	//Increment count
	memcpy(pointer,&newCount,sizeof(int));
	
//    // DEBUG
//    int temp;
//    PageId tPid;
//    memcpy(&tPid, buffer+sizeof(int), sizeof(int));
//    printf("NONLEAF: First pid: %d\n", tPid);
//    for (int x = 0; x < numKey+1; x++){    
//        memcpy(&temp, buffer+sizeof(int)+sizeof(PageId)+x*sizeRec, sizeof(int));
//        memcpy(&tPid, buffer+sizeof(int)+(x+1)*sizeRec, sizeof(int));
//        printf("NONLEAF: Key %d: %d, pid: %d\n", x, temp, tPid);
//    }
//    printf("\n");
    
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
	sizeTot = getKeyCount() * sizeRec;                                  //Size of each record
    
    int numKey = getKeyCount();
    
    // Check size, we shouldn't be splitting here
    if (numKey < sizeMax){
//        printf("Why am I here 1?\n");
        return RC_NODE_FULL; 
    }
    
	// Check if sibling is empty before it's called
	if (sibling.getKeyCount() > 0){
//        printf("Why am I here 2?\n");
		return RC_NODE_FULL; 
    }
    
    int lastPid = sizeMax / 2; // Number of nodes left in original node after split
    int keysToCopy = numKey - lastPid;
//    printf("NONLEAF: lastpid: %d, keystocopy: %d\n", lastPid, keysToCopy);
    
    PageId bPid = EMPTY;
    int bKey = EMPTY;
    
    // Get the middle pid, and initialize the sibling.
    PageId midPid;
    memcpy(&midPid, buffer+sizeCount+lastPid*sizeRec, sizeof(PageId));
    
    // For intializing, create a new BTNonLeafNode with the corresponding pid's and key
    // This should be from the middle pointer
    int tempKey;
    PageId tempPid;
    memcpy(&tempKey, buffer+sizeCount*2+lastPid*sizeRec, sizeof(int));
    memcpy(&tempPid, buffer+sizeCount*2+lastPid*sizeRec+sizeof(int), sizeof(PageId));
//    printf("Initialize tempKey: %d, tempPid: %d\n", tempKey, tempPid);
    sibling.initializeRoot(midPid, tempKey, tempPid);
    
    memcpy(&midKey, buffer+sizeCount*2+lastPid*sizeRec, sizeof(int));
//    printf("NONLEAF: midkey: %d\n", midKey);
    
    // Copy from old buffer into sibling.
    for (int x = 1; x < keysToCopy; x++){
		// Get the key startin from middle
		memcpy(&tempKey, buffer+sizeCount*2+(lastPid+x)*sizeRec, sizeof(int));
        memcpy(&tempPid, buffer+sizeCount*2+(lastPid+x)*sizeRec+sizeof(int), sizeof(PageId));
		
//        printf("Still splitting nonleaf...!\n");
//        printf("NONLEAF: Inserted: %d into sibling!\n", tempKey);
        
		// Insert it into the sibling
		sibling.insert(tempKey, tempPid);
        
        
		memcpy(buffer+sizeCount*2+(lastPid+x)*sizeRec, &bKey, sizeof(int));
        memcpy(buffer+sizeCount*2+(lastPid+x)*sizeRec+sizeof(int), &bPid, sizeof(PageId));
	}
    
    // Edit the count of the old buffer
    memcpy(buffer, &lastPid, sizeCount);
    
    // Finally, insert into either sibling or original
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
    // [Done. Should work]
    int offSet = sizeof(int)*2; // location to insert key
    
    char* pointer = &buffer[0]+sizeof(int)+sizeof(PageId);	//First key
    
	int compareKey = 0;	//storage for comparison
    int numKey = getKeyCount();
    
    //copy first key to compareKey for comparison. initial
	memcpy(&compareKey, buffer+offSet, sizeof(int));
//    printf("Comparekey: %d\n", compareKey);
    
    // Keep searching for the child pid
    for (int x = 0; x < numKey; x++){
        if (searchKey < compareKey){
            memcpy(&pid, pointer-sizeof(PageId), sizeof(PageId));
//            printf("NONLEAF PID FOUND: using pid: %d\n", pid);
            return 0;
        }
        // If search key >= compareKey
        pointer += sizeRec;
        memcpy(&compareKey, pointer, sizeof(int));
    }
//    printf("NONLEAF PID NOT FOUND: using pid: %d\n", pid);
    memcpy(&pid, pointer-sizeof(PageId), sizeof(int));
    
    
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
    
	char* pointer = &buffer[0] + sizeof(int);	//Pointing to the 1st element
    
	memcpy(pointer, &pid1, sizeof(PageId));	//Initialize first pageId
	pointer += sizeof(PageId);	// Increment by that amount
    
	memcpy(pointer, &key, sizeof(int));	//Initialize the key
	pointer += sizeof(int);	//Increment by that amount again
    
	memcpy(pointer, &pid2, sizeof(PageId));	//Create the 2nd PageId to insert behind the key
//    printf("ROOT: pid1: %d, key: %d, pid2: %d\n", pid1, key, pid2);
    
	pointer = &buffer[0];	//Modify Count
    int count = 1;
	memcpy(pointer,&count,sizeof(int));	//Incrementing count
    
    for (int x = 0; x < 1; x++){
        int temp;
        memcpy(&temp, buffer+sizeof(int)*2+x*sizeRec, sizeof(int));
//        printf("Root key %d: %d\n", x, temp);
    }
    
    // DEBUG
//    printf("\n");
	
	return 0;

 }

