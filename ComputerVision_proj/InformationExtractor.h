/*
 * Filename:	InformationExtractor.h
 * Purpose:		Header file for InformationExtractor class
 * Description: Responsible for extracting total from a receipt
 */
#ifndef INFORMATIONEXTRACTOR_H
#define INFORMATIONEXTRACTOR_H
#define BUFFERSIZE 4096

#include<Windows.h>
#include <tchar.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class InformationExtractor
{
public:
	InformationExtractor();
	~InformationExtractor();
	bool Init(const char* datapath = "./Tess/tessdata", const char* language = "eng");
	//change to get image from opencv
	bool ExtractTotal( unsigned char const *, int width, int height, int bytesPerPix, int cols);
	double GetTotalValue();

private:
	double TotalValue = 0.0;
	bool TextExtracted = false;
	tesseract::TessBaseAPI tess;
	//private methods used to extract total from receipt
	bool isValidDigit(char);
	bool isDollarValue(std::string);
	double dollarVal_TO_doubleVal(std::string);
	double FindTotal(std::vector<double>);
	std::vector<double> ExtractDollarValues(tesseract::ResultIterator *it_);
};

#endif 

