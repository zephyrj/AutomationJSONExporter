/************************************************* *
** Copyright(c) 2021 -- Camshaft Software PTY LTD
** All Rights Reserved
************************************************** */

#include "stdafx.h"
#include <Shlobj.h>

#include <string>
#include <vector>
#include <sstream>

#include "FileManager.h"
#include "JSONDocumentBuilder.h"


namespace {
	const std::wstring IllegalCharacters = L"\\/:?\"<>|";
	void sanitizeFileName(std::wstring& in)
	{
		for (std::wstring::iterator i = in.begin(); i < in.end(); i++)
		{
			if (IllegalCharacters.find(*i) != std::wstring::npos)
			{
				*i = L'_';
			}
		}
	}

	size_t FindDirDelimiter(std::wstring dir, size_t start)
	{
		size_t slashPos = dir.find(L"\\", start);

		if (slashPos == std::wstring::npos)
		{
			return dir.find(L"/", start);
		}

		return slashPos;
	}
}

const std::set<std::wstring> AuExpManager::METADATA_KEYS = { L"meta", L"__index" };

AuExpManager* AuExpManager::s_Instance = nullptr;

AuExpManager::AuExpManager() :
	m_ExportDirectory(L"")
{
	s_Instance = this;
	BuildKeyTranslations();
}

AuCarExpErrorCode AuExpManager::Init(const AuCarExpCarData* carData)
{
	TCHAR path[MAX_PATH];
	m_CarData = carData;

	//get the user's documents directory:
	if (SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, SHGFP_TYPE_CURRENT, path) == S_OK)
	{
		m_ExportDirectory = path;
		m_ExportDirectory += L"\\AutomationGame\\jsonExporter";
	}
	else
	{
		return AuCarExpErrorCode_CouldNotObtainOutputPathFatal;
	}

	m_ExportDirectory += L"\\";
	std::wstring exportFileName = m_CarData->GetStringData(0)->Value;
	sanitizeFileName(exportFileName);
	m_ExportDirectory += exportFileName;

	//ensure target directory exists:
	DWORD att = GetFileAttributes(m_ExportDirectory.c_str());
	if (att == INVALID_FILE_ATTRIBUTES)
	{
		//create directory, one level at a time:
		size_t slashPos = FindDirDelimiter(m_ExportDirectory, 0);
		size_t offset = 0;

		while (slashPos != std::wstring::npos)
		{
			CreateDirectory(m_ExportDirectory.substr(offset, slashPos - offset).c_str(), nullptr);
			slashPos = FindDirDelimiter(m_ExportDirectory, slashPos + 1);
		}

		//last one:
		CreateDirectory(m_ExportDirectory.c_str(), nullptr);
		att = GetFileAttributes(m_ExportDirectory.c_str());
	}

	if (att != INVALID_FILE_ATTRIBUTES && att & FILE_ATTRIBUTE_DIRECTORY)
	{
		//directory exists, all good to go:
		return AuCarExpErrorCode_Success;
	}
	else
	{
		std::wstring error = L"Could not create directory: ";
		error += m_ExportDirectory;

		MessageBox(nullptr, error.c_str(), TEXT("Error creating directory"), MB_OK);

		return AuCarExpErrorCode_CouldNotObtainOutputPathFatal;
	}
}

void AuExpManager::EndExport() 
{
	m_IsExportInProcess = true;
	ExportInternal();
	m_IsExportInProcess = false;
}

std::vector<std::wstring> AuExpManager::translateLuaKey(const std::wstring& in, bool* isMetaData)
{
	std::wstringstream test(in);
	std::wstring segment;
	std::vector<std::wstring> parts;
	bool meta = *isMetaData;

	while (std::getline(test, segment, LUA_KEY_DELIMITER))
	{
		if (!meta && (METADATA_KEYS.find(segment) != METADATA_KEYS.end()))
		{
			meta = true;
		}
		parts.push_back(GetLuaKeyTranslation(segment));
	}
	(*isMetaData) = meta;
	return parts;
}

void AuExpManager::ExportInternal()
{
	m_FileName = m_CarData->GetStringData(0)->Value;
	sanitizeFileName(m_FileName);
	writeJson(m_CarData->GetBoolData(0)->Value);
}

void AuExpManager::writeJson(bool outputMetadata)
{
	auto dataBuilder = JSONDocumentBuilder::DefaultTranslations();
	auto metadataBuilder = JSONDocumentBuilder::NoTranslations();
	for (auto const& it : m_LuaStringData)
	{
		bool isMetadata = false;
		const std::wstring& fullyQualifiedLuaKey = it.first;
		const std::wstring& luaVal = it.second;
		auto keyParts = translateLuaKey(fullyQualifiedLuaKey, &isMetadata);
		if (outputMetadata && isMetadata)
		{
			metadataBuilder.add(keyParts, luaVal);
		}
		if (!isMetadata)
		{
			dataBuilder.add(keyParts, luaVal);
		}
	}
	std::wstring localPath = AuExpManager::Instance()->GetExportDirectory();
	localPath += L"\\";
	dataBuilder.writeToFile(localPath + L"data.json");
	if (outputMetadata)
	{
		metadataBuilder.writeToFile(localPath + L"metadata.json");
	}
}

void AuExpManager::AddImage(const AuCarExpTexture* texture)
{
	if (!texture)
	{
		return;
	}

	//only add unique textures
	for (unsigned int i = 0; i < m_AllImages.size(); i++)
	{
		if (m_AllImages[i] == texture)
		{
			return;
		}
	}

	m_AllImages.push_back(texture);
}

void AuExpManager::SaveImages()
{
	for (unsigned int i = 0; i < m_AllImages.size(); i++)
	{
		AuExpImage::SaveImageFile(m_AllImages[i], m_ExportDirectory.c_str());
	}
}

void AuExpManager::SaveMesh(const AuCarExpMesh* mesh, const wchar_t* name)
{
	return;
}

void AuExpManager::AddLuaFloatData(const AuCarExpArray<AuCarExpLuaFloatData>& Data)
{
	for (int i = 0; i < Data.GetCount(); i++)
	{
		m_LuaFloatData[Data[i].ValueName] = Data[i].Value;
	}
}

void AuExpManager::AddLuaStringData(const AuCarExpArray<AuCarExpLuaStringData>& Data)
{
	for (int i = 0; i < Data.GetCount(); i++)
	{
		m_LuaStringData[Data[i].ValueName] = Data[i].Buffer;
	}
}

void AuExpManager::AddLuaFiles(const AuCarExpArray<AuCarLuaDataFile>& data)
{
	FileManager fileManager;

	for (int i = 0; i < data.GetCount(); i++)
	{
		m_LuaFDataFiles[data[i].FileName] = data[i];

		AuCarLuaDataFile& File = data[i];

		std::wstring FileName = File.FileName;
		//FileName = TextUtils::SanitiseFileName(FileName);
		//FILE* OutFile; //= m_FileManager->OpenFile(m_Directory + L"/" + FileName, L"wb");
		FILE* OutFile = fileManager.OpenFile((FileName).c_str(), L"wb");
		if (!OutFile)
		{
			return;
		}

		fwrite(File.Buffer, 1, File.BufferSize, OutFile);
	}
}

void AuExpManager::BuildKeyTranslations()
{
	// We have to do this because there seems to be a 64 char limit on the lua strings
	// Longer key names are abbreviated when collecting the data in the Lua script and we can
	// restore the original names here
	m_KeyTranslations[L"CC"] = L"CarCalculator";
	m_KeyTranslations[L"EC"] = L"EngineCalculator";
	m_KeyTranslations[L"EI"] = L"EngineInfo";
	m_KeyTranslations[L"CI"] = L"CarInfo";
	m_KeyTranslations[L"MI"] = L"ModelInfo";
	m_KeyTranslations[L"PI"] = L"PlatformInfo";
	m_KeyTranslations[L"R"] = L"Results";
	m_KeyTranslations[L"4C"] = L"UE4Curves";
	m_KeyTranslations[L"TI"] = L"TrimInfo";
	m_KeyTranslations[L"ENG"] = L"Engineering";
	m_KeyTranslations[L"ENG_G"] = L"EngineeringGrade";
	m_KeyTranslations[L"PROD"] = L"Production";
	m_KeyTranslations[L"ACCD"] = L"AccelerationDetails";
}

const std::wstring& AuExpManager::GetLuaKeyTranslation(const std::wstring& key)
{
	if (m_KeyTranslations.find(key) == m_KeyTranslations.end())
		return key;

	return m_KeyTranslations[key];
}
