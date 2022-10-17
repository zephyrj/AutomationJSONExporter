/************************************************* *
** Copyright(c) 2021 -- Camshaft Software PTY LTD
** All Rights Reserved
************************************************** */

#pragma once
#include "stdafx.h"
#include "jsonExporter.h"
#include "EXPVERSION.h"
#include <set>

class AuExpManager
{
public:
	static AuExpManager* Instance() { return s_Instance; }
	static void CreateInstance() { if (!s_Instance) s_Instance = new AuExpManager(); }
	static void DestroyInstance() { delete s_Instance; s_Instance = nullptr; }

	static const std::set<std::wstring> METADATA_KEYS;
	static constexpr wchar_t LUA_KEY_DELIMITER = L'.';

	AuCarExpErrorCode Init(const AuCarExpCarData* carData);

	bool IsExportInProcess() { return m_IsExportInProcess; }

	//Getting pixel data is a threaded operation within Automation, which means that we should not assume pixel data within these textures is complete until EndExport() is called
	//So what we do is save the reference and save to file at the end of the export
	void AddImage(const AuCarExpTexture* texture);
	void SaveImages();
	void EndExport();

	void SaveMesh(const AuCarExpMesh* mesh, const wchar_t* name);

	const wchar_t* GetExportDirectory() const { return m_ExportDirectory.c_str(); }

	const std::wstring& AuExpManager::GetLuaKeyTranslation(const std::wstring& key);
	std::vector<std::wstring> translateLuaKey(const std::wstring& in, bool* isMetaData);

	void AddLuaFiles(const AuCarExpArray<AuCarLuaDataFile>& Data);
	void AddLuaStringData(const AuCarExpArray<AuCarExpLuaStringData>& Data);
	void AddLuaFloatData(const AuCarExpArray<AuCarExpLuaFloatData>& Data);

	const wchar_t* GetCarName() const { return m_CarData->GetCarName(); }

private:
	static AuExpManager* s_Instance;

	AuExpManager();
	~AuExpManager() {}

	void BuildKeyTranslations();

	void ExportInternal();
	void writeJson(bool outputLuaFunctionData);
	
	bool m_IsExportInProcess = false;
	const AuCarExpCarData* m_CarData;

	std::vector<const AuCarExpTexture*> m_AllImages;

	std::wstring m_ExportDirectory;

	std::map<std::wstring, int> m_MeshNameCounts;

	std::map<std::wstring, float> m_LuaFloatData;
	std::map<std::wstring, std::wstring> m_LuaStringData;
	std::map<std::wstring, AuCarLuaDataFile> m_LuaFDataFiles;

	std::map<std::wstring, std::wstring> m_KeyTranslations;

	std::wstring m_FileName = L"";

};
