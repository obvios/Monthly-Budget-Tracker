/*
 * Filename:	InformationExtractor.h
 * Purpose:		Header file for InformationExtractor class
 * Description: Responsible for extracting total from a receipt
 */
#ifndef INFORMATIONEXTRACTOR_H
#define INFORMATIONEXTRACTOR_H

#include<Windows.h>
#include <tchar.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class InformationExtractor
{
public:
	InformationExtractor();
	~InformationExtractor();
	bool Init(const char* datapath = "./tessdata", const char* language = "eng");
	bool ExtractText(char const *);
	bool ExtractTotalSpent();
	double GetTotalValue();

private:
	double TotalValue = 0.0;
	bool TextExtracted = false;
	HANDLE hTempFile = INVALID_HANDLE_VALUE;
	TCHAR hTempFileNameBuffer[MAX_PATH];
	TCHAR hTempPathBuffer[MAX_PATH];
	tesseract::TessBaseAPI tess;
};

#endif // !INFORMATIONEXTRACTOR_H
