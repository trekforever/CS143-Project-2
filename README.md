CS143-Project-2
===============

May 16 2012 By Xin Wei
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
