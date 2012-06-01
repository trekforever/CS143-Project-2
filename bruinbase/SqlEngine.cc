/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

//RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
//{
//  RecordFile rf;   // RecordFile containing the table
//  RecordId   rid;  // record cursor for table scanning
//
//  RC     rc;
//  int    key;     
//  string value;
//  int    count;
//  int    diff;
//
//  // open the table file
//  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
//    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
//    return rc;
//  }
//
//  //Make use of With Indexing Option
//  BTreeIndex treeIndex;
//  int compareMe = 0;
//  //1 is key column
//  SelCond* NotEqual1 = NULL;
//  SelCond* Max1 = NULL;
//  SelCond* Equal1 = NULL;
//  SelCond* Min1 = NULL;
//  //2 is value column
//  SelCond* NotEqual2 = NULL;
//  SelCond* Max2 = NULL;
//  SelCond* Equal2 = NULL;
//  SelCond* Min2 = NULL;
//
//  for(int i=0; i < cond.size(); i++)
//  {
//	  //Can also use stringstream too
//	  compareMe = atoi(cond[i].value);
//	  //Iterate through all the members of the vector input
//	  if(cond[i].attr == 1)
//	  {
//		  //1 is Key Column
//		  if(cond[i].comp == SelCond::NE)
//		  {
//			  //Not Equal
//			  NotEqual1 = (SelCond*) &cond[i];
//		  }
//		  else if(cond[i].comp == SelCond::LE)
//		  {
//			  //Less Than or Equal To
//			  compareMe++;
//			  int bufferVal = atoi(Max1->value);
//			  if(Max1 == NULL || bufferVal < compareMe)
//			  {
//				  //Set new pointer
//				  Max1 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else if(cond[i].comp == SelCond::LT)
//		  {
//			  // Less THan
//			  int bufferVal = atoi(Max1->value);
//			  if(Max1 == NULL || bufferVal < compareMe)
//			  {
//				  //Set new pointer
//				  Max1 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else if(cond[i].comp == SelCond::EQ)
//		  {
//			  //Equal
//			  Equal1 = (SelCond*) &cond[i];
//		  }
//		  else if(cond[i].comp == SelCond::GE)
//		  {
//			  //Greater or Equal To
//			  compareMe--;
//			  int bufferVal = atoi(Min1->value);
//			  if(Min1 == NULL || bufferVal > compareMe)
//			  {
//				  //Set new pointer
//				  Min1 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else if(cond[i].comp == SelCond::GT)
//		  {
//			  int bufferVal = atoi(Min1->value);
//			  if(Min1 == NULL || bufferVal > compareMe)
//			  {
//				  //Set new pointer
//				  Min1 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else
//			  cerr << "Error: SqlEngine.cc. SHould never reach here" << endl;
//	  }
//	  else if(cond[i].attr == 2)
//	  {
//		  //We have a value column here
//		  // we do the same things again ...
//		  if(cond[i].comp == SelCond::NE)
//		  {
//			  //Not Equal
//			  NotEqual2 = (SelCond*) &cond[i];
//		  }
//		  else if(cond[i].comp == SelCond::LE)
//		  {
//			  //Less Than or Equal To
//			  compareMe++;
//			  int bufferVal = atoi(Max2->value);
//			  if(Max2 == NULL || bufferVal < compareMe)
//			  {
//				  //Set new pointer
//				  Max2 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else if(cond[i].comp == SelCond::LT)
//		  {
//			  // Less THan
//			  int bufferVal = atoi(Max2->value);
//			  if(Max2 == NULL || bufferVal < compareMe)
//			  {
//				  //Set new pointer
//				  Max2 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else if(cond[i].comp == SelCond::EQ)
//		  {
//			  //Equal
//			  Equal2 = (SelCond*) &cond[i];
//		  }
//		  else if(cond[i].comp == SelCond::GE)
//		  {
//			  //Greater or Equal To
//			  compareMe--;
//			  int bufferVal = atoi(Min2->value);
//			  if(Min2 == NULL || bufferVal > compareMe)
//			  {
//				  //Set new pointer
//				  Min2 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else if(cond[i].comp == SelCond::GT)
//		  {
//			  int bufferVal = atoi(Min2->value);
//			  if(Min2 == NULL || bufferVal > compareMe)
//			  {
//				  //Set new pointer
//				  Min2 = (SelCond*) &cond[i];
//			  }
//		  }
//		  else
//			  cerr << "Error: SqlEngine.cc. SHould never reach here" << endl;
//	  }
//	  else
//		  cerr << "Error: SqlEngine.cc. Should Never reach here" << endl;
//  }
//  int indexFlag = 0;
//  if(treeIndex.open(table+".idx",'r')==0)
//  {
//	//Success
//	  IndexCursor cursor;
//	  indexFlag = 1;
//	  
//	  //Lookup
//	  if(Equal1 || Min1 || Max1 || NotEqual1)
//	  {
//		  int target = 0;
//		  int lowerBound = 0;
//		  int upperBound = 0;
//		  int notkey = 0;
//		  //Key Value
//		  if(Equal1)
//			  target = atoi(Equal1->value);
//		  if(Min1)
//		  {
//			  lowerBound = atoi(Min1->value);
//			  target = lowerBound;
//		  }
//		  if(Max1)
//		  {
//			  upperBound = atoi(Max1->value);
//			  if(Min1 == NULL)
//			  {
//				  target = upperBound;
//			  }
//		  }
//		  if(NotEqual1)
//		  {
//			  notkey = atoi(NotEqual1->value);
//		  }
//
//		  //Error Checking
//		  if(Max1 && treeIndex.locate(0,cursor)!=0)
//		  {
//			  int returnvalue = treeIndex.locate(0,cursor);
//			  treeIndex.close();
//			  return returnvalue;
//		  }
//		  if(NotEqual1 == NULL && treeIndex.locate(target, cursor) != 0)
//		  {
//			   int returnvalue = treeIndex.locate(target, cursor);
//			   treeIndex.close();
//			   return returnvalue;
//		  }
//
//		  //Read Forward Now
//		  while(treeIndex.readForward(cursor,key,rid) == 0) //Sucessful
//		  {
//			  if(Max1 && Max1->comp == SelCond::LE)
//			  {
//				  if(target > upperBound)
//					  break;
//			  }
//			  else if(Max1 && Max1->comp != SelCond::LE)
//			  {
//				  if(target >= upperBound)
//					  break;
//			  }
//
//			  if(rf.read(rid,key,value) != 0)
//			  {
//				  //Error
//				  int returnvalue = rf.read(rid,key,value);
//				  treeIndex.close();
//				  return returnvalue;
//			  }
//
//			  count++;
//			  if(attr == 1) //Key
//				  cout << key << endl;
//			  else if(attr==2) //Value
//				  cout << value.c_str() << endl;
//			  else if(attr==3) //All
//				  cout << key << " " << value.c_str() << endl;
//			 
//			  if(Equal1 && key == target)
//				  //Found it
//				  break;
//			  if(NotEqual1 && key != notkey)
//				  //Still works
//				  break;
//		  }
//		  if(Equal1 == NULL || Min1 == NULL || Max1 == NULL) {
//			  treeIndex.close();
//			  indexFlag = 0;
//		  }
//		  else
//			treeIndex.close();
//	  }
//  }
//  if(indexFlag == 0)
//  {
//	  // scan the table file from the beginning
//	  rid.pid = rid.sid = 0;
//	  count = 0;
//	  while (rid < rf.endRid()) {
//		// read the tuple
//		if ((rc = rf.read(rid, key, value)) < 0) {
//		  fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
//		  goto exit_select;
//		}
//
//		// check the conditions on the tuple
//		int equal1val = atoi(Equal1->value);
//		int max1val = atoi(Max1->value);
//		int min1val = atoi(Min1->value);
//		int notequalval = atoi(NotEqual1->value);
//		if (Equal1 != NULL && (int) key != equal1val)	goto next_tuple;
//		if (Max1 != NULL && (int) key >= max1val)	goto next_tuple;
//		if (Min1 != NULL && (int) key <= min1val)	goto next_tuple;
//		if (NotEqual1 && (int) key == notequalval) goto next_tuple;
//
//		if (NotEqual2 != NULL && value != Equal2->value)		goto next_tuple;
//		if (Max2 != NULL &&  value >= Max2->value)	goto next_tuple;
//		if (Min2 != NULL && value <= Min2->value)	goto next_tuple;
//		if (NotEqual2 && value == NotEqual2->value) goto next_tuple;
//
//		// the condition is met for the tuple. 
//		// increase matching tuple counter
//		count++;
//
//		// print the tuple 
//		switch (attr) {
//		case 1:  // SELECT key
//		  fprintf(stdout, "%d\n", key);
//		  break;
//		case 2:  // SELECT value
//		  fprintf(stdout, "%s\n", value.c_str());
//		  break;
//		case 3:  // SELECT *
//		  fprintf(stdout, "%d '%s'\n", key, value.c_str());
//		  break;
//		}
//
//		// move to the next tuple
//		next_tuple:
//		++rid;
//	  }
//
//	  // print matching tuple count if "select count(*)"
//	  if (attr == 4) {
//		fprintf(stdout, "%d\n", count);
//	  }
//	  rc = 0;
//
//	  // close the table file and return
//	  exit_select:
//	  rf.close();
//	  return rc;
//  }
//}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
    RecordFile rf;   // RecordFile containing the table
    RecordId   rid;  // record cursor for table scanning
    
    RC     rc;
    int    key;     
    string value;
    int    count;
    int    diff;
    
    // open the table file
    if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
        fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
        return rc;
    }
    
    // scan the table file from the beginning
    rid.pid = rid.sid = 0;
    count = 0;
    
    // Try to open up the index
    BTreeIndex treeIndex;
    RC tempRc = treeIndex.open(table + ".idx", 'r');
    bool useIndex = false;
    
    if (!(tempRc == 0 && cond.size() != 0))
        useIndex = false;
        
    // Go through conditions to see if we should use an index.
    for(int x = 0; x < cond.size(); x++) {
        
        if(cond[x].attr == 2){ 
            useIndex = false;
            goto skip_cond;
        }

        switch (cond[x].comp) {
            case SelCond::NE:
                useIndex = false;
                goto skip_cond;
            case SelCond::GT:
            case SelCond::LT:
            case SelCond::GE:
            case SelCond::LE:
            case SelCond::EQ:
                useIndex = true;
                goto skip_cond;
        }
    }
skip_cond:
    
    // If we don't use index
    if( useIndex ) {
        
        while (rid < rf.endRid()) {
            // read the tuple
            if ((rc = rf.read(rid, key, value)) < 0) {
                fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
                goto exit_select;
            }
            
            // check the conditions on the tuple
            for (unsigned i = 0; i < cond.size(); i++) {
                // compute the difference between the tuple value and the condition value
                switch (cond[i].attr) {
                    case 1:
                        diff = key - atoi(cond[i].value);
                        break;
                    case 2:
                        diff = strcmp(value.c_str(), cond[i].value);
                        break;
                }
                
                // skip the tuple if any condition is not met
                switch (cond[i].comp) {
                    case SelCond::EQ:
                        if (diff != 0) goto next_tuple;
                        break;
                    case SelCond::NE:
                        if (diff == 0) goto next_tuple;
                        break;
                    case SelCond::GT:
                        if (diff <= 0) goto next_tuple;
                        break;
                    case SelCond::LT:
                        if (diff >= 0) goto next_tuple;
                        break;
                    case SelCond::GE:
                        if (diff < 0) goto next_tuple;
                        break;
                    case SelCond::LE:
                        if (diff > 0) goto next_tuple;
                        break;
                }
            }
            
            // the condition is met for the tuple. 
            // increase matching tuple counter
            count++;
            
            // print the tuple 
            switch (attr) {
                case 1:  // SELECT key
                    fprintf(stdout, "%d\n", key);
                    break;
                case 2:  // SELECT value
                    fprintf(stdout, "%s\n", value.c_str());
                    break;
                case 3:  // SELECT *
                    fprintf(stdout, "%d '%s'\n", key, value.c_str());
                    break;
            }
            
            // move to the next tuple
        next_tuple:
            ++rid;
        }
        
    }else {
        
        // We have an index, so let's use it!
        
        // DEBUG 
        //    printf("USING INDEX!\n");
        
        
        // keep a list of RecordIds to print out.
        vector<RecordId> to_print;
        RC read_status;
        RC search_status;
        
        // Figure out the conditions
        // Figure out which attribute we are looking for: 1 = key, 2 = value
        
        // check the conditions on the tuple
        for (unsigned i = 0; i < cond.size(); i++) {
            
            int compare_int_value = -1;
            char* compare_string_value;
            
            switch (cond[i].attr) {
                case 1:
                    compare_int_value =  atoi(cond[i].value);
                    compare_string_value = NULL;
                    break;
                case 2:
                    compare_string_value =  cond[i].value;
                    compare_int_value = -1;
                    break;
            }
            
            
            IndexCursor cursor;
            int key;
            RecordId rid;	
            RC smallest_search_status;
            IndexCursor startCursor;
            int smallestKey;
            
            switch (cond[i].comp) {
                    
                case SelCond::EQ:
                    // locate the correct tuple and then print it out
                    
                    // TODO: for now, if this is checking for value equality, then jus do it the old school way
                    if(cond[i].attr == 2) {
                        
                        
                    }
                    
                    
                    search_status = treeIndex.locate(compare_int_value, cursor);
                    
                    // If we couldn't find a match just keep going and forget about this condition
                    if (search_status != 0)
                        continue;
                    
                    
                    // Now read the record id, and add the tuple to the toPrint vector
                    
                    read_status = treeIndex.readForward(cursor, key, rid);
                    
                    if(read_status != 0) {
                        // DEBUG:
                        //	  printf("SqlEngine.select: couldn't read the key in EQ!");
                        continue;
                    }
                    
                    to_print.push_back(rid);
                    break;
                    
                    
                case SelCond::GT:
                    // locate the key-tuple and then print everything until then but using readforward
                    search_status = treeIndex.locate(compare_int_value, cursor);
                    
                    // If we couldn't find a match just keep going and forget about this condition
                    if (search_status != 0)
                        continue;
                    
                    
                    // NOTE: Beacuse this is GREATER THAN, we will skip the equal record
                    // Now read the record id, and add the tuple to the to_print vector
                    
                    read_status = treeIndex.readForward(cursor, key, rid);
                    
                    if(read_status != 0) {
                        // DEBUG:
                        //	  printf("SqlEngine.select: couldn't read the key in EQ!");
                        continue;
                    }
                    
                    while( treeIndex.readForward(cursor, key, rid) == 0 ){
                        to_print.push_back(rid);
                    }
                    
                    
                    
                case SelCond::LT:
                    // locate the tuple and then print everything 
                    // locate the key-tuple and then print everything until then but using readforward
                    search_status = treeIndex.locate(compare_int_value, cursor);
                    read_status = treeIndex.readForward(cursor, key, rid);
                    
                    
                    smallestKey = treeIndex.getSmallestKey();
                    smallest_search_status = treeIndex.locate(smallestKey, startCursor);
                    
                    
                    // If we couldn't find a match just keep going and forget about this condition
                    if (search_status != 0 || read_status != 0 || smallest_search_status != 0)
                        continue;
                    
                    // DEBUG
                    //	printf("select: key = %d\n",key);
                    //	printf("select: smallest key = %d\n",smallestKey);
                    
                    
                    // Now keep reading forward until we get to the ending cursor's value
                    while( treeIndex.readForward(startCursor, smallestKey, rid) == 0 && smallestKey < key) {
                        
                        
                        to_print.push_back(rid);
                        
                    }
                    
                    break;
                    
                    
                    // These guys are just the same deal but with the equal condition.
                case SelCond::GE:
                    // locate the key-tuple and then print everything until then but using readforward
                    search_status = treeIndex.locate(compare_int_value, cursor);
                    
                    // If we couldn't find a match just keep going and forget about this condition
                    if (search_status != 0)
                        continue;
                    
                    
                    // NOTE: Beacuse this is GREATER THAN, we will NOT skip the equal record
                    // Now read the record id, and add the tuple to the to_print vector
                    
                    /*read_status = index.readForward(cursor, key, rid);
                     
                     if(read_status != 0) {
                     // DEBUG:
                     //	  printf("SqlEngine.select: couldn't read the key in EQ!");
                     continue;
                     }*/
                    
                    while( treeIndex.readForward(cursor, key, rid) == 0 ){
                        to_print.push_back(rid);
                    }
                    break;
                    
                    
                case SelCond::LE:
                    // locate the tuple and then print everything 
                    // locate the key-tuple and then print everything until then but using readforward
                    search_status = treeIndex.locate(compare_int_value, cursor);
                    read_status = treeIndex.readForward(cursor, key, rid);
                    
                    
                    smallestKey = treeIndex.getSmallestKey();
                    smallest_search_status = treeIndex.locate(smallestKey, startCursor);
                    
                    
                    // If we couldn't find a match just keep going and forget about this condition
                    if (search_status != 0 || read_status != 0 || smallest_search_status != 0)
                        continue;
                    
                    // DEBUG
                    //	printf("select: key = %d\n",key);
                    //	printf("select: smallest key = %d\n",smallestKey);
                    
                    
                    // Now keep reading forward until we get to the ending cursor's value
                    while( treeIndex.readForward(startCursor, smallestKey, rid) == 0 && smallestKey <= key) {
                        
                        
                        to_print.push_back(rid);
                        
                    }
                    
                    
                    break;
            }
        }
        
        // DEBUG
        // printf("select: Size of to_print = %d\n",(int) to_print.size());
        
        // Go through the filtered list and print out the recordid's
        for (unsigned i = 0; i < to_print.size(); i++) {
            
            RecordId rid = to_print[i];
            
            // Open up this record and print it
            int key;
            string value;
            
            read_status = rf.read(rid, key, value);
            
            // if we can't read then just skip it
            if (read_status != 0)
                continue;
            
            // print the tuple 
            switch (attr) {
                case 1:  // SELECT key
                    fprintf(stdout, "%d\n", key);
                    break;
                case 2:  // SELECT value
                    fprintf(stdout, "%s\n", value.c_str());
                    break;
                case 3:  // SELECT *
                    fprintf(stdout, "%d '%s'\n", key, value.c_str());
                    break;
            }
            
            
            
            
        }
        
        
        
        
        //TODO: figure out the count
        // TODO: print out the tuple
    }
    
    // print matching tuple count if "select count(*)"
    if (attr == 4) {
        fprintf(stdout, "%d\n", count);
    }
    rc = 0;
    
    // close the table file and return
exit_select:
    rf.close();
    return rc;
}


// return 0 if successful
RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
//	printf("0> ");
  	RecordFile rf;
    RC rc = rf.open(table + ".tbl", 'w');
    if (rc != 0){
        rf.close();
        return rc;
    }
    
//	printf("a> ");
    ifstream input; // input stream (loadfile)
	input.open (loadfile.c_str()); // load file
	
	// if couldn't open the file, error
	if (input.fail()){
		input.close();
		return RC_FILE_OPEN_FAILED;
	}
    
    
//	printf("b> ");
	BTreeIndex treeIndex;
	if(index)
	{
		rc = treeIndex.open(table + ".idx", 'w');
        
//        printf("c> ");
        if (rc != 0){// if failed
            
//            printf("returned> ");
            input.close();
            rf.close();
            return rc;
        }
        
//        printf("d> ");
	}

	string line; // read each line
//	printf("0> ");
	// if we were able to open the input stream
	if (input.is_open()){
		while (input.good() && getline(input, line, '\n')){
			RecordId rid; 		// rid
			string val;			// value of each line
			int key;			// key of each line
			
			if (!parseLoadLine(line, key, val)){
//				printf("1> ");
				if (!rf.append(key, val, rid)){
//                    printf("2> ");
					// we successfully appended
					if(index)
					{
//                        printf("3> ");
						if(treeIndex.insert(key,rid) != 0)
						{
							cerr << "Unable to insert <" << key << "> to the treeIndex" << endl;
							return RC_FILE_WRITE_FAILED;
						}
					}
				} else {
					return RC_INVALID_ATTRIBUTE; // error in appending
				}
			} else {
				return RC_INVALID_ATTRIBUTE; // there's an error in parsing
			}
		}
	} 
	
	treeIndex.close();
	input.close();	// close the input stream
	rf.close(); 	// close the recordfile
  	return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
