#include "stdafx.h"

#include "FileManager.h"

FILE* FileManager::OpenFile(const wchar_t* path, const wchar_t* mode)
{
	FILE* file = nullptr;
	std::wstring localPath = AuExpManager::Instance()->GetExportDirectory();
	localPath += L"\\";
	localPath += path;
	errno_t err = _wfopen_s(&file, localPath.c_str(), mode);

	if (file)
		m_Files[path] = file;
	else
		MessageBox(nullptr, localPath.c_str(), TEXT("Error creating directory"), MB_OK);

	return file;
}

void FileManager::CloseAllFiles() 
{
	for (auto& pair : m_Files)
	{
		fclose(pair.second);
	}
}

bool FileManager::CheckFileExists(const wchar_t* path)
{
	FILE* file = nullptr;
	std::wstring localPath = AuExpManager::Instance()->GetExportDirectory();
	localPath += L"\\";
	localPath += path;
	errno_t err = _wfopen_s(&file, localPath.c_str(), L"rb");

	bool exists = false;

	if (file)
	{
		exists = true;
		fclose(file);
	}

	return exists;
}

std::wstring FileManager::ReadFileHeader(const wchar_t* path) const
{
	FILE* file = nullptr;
	std::wstring localPath = AuExpManager::Instance()->GetExportDirectory();
	localPath += L"\\";
	localPath += path;
	errno_t err = _wfopen_s(&file, localPath.c_str(), L"rb");

	const uint32_t bufferSize = 1024;
	wchar_t result[bufferSize];

	if (file)
	{
		fgetws(result, bufferSize, file);
		fclose(file);
	}
	else
		throw "Could not open file for reading";

	std::wstring wresult = result;

	return wresult;
}