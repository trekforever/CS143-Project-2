CS143-Project-2
===============
May 31, 2012 by Jonathan Nguy
 - Seems like everything works with other SqlEngine.cc (xlarge not tested, which is height of 3)
 - WHERE works with things such a =, <, >, <>. 
 	- Probably have to test other cases.
 - ALSO: TAKE OUT printf's USED FOR DEBUGGING.
 
May 30, 2012 by Jonathan Nguy
 - Fixed a bug in load() with "WITH INDEX", which was seg faulting when testing.
 - Completely redefined BTreeNode.cc, nodes are correct.
 	- Should've tested this sooner :'(
 - Implemented BTreeIndex.cc's insert() function. (Not sure if > 2 height works.)
 	- Only "xlarge.del" creates a tree with height > 2.
 	- xsmall, small, medium, large all work nicely, I believe.
 - Added test cases
 - BTreeNode.cc still has all my debugging outputs.
 - To test, run: ./clean.sh; ./temp.sh
 	- Edit the temp.sql file if you need to.
 
May 29, 2012 by Xin Wei
 - Implemented select + WITH INDEX option

May 28, 2012 by Jonathan Nguy
 - Tried to implement insert
	- Though, I didn't use the double link lists (kind of unsure what I could've done)
 - Have NOT tested, but compiles
 - QUESTION: Does locate use the functions from BTreeNode.cc?
	- Are we supposed to? I think that would make the most sense.
	
May 26, 2012 by Jonathan Nguy
 - Reworked typecasting into memcpy's
 - Read through code to make sure it logically made sense
 	- Most/All look good

May 21, 2012 by Jonathan Nguy
 - Retried to implement with other data structures.
 - Implemented all functions with said structures, including:
 	- BTLeafNode::getKeyCount()
 	- BTLeafNode::insert
 	- BTLeafNode::insertAndSplit
 	- BTLeafNode::locate
 	- BTLeafNode::readEntry
 	- BTLeafNode::getNextNodePtr() // unsure
 	- BTLeafNode::setNextNodePtr // unsure
 	
 	- BTNonLeafNode::getKeyCount()
 	- BTNonLeafNode::insert
 	- BTNonLeafNode::insertAndSplit
 	- BTNonLeafNode::locateChildPtr
 	- BTNonLeafNode::initializeRoot

 - Still need to fix possible errors
 - Compiles correctly
 - Added private struct NonLeaf (contains a key and a pointer to the next pid).
 	- Design is similar to the picture on the assignment page
 - Added private variable sizeMax, which is
 	- Maximum leaf entries (for leafNode)
 	- Maximum nonLeaf structs.
 
May 21, 2012 By Xin Wei
 - Fixed memory errors and typos
 - Implemented BTNonLeafNode::Insert
 - Started on BTreeIndex.h
	- Added char bufferArr[PageFile::PAGE_SIZE] for buffer in Header (global variable, makes life easier)
	- Added int height; to keep track of height (needed to determine for leafNode)
	- Added char method to keep track of mode (needed for close)
 - For BTreeIndex.cc
	- Implemented the Open Method
	- Implemented the Close Method

May 19, 2012 By Xin Wei
 - Fixed a few bugs and typos.
 - Implemented BTLeafNode::insert (ohy took me hours!)
 - Implemented BTNonLeafNode::GetKeyCount()
 - Implemented BTNonLeafNode::locatechildptr

May 16 2012 By Xin Wei
 - Added psuedocode for BtLeafNode::insert
 - Added constructor declaration to BTreeNode.h for both BTLeafNode and BtNonLeafNode
 - Added private members sizeRec and sizeTot (for the size of each record/slot, and the total size)
	to both BtLeafNode and BtNonLeafNode
 - Implemented the following functions:
 	- BTLeafNode::BTLeafNode()
	- BtLeafNode::Read
	- BtLeafNode::Write
	- BtLeafNode::GetKeyCount
		// Not 100% sure about my implementation
	- BtLeafNode::locate
	- BtLeafNode::readEntry
	- BtLeafNode::getNextNodePtr
	- BtNonLeafNode::BTNonLeafNode()
	- BtNonLeafNode::read
	- BtNonLeafNode::write
	- BtNonLeafNode::initializeRoot
		//Not 100% sure about my implementation


 - Still need to implement
	- BTLeafNode::insert
	- BtLeafNode::insertAndSplit
	- BtLeafNode::setNextNodePtr
	- BtNonLeafNode::getKeyCount	
		// If the above BtLeafNode::getKeyCount implementation is right, can just copy and paste
	- BtNonLeafNode::insert
	- BtNonLeafNode::insertAndSplit
	- BtNonLeafNode::loocateChildPtr
		// Can copy and paste from BtLeafNode::locate and modify
