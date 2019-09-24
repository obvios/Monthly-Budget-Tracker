/*
 * Filename:	InformationExtractor.cpp
 * Purpose:		Implementation for InformationExtractor functions
 */

#include <fstream>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "Tess/InformationExtractor.h"

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

//Extracts the text from receipt image.
//sets TextExtracted to true if successful.
bool InformationExtractor::ExtractText(unsigned char const * cvImage, int width, int height, int bytesPerPix, int cols) {
	// recognize
	this->tess.SetImage(cvImage, width, height, bytesPerPix, cols);
	this->tess.Recognize(0);

	//assert text extracted
	this->TextExtracted = true;

	//get iterator
	tesseract::ResultIterator * tessIt_ = tess.GetIterator();

	//call ExtractTotalSpent
	this->ExtractTotalSpent(tessIt_);

	//clean up
	tess.Clear();
	delete tessIt_;

	return true;
}

//Extracts the total amount spent from the text extracted out of the
//receipt image and adds it to TotalValue. Only works if TextExtracted is true.
//Resets TextExtracted to False.
///hasss tteessstttiinngggg
bool InformationExtractor::ExtractTotalSpent(tesseract::ResultIterator *it_) {
	if (this->TextExtracted) {
		while (it_->Next(tesseract::RIL_WORD)) {
			std::string line(it_->GetUTF8Text(tesseract::RIL_WORD));
			//analyze word 
			if (this->isDollarValue(line)) {
				std::cout << line << std::endl;
			}
		}
		this->TextExtracted = false;
		return true;
	}
	else {
		this->TextExtracted = false;
		return false;
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

//ensures that the character is a valid digit.
bool InformationExtractor::isValidDigit(char val) {
	if (val + 0 >= -1 && val + 0 <= 255) {
		return isdigit(val);
	}
	else {
		return false;
	}
}

//clears tess engine
InformationExtractor::~InformationExtractor() {
	tess.Clear();
}
