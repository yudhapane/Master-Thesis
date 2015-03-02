/*
 *  CassandraReader.cc
 *  
 *
 *  Created by Owen Thomas on 16/06/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include "CassandraReader.hh"

using namespace libpg;

list<string> tokenise (const string& line) {
	
	//construct a string stream.
	stringstream lineStream (line);
	
	//strip out white space and stop if 
	//we get a comment.
	list<string> tokens;
	string token;
	while(lineStream >> token) {
		if(string::npos != token.find("#", 0)) break;
		tokens.push_back (token);
	}
	
	return tokens;
}

void processData (SparseMatrix& m,  istream& matrixData)
{
	string line;
	
	for(unsigned int i = 0; i < m.size1(); i++) {
		getline(matrixData, line);
		if(i == 0 && line == "uniform") {
			for(i = 0; i < m.size1(); i++) {
				for(unsigned int j = 0; j < m.size2(); j++) {
					m(i,j) = 1.0  / m.size2();
				}
			}
			return;			
		}
		else if(i == 0 && line =="identity") {
			//I take min of size1() and size2() _incase_ someone has
			//put identity for a matrix that does not want to be square.
			for(i = 0; i < min(m.size1(),m.size2()); i++) {
				m(i,i) = 1.0;
			}
			return;
		}
		else {
			list<string> rowData = tokenise (line);
			list<string>::iterator it = rowData.begin ();
			for(unsigned int j = 0; j < m.size2(); j++) {
				m(i,j) = atof ((*it++).c_str());
			}
		}
	}
}

void processData (SparseMatrix& m, int b, list<string>::iterator s, 
	list<string>::iterator e) {
	if(b == -1) {
		for(unsigned int i = 0; i < m.size1(); i++) {
			processData (m, i, s, e);
		}
	} 
	else {
		int columnIndex = 0;
		while(s != e) {
			m (b, columnIndex++) = atof((*s++).c_str());;
		}
	}
}

void processData (SparseMatrix& m, int b,  istream& matrixData) {
	string line;
	while(getline(matrixData,line)) {
		list<string> tokens = tokenise (line);
		if(tokens.size()) {
			processData (m, b,  tokens.begin(), tokens.end());
			return;
		}
	}
}

void processData (SparseMatrix& m,  int b,  int c, string value) {
	if(b == -1) {
		for(unsigned int i = 0; i < m.size1(); i++) {
			processData (m, i, c, value);
		}
	}
	else if (c == -1) {
		for(unsigned int i = 0; i < m.size2(); i++) {
			processData (m, b, i, value);
		}
	}
	else
		m (b,c) = atof (value.c_str());
}

void processData (SparseMatrix& m,  int b,  int c, istream& matrixData) {
	string line;
	while(getline(matrixData,line)) {
		list<string> tokens = tokenise (line);
		if(tokens.size()) {
			processData (m, b, c, tokens.front());
			return;
		}
	}
}

int toIndex (map<string, int> map, string potentialIndex) {

	if(potentialIndex == "*") return -1;
	int i = atoi (potentialIndex.c_str());
	if(i) return i;
	return map [potentialIndex];
}

Cassandra* readCassandra (ifstream& file) {

	//Headers
	float discount = 0.0F;
	bool rewardValue;
	
	
	//Either there will be entries in the map,
	//or the corresponding numXXX value will be set.
	map<string, int> states;
	map<string, int> observations;
	map<string, int> actions;
	
	int numStates = 0;
	int numObservations = 0;
	int numActions = 0;
	
	//Data
	map<int, SparseMatrix*> actionTransitions;
	map<int, SparseMatrix*> actionObservations;
	map<int, map<int, SparseMatrix*> > rewards;
	
	string line;
	int lineNumber = 1;
	
	//Read through each line in the stream.
	while (getline (file, line)) {
	
		//Find the number of colons, used to for parsing
		//object definition lines.
		int numColons = 0;
		unsigned int i = line.find(":",0);
		
		while(i != string::npos) {
			numColons ++;
			i = line.find(":", i+1);
		}
		
		//Replace each : with a space, causing colons to become
		//tokens.
		for(int i = 0; i < numColons; i++) {
			int index = line.find(":", 0);
			line.replace (index, 1, " ");
		}
		
		//Tokenise line (this stops when it finds a token with a # in it).
		list<string> tokens = tokenise (line);
		
		//if we have some tokesn in this line.
		if(tokens.size()) {
		
			//retrieve the handle, the first token, used to determine
			//how to parse the line.
			string handle = tokens.front ();
			
			//The discount for the simulator, this is a floating point number.
			if("discount" == handle) {
				discount = atof (tokens.back ().c_str());
				if(discount == 0.0) {
					cout << "Error [" << lineNumber << 
					" invalid floating point: "
						<< tokens.back () << endl;
					return NULL;
				}
			}
			
			//Value, currently ignored in the simulator.
			if("values" == handle) {
				if(tokens.back() == "reward") {
					rewardValue = true;
				}
				else if(tokens.back () == "cost") {
					rewardValue = false;
				}
				else {
					cout << "Error [" << lineNumber <<"] unknown value: "
						<< tokens.back () << endl;
						
					return NULL;
				}
			}
			
			//Either a list of state names or an integer listing the
			//number of states.
			if("states" == handle) {
				list<string>::iterator it = tokens.begin ();
				it++;
				
				numStates = atoi ((*it).c_str());
				
				if(0 == numStates) {
					int i = 0;
					while(it != tokens.end()) {
						states [(*it++)] = i++;
					}
					numStates = states.size();
				}
			}
			
			//Either a list of action names or an integer
			//stating the number of actions.
			if("actions" == handle) {
				list<string>::iterator it = tokens.begin ();
				it++;
				
				numActions = atoi ((*it).c_str());
				if(0 == numActions) {
					int i = 0;
					while(it != tokens.end())  {
						actions [(*it++)] = i++;
					}
					numActions = actions.size ();
				}
			}
			
			//Either a list of action names or an integer stating
			//the number of actions.
			
			
			if("observations" == handle) {
				list<string>::iterator it = tokens.begin ();
				it++;
				
				numObservations = atoi ((*it).c_str());
				if(0 == numObservations) {
					int i = 0;
					while(it != tokens.end())  {
						observations [(*it++)] = i++;
					}
					numObservations = observations.size();
				}
			}
			
			//Process a line, and potentially additional lines of data,
			//defining an observation.
			if("O" == handle) {
				
				//Are we operating over all actions.
				bool wildAction = false;
								
				//Move through the list of tokens, skipping the handle.
				list<string>::iterator it = tokens.begin ();
				it ++;
				
				int actionIndex = toIndex (actions, (*it++));
				
				//If the action is a wild car.
				if(actionIndex == -1) {
					actionIndex = 0;
					wildAction = true;
				}
				
				//If there is no action so far.
				if(actionObservations [actionIndex] == NULL) {
					actionObservations [actionIndex] = new SparseMatrix (numStates, numObservations);
					actionObservations [actionIndex]->clear();
				}
				
				
				numColons --;
				
				//case: T: <action-id>
				if(0 == numColons) {
					processData (*(actionObservations [actionIndex]), file);
				}
				
				else {
					int endStateIndex = toIndex (states, (*it++));
					
					
					numColons --;
					
					//case: T: <action-id> : <end-state>
					if(0 == numColons) {
						//Data is on following line.
						if(it == tokens.end())
							processData (*(actionObservations [actionIndex]), endStateIndex, 
								file);
						
						//Data is on this line (as we have remaining tokens)
						else
							processData (*(actionObservations [actionIndex]), endStateIndex, 
								it, tokens.end());
					}
					
					else {
						int observationIndex = toIndex (observations, (*it++));
						
						//T: <action-id> : <end-state> : <observation-id>
						//Data is on next line.
						if(it == tokens.end()) 
							processData (*(actionObservations [actionIndex]), endStateIndex, observationIndex, file);
						//Data is on this line.
						else 
							processData (*(actionObservations [actionIndex]), endStateIndex, observationIndex, *it);
					}
				}
				
				//We have updated action[0]. If the action is a wild star, then copy
				//action[0] to all other actions.
				if(wildAction) {
					for(int i = 1; i < numActions; i++) {
						if(actionObservations [i]) {
							delete actionObservations [i];
						}
						actionObservations [i] = new SparseMatrix( *(actionObservations [0]));
					}
				}
			}
			
			//Process a Reward specification.
			if("R" == handle) {
				
				//Do we have a wild in action or start state
				bool wildAction = false;
				bool wildStartState = false;
				
				//iterate over remaining tokesn, ignoring handle.
				list<string>::iterator it = tokens.begin ();
				it++;
				
				//Retrieve the action index.
				int actionIndex = toIndex (actions, (*it++));
				
				//Is it a wild.
				if(actionIndex == -1) {
					wildAction = true;
					actionIndex = 0;
				}
				
				numColons --;
				
				//Retrieve the start state index, again is it a wild.
				int startState = toIndex (states, (*it++));
				
				
				if(startState == -1) {
					wildStartState = true;
					startState = 0;
				}
				
				numColons --;
				
				//If there is no entry for this action and start state, then create it.
				if((rewards [actionIndex]) [startState] == NULL) {
					rewards [actionIndex] [startState] = new SparseMatrix (numStates, numObservations);
					rewards [actionIndex] [startState]->clear();
				}

				//R: <action-id> : <start-state>
				//<matrix>
				if(numColons == 0) {
					processData (*(rewards [actionIndex] [startState]), file);
				}
				
				else {
					int endState = toIndex (states, (*it++));
				
					numColons --;
					//R: <action-id> : <start-state> : <end-state>
					//<row-vector>
					if(numColons == 0) {
						//Data is on next line.
						if(it == tokens.end()) {
							processData (*(rewards [actionIndex] [startState]), endState, file);
						//Data is on this line.
						} else {
							processData (*(rewards [actionIndex] [startState]), endState, it, tokens.end());
						}
					}
					
					//R: <action-id> : <start-state> : <end-state> : <observation> : %f
					else {
						
						int observationIndex = toIndex (observations, (*it++)) ;
						//Data is on next line.
						if(it == tokens.end()) {
							processData (*(rewards [actionIndex] [startState]), endState, observationIndex, file);
						//Data is on this line.
						} else {
							processData (*(rewards [actionIndex] [startState]), endState, observationIndex, *it);
						}
					}
				}
				
				
				if(wildStartState) {
					for(int j = 1; j < numStates; j++) {
						if(rewards [actionIndex] [0])
							rewards [actionIndex] [j] = new SparseMatrix (*(rewards [actionIndex] [0]));
						else {
							rewards [actionIndex] [j] = new SparseMatrix (numStates,numObservations);
							rewards [actionIndex] [j]->clear();
						}
					}
				}

				
				if(wildAction) {
					for(int i = 1; i < numActions; i++) {
						for(int j = 0; j < numStates; j++) {
							if(rewards [0] [j]) {
								rewards [i] [j] = new SparseMatrix ( *(rewards [0] [j]));
							} else {
								rewards [i] [j] = new SparseMatrix (numStates, numObservations);
								rewards [i] [j]->clear ();
							}
						}
					}
				}
			}
			
			//Process an action transition probability entry.
			if("T" == handle) {
				list<string>::iterator it = tokens.begin ();
				it++;
				
				bool wildAction = false;
				
				int actionIndex = toIndex (actions, (*it++));
				
				if(actionIndex == -1) {
					wildAction = true;
					actionIndex = 0;
				}
				
				
				if(actionTransitions [actionIndex] == NULL) {
					actionTransitions [actionIndex] =  new SparseMatrix(numStates, numStates);
					actionTransitions [actionIndex]->clear();
				}
				
				numColons --;
				
				if(0 == numColons) {
					processData (*(actionTransitions [actionIndex]), file);
				}
				
				else {
					int startState = toIndex (states, (*it++));
				
					numColons --;
				
					if(0 == numColons) {
						if(it == tokens.end()) {
							processData (*(actionTransitions [actionIndex]), startState, file);
						} else {
							processData (*(actionTransitions [actionIndex]), startState, it, tokens.end());
						}
					}
				
					else {	
						int endState = toIndex (states, (*it++));
						
						if(it == tokens.end()) {
							processData (*(actionTransitions [actionIndex]), startState, endState, file);
						} else {
							processData (*(actionTransitions [actionIndex]), startState, endState, *it);
						}
					}
				}
				
				if(wildAction) {
					for(int i = 1; i < numActions; i++) {
						if(actionTransitions [i]) {
							delete actionTransitions [i];
						}
						actionTransitions [i] = new SparseMatrix( *(actionTransitions [0]) );
					}
				}
			}
		
		}
		
		lineNumber ++;
	}
	
	
	//If we have remaining, unspecified matricies - fill them out.
	//This could potentially occur in the simulator, with the simulator providing
	//entries when they are requested. But that could lead to sporadic timing results,
	//this way it will be consistent.
	
		for(int i = 0; i < numActions; i++) {
			if(!actionTransitions [i]) {
				actionTransitions [i] = new SparseMatrix (numStates, numStates);
				actionTransitions [i]->clear();
			}
			if(!actionObservations [i]) {
				actionObservations [i] = new SparseMatrix (numStates, numObservations);
				actionObservations [i]->clear ();
			}
			for(int j = 0; j < numStates; j++) {
				if(!rewards [i] [j]) {
					rewards [i] [j] = new SparseMatrix (numStates, numObservations);
					rewards [i] [j]->clear ();
				}
			}
		}
	
	SparseMatrix* ats = new SparseMatrix [numActions];
	SparseMatrix* aobs = new SparseMatrix [numActions];
	SparseMatrix** rs = new SparseMatrix* [numActions];
	
	for(int i = 0; i < numActions; i++) {
		ats [i] = *(actionTransitions [i]);
		aobs [i] = *(actionObservations [i]);
		rs [i] = new SparseMatrix [numStates];
		for(int j = 0; j < numStates; j++) {
			rs [i] [j] = *(rewards [i] [j]);
			cout << "reward matrix: " << *(rewards [i] [j]) << endl;
		}
	}
	
	//Return the Simulator.
	return new Cassandra (ats, aobs, rs, 
		numActions, numStates, numObservations, discount);
}
