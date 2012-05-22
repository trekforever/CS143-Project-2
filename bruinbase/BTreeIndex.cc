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

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
	height = 0;	//initially 0 count. Needed to know whether reached leaf level
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
	memcpy(&height,pointer,sizeof(int));		//Copy height to the beginnning
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
		memcpy(pointer,&height,sizeof(int));
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
    return 0;
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
    return 0;
}
