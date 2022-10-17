#pragma once

#include "stdafx.h"

class FileManager
{
public:

	FileManager() { /*s_Instance = this;*/ }
	~FileManager() { CloseAllFiles(); }

	//static FileManager* GetInstance() { return s_Instance; }

	FILE* OpenFile(const wchar_t* path, const wchar_t* mode);

	void CloseAllFiles();

	bool CheckFileExists(const wchar_t* path);

	std::wstring ReadFileHeader(const wchar_t* path) const;

private:

	//static FileManager* s_Instance;

	std::map<std::wstring, FILE*> m_Files;
};

