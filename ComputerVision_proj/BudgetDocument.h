/* Name: BudgetDocument.h
 * Owner: Eric Palma
 * Project ReceiptReader_BudgetProjecct
 * Purpose: This class represents the Budget File
 */
#ifndef BUDGETDOCUMENT_H
#define BUDGETDOCUMENT_H
#include <iostream>
#include <map>

//Holds all the data related to the budget for a given month
struct Month
{
	std::string monthName;
	double total;
};

//Class Definition
class BudgetDocument
{
private:
	BudgetDocument();			//no default, must have a text file
	std::string fileDirectory;
	std::string fileName;
	std::map < std::string, Month*> months;
public:
	BudgetDocument(std::string, std::string);
	~BudgetDocument();
	bool Init();
	bool WriteToFile(Month);
};

#endif
