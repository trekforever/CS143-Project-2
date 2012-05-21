CS143-Project-2
===============

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
