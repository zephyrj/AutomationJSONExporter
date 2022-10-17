#pragma once

#include "stdafx.h"

class JSONDocumentBuilder
{
public:
    using CharType = rapidjson::UTF16<>;
    using Document = rapidjson::GenericDocument<CharType>;
    using Value = rapidjson::GenericValue<CharType>;

    static JSONDocumentBuilder NoTranslations();
    static JSONDocumentBuilder DefaultTranslations();

    ~JSONDocumentBuilder() = default;

    JSONDocumentBuilder(const JSONDocumentBuilder&) = default;
    JSONDocumentBuilder(JSONDocumentBuilder&&) = default;

private:
    JSONDocumentBuilder();
    
public:
    void add(const std::vector<std::wstring>& keyParts, const std::wstring& value);
    void addValueTranslation(std::wstring from, std::wstring to);
    void writeToFile(const std::wstring& filepath);

private:
    void setDefaultValueTranslations();
    const std::wstring& getValueTranslation(const std::wstring& value);

private:
    Document m_doc;
    std::unordered_map<std::wstring, std::wstring> m_valueTranslations;

};

