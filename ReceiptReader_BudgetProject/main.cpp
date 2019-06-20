#include "BudgetDocument.h"
#include "InformationExtractor.h"
#include <iostream>


int main(int argc, char * argv[]) {
	BudgetDocument budgetDoc = BudgetDocument("./", 
		"BudgetFile.txt");
	InformationExtractor infoextractor;

	if (budgetDoc.Init()) {
		//initialized
		//initialize information extractor
		infoextractor.Init("./tessdata", "eng");
		infoextractor.ExtractText("receipt1.jpg");
	}
	else {
		std::cout << "could not open file\n";
		std::cin.get();
	}


	return 0;
}