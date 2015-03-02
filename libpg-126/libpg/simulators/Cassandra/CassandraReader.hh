/*
 *  CassandraReader.hh
 *  
 *
 *  Created by Owen Thomas on 19/06/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#include <map>

#include "Cassandra.hh"

using namespace std;
using namespace libpg;

Cassandra* readCassandra ( ifstream& file);

list<string> tokenise (const string& line);

void processData (Matrix& m, istream& matrixData);

void processData (Matrix& m, int b, list<string>::iterator s, 
	list<string>::iterator e);
	
void processData (Matrix& m, int b, istream& matrixData);

void processData (Matrix& m, int b, int c, string value);

void processData (Matrix& m, int b, int c, istream& matrixData);

int toIndex (map<string, int> map, string potentialIndex);
