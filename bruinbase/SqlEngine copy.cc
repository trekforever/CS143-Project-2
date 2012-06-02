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
	BTreeIndex treeIndex;	//Define index
	bool useIndex = false;	//At first we're not using index
    IndexCursor indexCur;
    
    //Make use of With Indexing Option
    int compareMe = 0;
    //1 is key column
    SelCond* NotEqual1 = NULL;
    SelCond* Max1 = NULL;
    SelCond* Equal1 = NULL;
    SelCond* Min1 = NULL;
	vector<SelCond*> NotEqualV1;
    //2 is value column
    SelCond* NotEqual2 = NULL;
    SelCond* Max2 = NULL;
    SelCond* Equal2 = NULL;
    SelCond* Min2 = NULL;
  vector<SelCond*> NotEqualV2;
    
    //Other importante variables
    bool GLOBAL_GEqualKey = false;	//Defines if the key is Greater/Equal to
    bool GLOBAL_LEqualKey = false;


	        bool noteql1 = false;
	 bool noteql2 = false;
	vector<int> keyNotEqual;
	bool eqlBool = false;
    
	//We go through all of our conditions, and determine the min and the max and all other
	//conditional values. We do this so that we can optimize the query instead of going
	//through each condition and running it.
    for(int i=0; i < cond.size(); i++)
    {
        //Can also use stringstream too
        compareMe = atoi(cond[i].value);
        
        //Iterate through all the members of the vector input
        if(cond[i].attr == 1)
        {
            //1 is Key Column
            switch(cond[i].comp)	//We traverse and iterate through all the comparators
            {
                case SelCond::NE:	//Not Equal
                    NotEqual1 = (SelCond*) &cond[i];
						 NotEqualV1.push_back(NotEqual1);
                    break;
                case SelCond::GT:
                    // First need to check if the key is a min
                    // Also if our value is greater than it.
                    if(Min1 == NULL || compareMe > atoi(Min1->value))
                    {
                        //There is no min value set, or the current
									// value we have is greater than the value set.
                        Min1 = (SelCond*) &cond[i];
                        GLOBAL_GEqualKey = false;
                    }
                    break;
                case SelCond::LT:
                    // First need to check if the key is a max
                    // Also if it is lesser than our value
                    if(Max1 == NULL || compareMe < atoi(Max1->value))
                    {
                        //There is no max or the current value we have
									// is lesser than the value set.
                        Max1 = (SelCond*) &cond[i];
                        GLOBAL_GEqualKey = false;
                    }
                    break;
                case SelCond::GE:
                    compareMe--;	//Increment to just do GT
                    if(Min1 == NULL || compareMe > atoi(Min1->value))
                    {
                        //This is just the same code as GT, except our
									//GEqualKey is set to true
                        Min1 = (SelCond*) &cond[i];
                        GLOBAL_GEqualKey = true;
                    }
                    break;
                case SelCond::LE:
                    compareMe++;	//Increment so just do LT
                    if(Max1 == NULL || compareMe < atoi(Max1->value))
                    {
                        //This is just the same code as LT, except
									//our LEqualKey is set to true
                        Max1 = (SelCond*) &cond[i];
                        GLOBAL_LEqualKey = true;
                    }
                    break;
                case SelCond::EQ:	//Equal
						 if(Equal1 == NULL || compareMe == atoi(Equal1->value))
                    	Equal1 = (SelCond*) &cond[i];
						else
							eqlBool = true;
                    break;
            }
        }
        else if(cond[i].attr == 2)
        {
            //2 is Value Column
				// The following code is just literally copied from above, it's the same
				// as cond[i].attr == 1
            switch(cond[i].comp)
            {
                case SelCond::NE:	//Not Equal
                    NotEqual2 = (SelCond*) &cond[i];
						NotEqualV2.push_back(NotEqual2);
                    break;
                case SelCond::GT:
                    // First need to check if the key is a min
                    // Also if it is greater than our value
                    if(Min2 == NULL || atoi(Min2->value) < compareMe)
                    {
                        //There is no min value, or existing value greater
                        Min2 = (SelCond*) &cond[i];
                    }
                    break;
                case SelCond::LT:
                    // First need to check if the key is a max
                    // Also if it is lesser than our value
                    if(Max2 == NULL || atoi(Max2->value) > compareMe)
                    {
                        //There is no max or existing value is lesser
                        Max2 = (SelCond*) &cond[i];
                    }
                    break;
                case SelCond::GE:
                    compareMe--;	//Increment to just do GT
                    if(Min2 == NULL || atoi(Min2->value) < compareMe)
                    {
                        //There is no min value, or existing value greater
                        Min2 = (SelCond*) &cond[i];
                    }
                    break;
                case SelCond::LE:
                    compareMe++;	//Increment so just do LT
                    if(Max2 == NULL || atoi(Max2->value) > compareMe)
                    {
                        //There is no max or existing value is lesser
                        Max2 = (SelCond*) &cond[i];
                    }
                    break;
                case SelCond::EQ:	//Equal
							if(Equal1 == NULL || compareMe == atoi(Equal1->value))
							     Equal1 = (SelCond*) &cond[i];
							else
								eqlBool = true;
                    break;
            }
        }
        else
            cerr << "Error: SqlEngine.cc. Should Never reach here" << endl;
        
    }	//End for loop
    
    
    if(treeIndex.open(table + ".idx", 'r') == 0)
    {	//Open TreeIndex
        useIndex = true;
		//search the vector to see if there exist a not equal value
			if(NotEqualV1.size() > 0)
				noteql1 = true;
			if(NotEqualV2.size() > 0)
				noteql2 = true;
				
        //TreeIndex Opened successful
        if(Equal1 || Min1 || Max1 || noteql1)
        {
            //Using index lookup
            int target;
            int lowerBound;
            int upperBound;
            
            if(Equal1)
                target = atoi(Equal1->value); //Set target if it's equal
            else if(Min1) // greater than
            {
                lowerBound = atoi(Min1->value);	//Set lower bound
                target = atoi(Min1->value);
            }

				if (eqlBool)
					target = -1;
					
            if(Max1) //Less than
            {
                upperBound = atoi(Max1->value);	//Set upper bound
                if(!Min1)
                    target = upperBound;
            }
            if(noteql1)
            {
					for(int i = 0; i < NotEqualV1.size(); i++)
					{
						keyNotEqual.push_back(atoi(NotEqualV1[i]->value));
					}
            }
            
            //Error Checking
            if(Max1) // less than
            {	//It's less than so we want to start from 0 and go to the cusrsor
                rc = treeIndex.locate(0,indexCur);
                if(rc != 0)
                {
                    treeIndex.close();
                    return rc;
                }
            }
            else if(!noteql1)
            {	//if anything else but not equal, we want to start from our target and goes to the cursor
                rc = treeIndex.locate(target,indexCur);
                if(rc != 0)
                {
                    treeIndex.close();
                    return rc;
                }
            }
            
            //Now to read forward
            //When we have a equal1, we read while the key is equal
            //otherwise if we have a min1, we read until the end
            
            while(!treeIndex.readForward(indexCur, key, rid))
            {
                if(Equal1 && key != target)
                    continue;	//If is equal and our key does not match our target, continue
                if(Min1 && ((GLOBAL_GEqualKey)?(key < lowerBound):(key <= lowerBound)))
                    continue;	//if is min, but our key is less than that, keep going
                if(Max1 && ((GLOBAL_LEqualKey)?(key > upperBound):(key >= upperBound)))
                    break;	//if it;s max but our key is greater, keep going
					//go through each of the keynotequal element in the vector, and compare
					//that with our key, and IFF they are equal do we continue
					
					bool temp = false;
					if(noteql1)
						for(int i=0; i<keyNotEqual.size(); i++)
							if(key == keyNotEqual[i])
                                temp = true;
					if(temp)
						continue;
						
							
//                if(NotEqual1 && key == keyNotEqual)
//                    continue; //if not equal and our key equals not equal, then keep going
                rc = rf.read(rid, key, value);
                if(rc != 0)
                {
                    cerr << "Error while reading tuple in SqlEngine.cc" << endl;
                    treeIndex.close();
                    return rc;
                }
                
                if(Equal2 != NULL && value != Equal2->value)
                    continue;
                if(Max2 != NULL && value >= Max2->value)
                    continue;
                if(Min2 != NULL && value <= Min2->value)
                    continue;

					temp = false;
					if(noteql2)
						for(int i=0; i<NotEqualV2.size(); i++)
							if(value <= NotEqualV2[i]->value)
                                temp = true;
					if(temp)
						continue;

//                if(noteql2 != NULL && value <= NotEqual2->value)
//                    continue;
                
                count++;

                //Print out the tuples
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
                
                if(Equal1 != NULL && key == target )
                    break;	//Found

                if(noteql1 && key != target)
                    continue;	//Found
            }
            if(attr == 4)
                cout << count << endl;
        }
        if(Equal1 == NULL && Min1 == NULL && Max1 == NULL)
        {	//If we dont have anything set ... uhh then no need to use index
            treeIndex.close();
            goto scan_began;
        }
        treeIndex.close();
    }
    
    
    // If we don't use index
    if(!useIndex)
    {
    scan_began:
			bool temp = false;
	
        // scan the table file from the beginning
        rid.pid = rid.sid = 0;
        count = 0;
        while (rid < rf.endRid()) {
            // read the tuple
            if ((rc = rf.read(rid, key, value)) < 0) {
                fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
                goto exit_select;	//exit
            }
            
            // check the conditions on the tuple
            
            if (Equal1 != NULL && (int) key != atoi(Equal1->value))	goto next_tuple;	//if our key equal is set and the value does not equal to the currently set value
            if (Max1 != NULL && (int) key >= atoi(Max1->value))	goto next_tuple;	//If our max value is greater than the currently set max value
            if (Min1 != NULL && (int) key <= atoi(Min1->value))	goto next_tuple; //if our min value is lesser than the currently set min value
//            if (NotEqual1 != NULL && (int) key == atoi(NotEqual1->value)) goto next_tuple;
				temp = false;
				if(noteql1)
					for(int i=0; i<keyNotEqual.size(); i++)
						if(key == keyNotEqual[i])								temp = true;
				if(temp)
					goto next_tuple;
					
            	//if our value for NotEqual is set and that it equals to our NE value.
            if (noteql2 && value != Equal2->value)		goto next_tuple;	//The following are the same, but instead of key, is the value attribute\
//				 temp = false;
//				if(noteql2)
//					for(int i=0; i<NotEqualV2.size(); i++)
//						if(value == NotEqualV2[i]->value)								temp = true;
//				if(!temp)
//					goto next_tuple;
					
            if (Max2 != NULL && value >= Max2->value)	goto next_tuple;
            if (Min2 != NULL && value <= Min2->value)	goto next_tuple;
            if (NotEqual2 && value == NotEqual2->value) goto next_tuple;
            
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
        
        // print matching tuple count if "select count(*)"
        if (attr == 4) {
            fprintf(stdout, "%d\n", count);
        }
        rc = 0;
        
        // close the table file and return
    exit_select:
        rf.close();
    }
	return rc;
    
}

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
		rc = treeIndex.open(table + ".idx", 'w');	//open index file for write
        
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
