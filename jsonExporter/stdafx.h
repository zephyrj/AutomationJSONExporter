/************************************************* *
** Copyright(c) 2021 -- Camshaft Software PTY LTD
************************************************** */

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <string>
#include <unordered_map>

#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include "resource2.h"

#include "jsonExporter.h"

#include "ExportManager.h"
#include "Image.h"
#include "Mesh.h"
