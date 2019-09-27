/* Name: BudgetDocument.h
 * Owner: Eric Palma
 * Project ReceiptReader_BudgetProjecct
 * Purpose: This class represents the Budget File
 */
#ifndef BUDGETDOCUMENT_H
#define BUDGETDOCUMENT_H
#include <iostream>
#include <map>
#include <filesystem>
namespace fs = std::experimental::filesystem;

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
	std::string fileDirectory = "C:\\Users\\Eric\\Documents\\Development\\TesseractProject\\ReceiptReaderProject\\Financial";
	std::string fileName = "\\Budget.txt";
	std::map < std::string, Month*> months;
	bool ReadDocument();
	bool CreateDocument();
public:
	BudgetDocument();
	~BudgetDocument();
	bool WriteToFile(std::string month, double val);
};

#endif
