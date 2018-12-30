
#include <iostream>
#include <cstdio>
#include <algorithm>

#include "bufferManager.h"
#include "indexManager.h"
#include "definitions.h"
#include "catalogManager.h"
#include "exception.h"
#include "basic.h"
#include "node.h"
#include "interpreter.h"
#include "RecordManager.h"

bufferManager* buffer_manager;

using namespace std;

int main(int argc, const char * argv[]) {
    cout << "Hello, World!" << endl;
    cout << "Start testing!" << endl;
    
    buffer_manager = new bufferManager();

	
	while (1){
		Interpreter III;
		 
		III.getInput();	
		III.parseInput();
	}
	delete buffer_manager;
	return 0;
}	

