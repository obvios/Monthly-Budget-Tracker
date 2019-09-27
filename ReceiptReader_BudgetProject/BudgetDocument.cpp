/*
 * Filename:	BudgetDocument.h
 * Purpose:		Represents a budget txt file. Handles data persistence
 */
#include "BudgetDocument.h"
#include <iostream>
#include <fstream>
#include <string>

//constructor
BudgetDocument::BudgetDocument(std::string dir, std::string fileName) {
	this->fileDirectory = dir;
	this->fileName = fileName;
}

//If file exists, this function reads in the months and their totals from 
//the file, storing the data in Month struct. If file does not exist, it creates one with the 
//proper layout, initializing each month's total to zero. 
//Examples of line: "January:-34.89" , "February:+41.99"
bool BudgetDocument::ReadDocument() {
	std::cout << fileDirectory + fileName << std::endl;
	std::ifstream budgTxtFile(fileDirectory + fileName);

	//attempt to open budget file
	if (budgTxtFile.is_open())
	{
		std::string line;

		//read in file line by line
		while (std::getline(budgTxtFile, line)) {
			std::string month_ = "";
			std::string number ="";
			std::string sign;

			//loop until ':' is hit. Thats the end of the month
			int index = 0;
			while (line[index] != ':') {
				month_ += line[index];
				index++;
			}

			//read in number sign. Increment index because we are at ':'
			index++;
			if (index < static_cast<int>(line.length())) {
				sign = line[index];
			}
			else {
				return false;
			}

			//just got the sign
			//read number
			index++;
			while (index < static_cast<int>( line.length() )){
				number += line[index];
				index++;
			}

			//input info into struct
			Month *newMonth = new Month;
			newMonth->monthName = month_;
			if (sign[0] == '-')
			{
				newMonth->total = -1.0 * std::stod(number);
			}
			else {
				newMonth->total = std::stod(number);
			}
			
			//insert into months map
			months.emplace(month_, newMonth);
		}
		budgTxtFile.close();
		return true;
	}
	else {
		return false;
	}
}

//iterates through months map and deletes all allocated memory
BudgetDocument::~BudgetDocument()
{
	for (std::map<std::string, Month*>::iterator it = months.begin(); it != months.end(); ++it) {
		if (it->second->total < 0)	//negative
		{
			std::cout << it->first << ": " << it->second->total << std::endl;
			std::cin.get();
		}
		else {
			std::cout << it->first << ": +" << it->second->total << std::endl;
			std::cin.get();
		}
		delete it->second;
	}
}
