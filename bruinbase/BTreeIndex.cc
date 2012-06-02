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
#include <iostream>
const int GLOBALpsize = PageFile::PAGE_SIZE;
using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = EMPTY;
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
    BTLeafNode leaf; 
    int newKey;
	RC rc;

    // If empty tree
    if (treeHeight == 0) {
        
        // Insert to the leaf
        leaf.insert(key, rid);
//        printf("Inserted key: %d \n", key);
        
        // This is for hte return value
        rootPid = pf.endPid();
        if (rootPid == 0)
            rootPid++;
        
        // Increase tree height
        treeHeight++;
        
        // Write the rootPid
//        printf("rootPid = %d", rootPid);
        rc = leaf.write(rootPid, pf);
        
    } else {
        
        rc = insertHelp(key, rid, rootPid, newKey, newPid, 1);

    }

    return rc; // Success
}

RC BTreeIndex::insertHelp(int key, const RecordId& rid, PageId currPid, int& nKey, PageId& nPid, int lvl)
{
    BTNonLeafNode myRoot;
//    myRoot.read(rootPid, pf);
                
    // BASE CASE: leaf node
    if (lvl == treeHeight)
    {
        // Read the node
        BTLeafNode ln;
        ln.read(currPid, pf); 
        
        // Insert
        RC rc = ln.insert(key, rid);
        
        // Successful insert (node not full)
        if (rc == 0)
        {
            // Write to pagefile
            ln.write(currPid, pf);
            
            // DEBUG 
            //printf("BTreeIndex.insert: Inserted key = %d at pid = %d. TreeHeight = %d. rc = %d.\n", key, currPid, treeHeight, rc);
            
            return rc;
        } else // Node full
        {
            // Create a sibling node
            BTLeafNode sn;
            
            // Return key from insertAndSplit            
            int sibKey;
            
//            printf("SPLITTED!\n");
            
            // Insert and split
            ln.insertAndSplit(key, rid, sn, sibKey);
            
            // Write the sibling node
            PageId sibPid = pf.endPid();
            sn.write(sibPid, pf);	  
            
            // Set next pointer after split
            ln.setNextNodePtr(sibPid);
            
            // Write the leaf node
            ln.write(currPid, pf);
            
            // Set return
            nPid = sibPid;
            nKey = sibKey;
            
            // If we're at the root, create a new root
            if (lvl == 1)
            {
                rc = myRoot.initializeRoot(currPid, sibKey, sibPid);
                        
                // Set the new root pid
                rootPid = pf.endPid();
                
                // Write the new root
                myRoot.write(rootPid, pf);
                
                // Set the new tree height
                treeHeight++;
                
//                printf ("ROOT INFO: currPid: %d, sibKey: %d, sibPid: %d, rootPid: %d\n", currPid, sibKey, sibPid, rootPid);
            }     
            
            return rc; // Inserted and splitted
            
        }
        
    } else // RECURSIVE CASE: Non leaf node, need to traverse
    {
//        printf("Something new to insert in pid %d\n", currPid);
        
        // Initialize and read
        BTNonLeafNode n;
        
        // If it's in a non leaf/ non root node.
        if (lvl == 1)
            myRoot.read(rootPid, pf);
        else
            n.read(currPid, pf);
        
        PageId childPid, splitPid;
        int splitKey;
        
        if (lvl == 1) // rootNode
            myRoot.locateChildPtr(key, childPid);
        else { // nonleaf, nonroot node
            n.locateChildPtr(key, childPid);
//            printf("Located child pointer for nonleaf, nonroot: %d\n", childPid);
//            printf("Current level: %d, current height: %d\n", level, treeHeight);
        }
        
        // Recurse through tree
        RC rc = this->insertHelp(key, rid, childPid, splitKey, splitPid, lvl+1);
        
        // This means we had to split the node below
        if (rc == RC_NODE_FULL)
        {
//            printf("WOO INSERTED AND SPLITTED!\n");
//            printf("splitKey: %d, splitPid: %d\n", splitKey, splitPid);
            
            // Insert into the non leaf after split of leaf
            if (lvl == 1) // if root node, insert into root
                rc = myRoot.insert(splitKey, splitPid);
            else // if nonleaf, nonroot, insert into that node
                rc = n.insert(splitKey, splitPid);
            
            // Success
            if (rc == 0)
            {
                // Write to either the root or the nonleaf, non-root
                if (lvl == 1)
                    myRoot.write(currPid, pf);
                else 
                    n.write(currPid, pf);
                return rc;
                
            } else // If nonleaf is full, insert and split.
            {
//                printf("I shouldn't be here. \n");
                
//                RC rc = RC_NODE_FULL;
                // Create sibling
                BTNonLeafNode sn;
                
                // Get midkey from insertandsplit of non leaf node
                int midKey;
                
//                printf("Current level after nonleaf node split: %d\n", lvl);
                if (lvl == 1) // split root
                    myRoot.insertAndSplit(splitKey, splitPid, sn, midKey);
                else { // split nonleaf
//                    printf("WHY U NO WORKZ!\n");
                    n.insertAndSplit(splitKey, splitPid, sn, midKey);
                }
                    
//                printf("Inserted and splitted root\n");
                
                // Write the sibling to sibId
                PageId sibId = pf.endPid();
                sn.write(sibId, pf);		
                
                if (lvl == 1)
                    myRoot.write(currPid, pf);
                else
                // Write the node that was split
                    n.write(currPid, pf);
                
                // If we're at the root, create a new root
                if (lvl == 1)
                {
                    rc = myRoot.initializeRoot(currPid, midKey, sibId);
                    
                    // Set the new root pid
                    rootPid = pf.endPid();
                    
                    // Write the new root
                    myRoot.write(rootPid, pf);
                    
                    // Set the new tree height
                    treeHeight++;
                    
                    // DEBUG 
//                printf ("ROOT INFO: currPid: %d, sibKey: %d, sibPid: %d, rootPid: %d\n", currPid, sibKey, sibPid, rootPid);
                }
                
                // Set return info
                nPid = sibId;
                nKey = midKey;
                
                return rc;
            }
        } else 
            return rc;
        return rc;
    } 
    
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
	//Base case is when height is 0 or 1
    if(treeHeight==0)
	{
		rootPid = 1;	//set rootpid
		cursor.pid = 1;	//the cursor pid will be 1
		cursor.eid = 0; //the index for cusor will be 0 because it is the root
		treeHeight++;
	}
    
    PageId tempPid = rootPid;
    
    // Iterate until you reach leaf node
    for (int x = 1; x < treeHeight; x++){
        BTNonLeafNode nln;
        if (nln.read(tempPid, pf) != 0)
            return RC_FILE_READ_FAILED;
        
        if (nln.locateChildPtr(searchKey, tempPid) != 0)
            return RC_INVALID_CURSOR;
    }
    
    int searchEid = 0;
    BTLeafNode temp;
    // read the tempPid
    RC rc = temp.read(tempPid, pf);

    if(rc != 0)
        return RC_FILE_READ_FAILED;	//Unable to read
    
    if(temp.locate(searchKey, searchEid) != 0)
        return RC_NO_SUCH_RECORD;	//Unable to find
    
//    printf("Located.. \n");
    //Found and stored in IndexCusor
    cursor.pid = tempPid;
    cursor.eid = searchEid;
//    printf("Returned!! \n");
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
