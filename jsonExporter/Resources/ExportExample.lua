---------------------------------------------------
-- Copyright (c) 2021 -- Camshaft Software PTY LTD
---------------------------------------------------

function tableHasKey(table,key)
    return table[key] ~= nil
end

function createSafeKey(k, idx)
  local key_type = type(k) 
  local key_name = "meta" .. idx .. key_type
  if key_type == "string" then 
	key_name = k
  elseif (key_type == "boolean" or key_type == "number") then
	  key_name = tostring(k) 
  end
  return key_name
end

function metadataExplore(mt, depth)
  local data = {}
  local idx = 1
  local max_depth = 3
  if depth > max_depth then
    return {}
  end
  for k, v in pairs(mt) do
    local var_type = type(v)
	local key_name = createSafeKey(k, idx)
	if var_type == "string" then
	  data[key_name] = v
	elseif (var_type == "boolean" or var_type == "number") then
	  data[key_name] = tostring(v)
	elseif (var_type == "table" and (not(key_name == "__index"))) then
	  for k3, v3 in pairs(metadataExplore(v, depth+1)) do
	    data[key_name .. "." .. k3] = v3 
	  end
	else
	  data[key_name] = var_type
	end
	idx = idx + 1
  end
  return data
end


function tablify(table, key_prefix)
  local key_mapping = {EngineCalculator="EC", EngineInfo="EI", CarInfo="CI", ModelInfo="MI", PlatformInfo="PI", Results="R", UE4Curves="4C", TrimInfo="TI", Engineering="ENG", EngineeringGrade="ENG_G", Production="PROD",
                       AccelerationDetails="ACCD", }
  local data = {}
  for k, v in pairs(table) do
    local var_type = type(v)
	if tableHasKey(key_mapping, k) then
	  k = key_mapping[k]
	end
	if var_type == "table" then
	  for k2, v2 in pairs(tablify(v, key_prefix .. "." .. k)) do data[k2] = v2 end
	elseif var_type == "string" then
	  data[key_prefix .. "." .. k] = v
	elseif (var_type == "boolean" or var_type == "number") then
	  data[key_prefix .. "." .. k] = tostring(v)
	else
	  data[key_prefix .. "." .. k] = var_type
	end
  end
  if not (table.__index == nil) then
    if type(table.__index) == "table" then
      for k4, v4 in pairs(metadataExplore(table.__index, 1)) do data[key_prefix .. ".__index." .. k4] = v4 end
	else
      data[key_prefix .. ".__index"] = type(table.__index)
	end
  end
  return data
end

function DoExport(CarCalculator, CarFile)
	local CarData = tablify(CarCalculator, "CC")
	local CarFiles = CExporter.ExportCarFiles(CarCalculator)
	CarFiles[string.format("%s - %s.car", CarCalculator.CarInfo.PlatformInfo.Name, CarCalculator.CarInfo.TrimInfo.Name)] = CarFile

	local Data = {}
	for k,v in pairs(CarData) do
		Data[k] = v
	end

	local Files = {}
	for k,v in pairs(CarFiles) do
		Files[k] = v
	end

	return Files, Data
end


if CExporter == nil then
	CExporter = {}
	CExporter.__index = CExporter
end

function CExporter.ExportCarFiles(CarCalculator)
	Files = { }
	return Files
end
