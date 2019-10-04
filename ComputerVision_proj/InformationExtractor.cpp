/*
 * Filename:	InformationExtractor.cpp
 * Purpose:		Implementation for InformationExtractor functions
 */

#include <fstream>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <algorithm>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "InformationExtractor.h"

//default constructor
InformationExtractor::InformationExtractor() {};

//initializes tesseract ocr engine
bool InformationExtractor::Init(const char * datapath, const char * language) {
	//initialize tesseract
	if (this->tess.Init("./Tess/tessdata", "eng"))
	{
		std::cout << "OCRTesseract: Could not initialize tesseract." << std::endl;
		std::cin.get();
		return false;
	}

	// setup tesseract
	tess.SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);
	tess.SetVariable("save_best_choices", "T");

	
}

//Extracts the text from receipt image and extracts total.
//sets TextExtracted to true if successful.
//returns true if a value is extracted
bool InformationExtractor::ExtractTotal(unsigned char const * cvImage, int width, int height, int bytesPerPix, int cols) {
	// recognize
	this->tess.SetImage(cvImage, width, height, bytesPerPix, cols);

	//image read successfully
	if (this->tess.Recognize(0) == 0) {
		//assert text extracted
		this->TextExtracted = true;

		//get iterator
		tesseract::ResultIterator * tessIt_ = tess.GetIterator();

		//get all dollar values in receipt image
		std::vector<double> values = this->ExtractDollarValues(tessIt_);

		//dollar values detected sccessfully
		if (values.size() != 0) {
			//find total from dollar values
			this->TotalValue = this->FindTotal(values);
			
			//clean up
			delete tessIt_;
			tess.Clear();

			return true;
		}
		else {
			//clean up
			delete tessIt_;
			tess.Clear();

			return false;
		}
	}

	//clean up
	tess.Clear();

	return false;
}

//returns a vector holding all the dollar values found in receipt
//returns empty vector if no dollar values detected in receipt
std::vector<double> InformationExtractor::ExtractDollarValues(tesseract::ResultIterator *it_){
	std::vector<double> dollarValues = {};
	if (this->TextExtracted) {
		while (it_->Next(tesseract::RIL_WORD)) {
			std::string line(it_->GetUTF8Text(tesseract::RIL_WORD));
			//analyze word 
			if (this->isDollarValue(line)) {
				//convert to double
				double doubleDollVal = this->dollarVal_TO_doubleVal(line);

				//insert to values vector
				dollarValues.push_back(doubleDollVal);
			}
		}
		this->TextExtracted = false;
		return dollarValues;
	}
	else {
		this->TextExtracted = false;
		return dollarValues;
	}
}

//checks if string is a dollar value.
//returns true if it is, else it returns false.
//currently only limited to values under 1,000.
//only works with values that have a '$' before or no sign at all
bool InformationExtractor::isDollarValue(std::string val) {
	int i = 0;
	bool decimalFlag = false;		//to ensure a number has a decimal value
	//case 1. Word starts with dollar sign
	if ( val[i] == '$' ) {
		if(i + 1 < val.size()){ i++; }
		//ensure characters after dollar sign are valid
		for (; i < val.size(); i++) {
			if ( !this->isValidDigit(val[i]) && val[i] != '.') {
				return false;
			}
			
			if (val[i] == '.') {
				//validate the two following characters. Currency after "." is typically in format ".13", meaning only two digits to represent cents
				//must also ensure we do not go out of bounds
				if ( ( (i + 1) < val.size() ) && ( (i + 2) < val.size() ) && ( (i + 3) == val.size() ) ) {
					if ( this->isValidDigit(val[i + 1]) && this->isValidDigit(val[i + 2]) ) {
						decimalFlag = true;
					}
				}
			}
		}

		return decimalFlag;
	}
	else {	//case 2. word starts with digit, has no dollar sign
		if (this->isValidDigit(val[i]) ) {
			if (i + 1 < val.size()) { i++; }
			for (; i < val.size(); i++) {
				if (!this->isValidDigit(val[i]) && val[i] != '.') {
					return false;
				}

				if (val[i] == '.') {
					if ( ( (i + 1) < val.size() ) && ( (i + 2) < val.size() ) && ( (i + 3) == val.size() ) ) {
						if (this->isValidDigit(val[i + 1]) && this->isValidDigit(val[i + 2])) {
							decimalFlag = true;
						}
					}
				}
			}

			return decimalFlag;
		}
		else {
			return false;
		}
	}
}

//converts a string holding a dollar value into a double.
//dollar value may have a $ sign or not
double InformationExtractor::dollarVal_TO_doubleVal(std::string dollarVal)
{
	char firstChar = dollarVal[0];
	if (firstChar == '$') {
		std::string temp = "";

		for (int i = 1; i < dollarVal.size(); i++) {
			temp += dollarVal[i];
		}

		return std::stod(temp);
	}
	else {
		return std::stod(dollarVal);
	}
}

//Finds the total from a list of dollar values extracted from a receipt.
//Finds largest value and assumes it to be the total, unless the next value subtracted from it 
//equals another value in the list. Meaning the value subtracted from it were taxes, and the new result
//is the actual total even though it is not the max.
double InformationExtractor::FindTotal(std::vector<double> vals)
{
	std::vector<double>::iterator it_ = std::max_element(vals.begin(), vals.end());
	double subTotal = *it_;

	//check if any number after it equals subTotal - taxes, meaning it would be the true total
	if (it_ != vals.end()) {
		it_++;
	}
	else {
		return subTotal;
	}
	double Total = subTotal - (*it_);

	//search for Total
	for (double value : vals) {
		if (value == Total) { return Total; }
	}
	
	//subTotal is true total
	return subTotal;
}

//ensures that the character is a valid digit.
bool InformationExtractor::isValidDigit(char val) {
	if (val + 0 >= -1 && val + 0 <= 255) {
		return isdigit(val);
	}
	else {
		return false;
	}
}

//returns the Total extracted from receipt.
//This method is destructive, meaning the current Total Value is reset to 0.00
double InformationExtractor::GetTotalValue()
{
	double Total = this->TotalValue;
	this->TotalValue = 0.0;

	return Total;
}

//clears tess engine
InformationExtractor::~InformationExtractor() {
	tess.Clear();
}
