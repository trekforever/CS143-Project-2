CS143-Project-2
===============
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
