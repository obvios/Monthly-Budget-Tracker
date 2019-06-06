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
//creates the temporary file in the temporary directory.
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

	//create temp document
	//first get  temp directory path
	int tdRetVal = GetTempPath(MAX_PATH, this->hTempPathBuffer);

	if (tdRetVal > MAX_PATH || (tdRetVal == 0)) {
		std::cout << "unable to get temp path...\n";
		return false;
	}

	//get temp file name
	UINT tfRetVal = GetTempFileName(this->hTempPathBuffer, TEXT("TessOutput"), 0, this->hTempFileNameBuffer);

	if (tfRetVal == 0) {
		std::cout << "unable to get temp file name...\n";
		return false;
	}

	//create new temp file
	this->hTempFile = CreateFile((LPTSTR)this->hTempFileNameBuffer,
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (this->hTempFile == INVALID_HANDLE_VALUE) {
		std::cout << "unable to create temp file...\n";
		return false;
	}
}

//Extracts the text from receipt image.
//Saves extracted text onto temp file. 
//sets TextExtracted to true if successful.
bool InformationExtractor::ExtractText(char const * fileName) {
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

	//get text
	const char * result = this->tess.GetUTF8Text();

	//write to file
	DWORD dwBytesToWrite = (DWORD)strlen(result);
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;

	bErrorFlag = WriteFile(		//should have bytes written
		this->hTempFile,
		result,
		dwBytesToWrite,
		&dwBytesWritten,
		NULL
	);

	if (FALSE == bErrorFlag) {
		std::cout << "Unable to write to file\n";
		return false;
	}
	else {
		if (dwBytesWritten != dwBytesToWrite) {
			std::cout << "bytes written != bytes to write\n";
			return false;
		}
	}

	//close file
	if (!CloseHandle(this->hTempFile)) {
		std::cout << "did not close temp file...\n";
		return false;
	}

	//delete result pointer
	delete[] result;
	//assert text extracted
	this->TextExtracted = true;
}

//Extracts the total amount spent from the text extracted out of the
//receipt image and adds it to TotalValue. Only works if TextExtracted is true.
//Resets TextExtracted to False.
//*****will be testing code initially** Must delete
bool InformationExtractor::ExtractTotalSpent() {
	if (this->TextExtracted) {		//text was successfully extracted by ExtractText()
		const DWORD BuffSize = 400;
		DWORD lpNumberOfBytesRead = 0;
		char ReadBuffer[BuffSize] = { 0 };

		//create file to open
		this->hTempFile = CreateFile(this->hTempFileNameBuffer,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		
		if (FALSE == ReadFile(this->hTempFile, ReadBuffer, BuffSize - 1, &lpNumberOfBytesRead, NULL)) {
			std::cout << "File not read...\n";
			return false;
		}

		std::cout << "bytest read = " << lpNumberOfBytesRead;

		if (lpNumberOfBytesRead > 0 ) {
			ReadBuffer[lpNumberOfBytesRead] = '\0';	//NULL character
			std::cout << ReadBuffer;
			this->TextExtracted = false;			//reset text extracted
			return true;
		}
		else {
			return false;
		}
	}
	else {
		std::cout << "did not even enter block to read temp file\n";
		this->TextExtracted = false;
		return false;
	}
}

//destructor closes handle to file and deletes it
InformationExtractor::~InformationExtractor() {
	if (!CloseHandle(this->hTempFile)) {
		std::cout << "did not close temp file...\n";
	}

	if (!DeleteFileA(this->hTempFileNameBuffer)) {
		std::cout << "unable to delete temp file...\n";
	}
}
