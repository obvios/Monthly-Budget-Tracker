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
#include "InformationExtractor.h"

//default constructor
InformationExtractor::InformationExtractor() {};

//initializes tesseract ocr engine
bool InformationExtractor::Init(const char * datapath, const char * language) {
	//initialize tesseract
	if (this->tess.Init("./tessdata", "eng"))
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
bool InformationExtractor::ExtractText(char const * fileName) {
	//read file
	auto pixs = pixRead(fileName);
	if (!pixs)
	{
		std::cout << "Cannot open input file: " << fileName << std::endl;
		std::cin.get();
		return false;
	}

	// recognize
	this->tess.SetImage(pixs);
	this->tess.Recognize(0);

	//assert text extracted
	this->TextExtracted = true;

	//get iterator
	tesseract::ResultIterator * tessIt_ = tess.GetIterator();

	//call ExtractTotalSpent
	this->ExtractTotalSpent(tessIt_);

	//clean up
	tess.Clear();
	pixDestroy(&pixs);
	delete tessIt_;

	return true;
}

//Extracts the total amount spent from the text extracted out of the
//receipt image and adds it to TotalValue. Only works if TextExtracted is true.
//Resets TextExtracted to False.
bool InformationExtractor::ExtractTotalSpent(tesseract::ResultIterator *it_) {
	if (this->TextExtracted) {
		while (it_->Next(tesseract::RIL_WORD)) {
			std::string line(it_->GetUTF8Text(tesseract::RIL_WORD));
			//analyze word
			
		}
		this->TextExtracted = false;
		return true;
	}
	else {
		this->TextExtracted = false;
		return false;
	}
}

//
bool InformationExtractor::isDollarValue(std::string val) {
	int i = 0;
	//case 1
	if ( val[i] == '$' ) {
		i++;
		for (; i < val.size(); i++) {
			if ( !isdigit(val[i]) && val[i] != '.') {
				return false;
			}
			
			if (val[i] == '.') {
				if (!isdigit(val[i + 1]) || !isdigit(val[i + 2])) {
					return false;
				}
				else {
					return true;
				}
			}
		}
	}
	else {
		if (isdigit(val[i]) ) {
			i++;
			for (; i < val.size(); i++) {
				if (!isdigit(val[i]) && val[i] != '.') {
					return false;
				}

				if (val[i] == '.') {
					if (!isdigit(val[i + 1]) || !isdigit(val[i + 2])) {
						return false;
					}
					else {
						return true;
					}
				}
			}
		}
		else {
			return false;
		}
	}
}

//clears tess engine
InformationExtractor::~InformationExtractor() {
	tess.Clear();
}
