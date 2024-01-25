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
	  local child_table = tablify(v, key_prefix .. "." .. k)
	  if next(child_table) == nil then
	    data[key_prefix .. "." .. k] = "__empty_table#"
	  else
	    for k2, v2 in pairs(child_table) do data[k2] = v2 end
	  end

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
    CarCalculator.EngineCalculator:CalculateEngine()
	local CarData = tablify(CarCalculator, "CC")
	local EngineFuncData = tablify(CExporter.GetEngineCalculatorAPIFuncData(CarCalculator.EngineCalculator), "FN.EC")
	-- local CurveData = CExporter.GetCurveData(CarCalculator.CarInfo.TrimInfo.EngineInfo.ModelInfo.Results.UE4Curves)
	local CarFiles = CExporter.ExportCarFiles(CarCalculator)
	CarFiles[string.format("%s - %s.car", CarCalculator.CarInfo.PlatformInfo.Name, CarCalculator.CarInfo.TrimInfo.Name)] = CarFile

	local Data = {}
	for k,v in pairs(CarData) do
		Data[k] = v
	end

	for k,v in pairs(EngineFuncData) do
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

function CExporter.GetCurveData(Curves)
   local Data = {}
   for curve_key, curve_data in pairs(Curves) do
	 local child_table = tablify(curve_data, curve_key)
	 if next(child_table) == nil then
	    Data[curve_key] = "__empty_table#"
	 else
	    for k2, v2 in pairs(child_table) do Data[k2] = v2 end
	 end
   end
   return Data
end

-- 
--  UE4Curves = table: 0x0265313c6b00  {NATorque:table: 0x0265313cd240, FrictionCrank:table: 0x026531338880, VolumetricEffeciency:table: 0x0265313cb1c0 (more...)}
function CExporter.GetEngineCalculatorAPIFuncData(EngineCalculator)
  local Data = {}
  --Data.CalcEstimatedCamPeak = EngineCalculator:CalcEstimatedCamPeak(camSettingFraction)
  --Data.CalculateAspirationCost = EngineCalculator:CalculateAspirationCost()
  --Data.CalculateAspirationEngineeringTime = EngineCalculator:CalculateAspirationEngineeringTime()
  --Data.CalculateAspirationHours = EngineCalculator:CalculateAspirationHours()
  --Data.CalculateAspirationWeight = EngineCalculator:CalculateAspirationWeight()
  --Data.CalculateBalanceShaftCost = EngineCalculator:CalculateBalanceShaftCost()
  --Data.CalculateAspirationCost = EngineCalculator:CalculateAspirationCost()
  --Data.CalculateBalanceShaftWeight = EngineCalculator:CalculateBalanceShaftWeight()
  --Data.CalculateBaseMajorToolingCosts = EngineCalculator:CalculateBaseMajorToolingCosts()
  --Data.CalculateBaseMinorToolingCosts = EngineCalculator:CalculateBaseMinorToolingCosts()
  --Data.CalculateBaseToolingCosts = EngineCalculator:CalculateBaseToolingCosts()
  --Data.CalculateBaseTorque = EngineCalculator:CalculateBaseTorque()
  --Data.CalculateBlockCost = EngineCalculator:CalculateBlockCost()
  --Data.CalculateBlockEngineeringTime = EngineCalculator:CalculateBlockEngineeringTime()
  --Data.CalculateBlockHours = EngineCalculator:CalculateBlockHours()
  --Data.CalculateBlockWeight = EngineCalculator:CalculateBlockWeight()
  --Data.CalculateCatConverterCost = EngineCalculator:CalculateCatConverterCost()
  --Data.CalculateCatConverterHours = EngineCalculator:CalculateCatConverterHours()
  --Data.CalculateCatConverterWeight = EngineCalculator:CalculateCatConverterWeight()
  --Data.CalculateCoolingRequired = EngineCalculator:CalculateCoolingRequired()
  --Data.CalculateCrankCost = EngineCalculator:CalculateCrankCost()
  --Data.CalculateCrankEngineeringTime = EngineCalculator:CalculateCrankEngineeringTime()
  --Data.CalculateCrankHours = EngineCalculator:CalculateCrankHours()
  --Data.CalculateCrankWeight = EngineCalculator:CalculateCrankWeight()
  --Data.CalculateEFIFlowLimit = EngineCalculator:CalculateEFIFlowLimit()
  --Data.CalculateEmissions = EngineCalculator:CalculateEmissions()
  --Data.CalculateEngine = EngineCalculator:CalculateEngine()
  --Data.CalculateEngineCapacity = EngineCalculator:CalculateEngineCapacity()
  --Data.CalculateEngineCost = EngineCalculator:CalculateEngineCost()
  --Data.CalculateEngineHours = EngineCalculator:CalculateEngineHours()
  --Data.CalculateEngineMTTF = EngineCalculator:CalculateEngineMTTF()
  --Data.CalculateEngineMiscEngineeringTime = EngineCalculator:CalculateEngineMiscEngineeringTime()
  --Data.GetAFRValue = EngineCalculator:GetAFRValue()
  --Data.GetAirflowPartRatingAtRPM = EngineCalculator:GetAirflowPartRatingAtRPM()
  --Data.GetAllAvailableFuels = EngineCalculator:GetAllAvailableFuels()
  --Data.GetAllSelectedPartNamesForSection = EngineCalculator:GetAllSelectedPartNamesForSection()
  --Data.GetAllSelectedParts = EngineCalculator:GetAllSelectedParts()
  Data.GetAspirationBoostControls = EngineCalculator:GetAspirationBoostControls()
  Data.GetAspirationItemOptions_1 = EngineCalculator:GetAspirationItemOptions_1()
  Data.GetAspirationItemOptions_2 = EngineCalculator:GetAspirationItemOptions_2()
  Data.GetAspirationItemSubOptions_1 = EngineCalculator:GetAspirationItemSubOptions_1()
  Data.GetAspirationItemSubOptions_2 = EngineCalculator:GetAspirationItemSubOptions_2()
  Data.GetAspirationPartsCostSummary = EngineCalculator:GetAspirationPartsCostSummary()
  Data.GetAspirationSetups = EngineCalculator:GetAspirationSetups()
  Data.GetAspirationTechPool = EngineCalculator:GetAspirationTechPool()
  Data.GetAspirationTypes = EngineCalculator:GetAspirationTypes()
  Data.GetBalanceShaftParts = EngineCalculator:GetBalanceShaftParts()
  Data.GetBaseImbalance = EngineCalculator:GetBaseImbalance()
  Data.GetBlockEngineeringGrade = EngineCalculator:GetBlockEngineeringGrade()
  Data.GetBlockMaterialParts = EngineCalculator:GetBlockMaterialParts()
  Data.GetBlockParts = EngineCalculator:GetBlockParts()
  Data.GetBlockPartsCostSummary = EngineCalculator:GetBlockPartsCostSummary()
  Data.GetBlockToolingGrade = EngineCalculator:GetBlockToolingGrade()
  Data.GetBlockTypes = EngineCalculator:GetBlockTypes()
  Data.GetBore = EngineCalculator:GetBore()
  Data.GetBottomEndPartsCostSummary = EngineCalculator:GetBottomEndPartsCostSummary()
  Data.GetBottomEndTechPool = EngineCalculator:GetBottomEndTechPool()
  --Data.GetCamDurationAtRPM = EngineCalculator:GetCamDurationAtRPM()
  local cam_min, cam_max = EngineCalculator:GetCamProfileMinMax()
  Data.GetCamProfileMinMax = {}
  Data.GetCamProfileMinMax.Min = cam_min
  Data.GetCamProfileMinMax.Max = cam_max
  --Data.GetCamSetting = EngineCalculator:GetCamSetting(vvl, inRPM)
  Data.GetCatalyticConverterParts = EngineCalculator:GetCatalyticConverterParts()
  Data.GetChargerMaxBoostLimits_1 = EngineCalculator:GetChargerMaxBoostLimits_1()
  Data.GetChargerMaxBoostLimits_2 = EngineCalculator:GetChargerMaxBoostLimits_2()
  Data.GetChargerMaxBoost_1 = EngineCalculator:GetChargerMaxBoost_1()
  Data.GetChargerMaxBoost_2 = EngineCalculator:GetChargerMaxBoost_2()
  Data.GetChargerSizeLimits_1 = EngineCalculator:GetChargerSizeLimits_1()
  Data.GetChargerSizeLimits_2 = EngineCalculator:GetChargerSizeLimits_2()
  Data.GetChargerSize_1 = EngineCalculator:GetChargerSize_1()
  Data.GetChargerSize_2 = EngineCalculator:GetChargerSize_2()
  Data.GetChargerTuneLimits_1 = EngineCalculator:GetChargerTuneLimits_1()
  Data.GetChargerTuneLimits_2 = EngineCalculator:GetChargerTuneLimits_2()
  Data.GetChargerTune_1 = EngineCalculator:GetChargerTune_1()
  Data.GetChargerTune_2 = EngineCalculator:GetChargerTune_2()
  Data.GetConrodParts = EngineCalculator:GetConrodParts()
  Data.GetCrankParts = EngineCalculator:GetCrankParts()
  Data.GetCrankStrengthMultiplier = EngineCalculator:GetCrankStrengthMultiplier()
  Data.GetCurrentFuelInfo = EngineCalculator:GetCurrentFuelInfo()
  Data.GetEngineVolume = EngineCalculator:GetEngineVolume()
  Data.GetEstimatedCamPeak = EngineCalculator:GetEstimatedCamPeak()
  Data.GetEstimatedVVLCamPeak = EngineCalculator:GetEstimatedVVLCamPeak()
  Data.GetExhaustBackPressure = EngineCalculator:GetExhaustBackPressure()
  Data.GetExhaustBypassValveParts = EngineCalculator:GetExhaustBypassValveParts()
  Data.GetExhaustCountParts = EngineCalculator:GetExhaustCountParts()
  Data.GetExhaustPartsCostSummary = EngineCalculator:GetExhaustPartsCostSummary()
  Data.GetExhaustResonanceSettings = EngineCalculator:GetExhaustResonanceSettings()
  Data.GetExhaustTechPool = EngineCalculator:GetExhaustTechPool()
  Data.GetExhaustValvesKW = EngineCalculator:GetExhaustValvesKW()
  --Data.GetFactory = EngineCalculator:GetFactory()
  --Data.GetFactoryIndex = EngineCalculator:GetFactoryIndex()
  --Data.GetFailureFractionForBottomEndPart = EngineCalculator:GetFailureFractionForBottomEndPart(partInfo, engineRPM, EngineResults)
  Data.GetFamilyMaxCapacity = EngineCalculator:GetFamilyMaxCapacity()
  Data.GetFamilyTechPool = EngineCalculator:GetFamilyTechPool()
  Data.GetFamilyYear = EngineCalculator:GetFamilyYear()
  Data.GetFirstMufflerParts = EngineCalculator:GetFirstMufflerParts()
  Data.GetFrictionValues = EngineCalculator:GetFrictionValues(1000)
  Data.GetFuelSystemParts = EngineCalculator:GetFuelSystemParts()
  Data.GetFuelSystemPartsCostSummary = EngineCalculator:GetFuelSystemPartsCostSummary()
  Data.GetFuelSystemTechPool = EngineCalculator:GetFuelSystemTechPool()
  Data.GetFuelSystemTypeParts = EngineCalculator:GetFuelSystemTypeParts()
  Data.GetHeadEmissionsGrade = EngineCalculator:GetHeadEmissionsGrade()
  Data.GetHeadEngineeringGrade = EngineCalculator:GetHeadEngineeringGrade()
  Data.GetHeadMaterialParts = EngineCalculator:GetHeadMaterialParts()
  Data.GetHeadParts = EngineCalculator:GetHeadParts()
  Data.GetHeadToolingGrade = EngineCalculator:GetHeadToolingGrade()
  Data.GetHeadTotalWeight = EngineCalculator:GetHeadTotalWeight()
  Data.GetHeaderParts = EngineCalculator:GetHeaderParts()
  Data.GetIntakeManifoldConfigParts = EngineCalculator:GetIntakeManifoldConfigParts()
  Data.GetIntakeParts = EngineCalculator:GetIntakeParts()
  Data.GetIntakeResonanceSettings = EngineCalculator:GetIntakeResonanceSettings()
  Data.GetIntakeValvesKW = EngineCalculator:GetIntakeValvesKW()
  Data.GetIntercoolerAirFlowInKGS = EngineCalculator:GetIntercoolerAirFlowInKGS()
  Data.GetIntercoolerEfficiency = EngineCalculator:GetIntercoolerEfficiency()
  Data.GetIntercoolerEngineBayVolume = EngineCalculator:GetIntercoolerEngineBayVolume()
  Data.GetIntercoolerPower = EngineCalculator:GetIntercoolerPower()
  Data.GetIntercoolerVolume = EngineCalculator:GetIntercoolerVolume()
  Data.GetListedOctaneFuellist = EngineCalculator:GetListedOctaneFuellist()
  Data.GetMaxBore = EngineCalculator:GetMaxBore()
  --Data.GetMaxRPM = EngineCalculator:GetMaxRPM()  Seems to be a bug in this
  --Data.GetMaxRPMPart = EngineCalculator:GetMaxRPMPart(part)
  Data.GetMaxStroke = EngineCalculator:GetMaxStroke()
  --Data.GetMaxVariantBore = EngineCalculator:GetMaxVariantBore()
  --Data.GetMaxVariantStroke = EngineCalculator:GetMaxVariantStroke()
  Data.GetMinBore = EngineCalculator:GetMinBore()
  Data.GetMinStroke = EngineCalculator:GetMinStroke()
  --Data.GetMinVariantBore = EngineCalculator:GetMinVariantBore()
  --Data.GetMinVariantStroke = EngineCalculator:GetMinVariantStroke()
  Data.GetMiscEngineCost = EngineCalculator:GetMiscEngineCost()
  --Data.GetModelBore = EngineCalculator:GetModelBore()
  --Data.GetModelBoreSetting = EngineCalculator:GetModelBoreSetting()
  ----Data.GetModelStroke = EngineCalculator:GetModelStroke()
  --Data.GetModelStrokeSetting = EngineCalculator:GetModelStrokeSetting()
  --Data.GetMostUsedTechPoolForSection = EngineCalculator:GetMostUsedTechPoolForSection(sectionName)
  Data.GetNaturallyAspiratedEnginePowerPeak = EngineCalculator:GetNaturallyAspiratedEnginePowerPeak()
  Data.GetPanelFractionalContributionToET = EngineCalculator:GetPanelFractionalContributionToET()
  Data.GetPanelRunningCostsContribution = EngineCalculator:GetPanelRunningCostsContribution()
  --Data.GetPart = EngineCalculator:GetPart()
  --Data.GetPartByName = EngineCalculator:GetPartByName()
  --Data.FN.EngineCalculator.GetPartIndex = EngineCalculator:GetPartIndex()
  --Data.GetPartInfoFromName = EngineCalculator:GetPartInfoFromName()
  --Data.GetPartRelabilityFactorAtRPM = EngineCalculator:GetPartRelabilityFactorAtRPM(rpm)
  --Data.GetPartsUnlockedByTechpool = EngineCalculator:GetPartsUnlockedByTechpool()
  --Data.GetPartRelabilityFactorAtTorque = EngineCalculator:GetPartRelabilityFactorAtTorque()
  Data.GetPistonParts = EngineCalculator:GetPistonParts()
  --Data.GetPowerAtRPM = EngineCalculator:GetPowerAtRPM(rpm)
  Data.GetPowerBulgeRPM = EngineCalculator:GetPowerBulgeRPM()
  Data.GetPrettyFileName = EngineCalculator:GetPrettyFileName()
  --Data.GetQualiltyOnlyForPart = EngineCalculator:GetQualiltyOnlyForPart()
  --Data.GetQualityForPart = EngineCalculator:GetQualityForPart()
  --Data.GetQualityForPurchasedPart = EngineCalculator:GetQualityForPurchasedPart()
  --Data.GetQualitySliderTooltipInfo = EngineCalculator:GetQualitySliderTooltipInfo()
  Data.GetSecondMufflerParts = EngineCalculator:GetSecondMufflerParts()
  Data.GetServiceCost = EngineCalculator:GetServiceCost()
  Data.GetStolenTorque = EngineCalculator:GetStolenTorque()
  Data.GetShortName = EngineCalculator:GetShortName()
  Data.GetStroke = EngineCalculator:GetStroke()
  --Data.GetTechPoolForSection = EngineCalculator:GetTechPoolForSection()
  Data.GetTopEndPartsCostSummary = EngineCalculator:GetTopEndPartsCostSummary()
  --Data.GetTorqueAtPoint = EngineCalculator:GetTorqueAtPoint()
  Data.GetTopEndTechPool = EngineCalculator:GetTopEndTechPool()
  --Data.GetTorqueAtRPM = EngineCalculator:GetTorqueAtRPM()
  --Data.GetTorqueForPart = EngineCalculator:GetTorqueForPart()
  Data.GetTotalEngineCostSummary = EngineCalculator:GetTotalEngineCostSummary()
  Data.GetTurbineSizeLimits_1 = EngineCalculator:GetTurbineSizeLimits_1()
  Data.GetTurbineSizeLimits_2 = EngineCalculator:GetTurbineSizeLimits_2()
  Data.GetTurbineSizeMultiplier_1 = EngineCalculator:GetTurbineSizeMultiplier_1()
  Data.GetTurbineSizeMultiplier_2 = EngineCalculator:GetTurbineSizeMultiplier_2()
  --Data.GetUsedTechPoolForPart = EngineCalculator:GetUsedTechPoolForPart()
  local vvl_cam_min, vvl_cam_max = EngineCalculator:GetVVLCamProfileMinMax()
  Data.GetVVLCamProfileMinMax = {}
  Data.GetVVLCamProfileMinMax.Min = vvl_cam_min
  Data.GetVVLCamProfileMinMax.Min = vvl_cam_max
  Data.GetVVLParts = EngineCalculator:GetVVLParts()
  local vvl_rpm_min, vvl_rpm_max = EngineCalculator:GetVVLRPMMinMax()
  Data.GetVVLRPMMinMax = {}
  Data.GetVVLRPMMinMax.Min = vvl_rpm_min
  Data.GetVVLRPMMinMax.Min = vvl_rpm_max
  Data.GetVVTParts = EngineCalculator:GetVVTParts()
  --Data.GetValveFloatCurve = EngineCalculator:GetValveFloatCurve(PartName, RPM)
  Data.GetVariantsForgedCount = EngineCalculator:GetVariantsForgedCount()
  Data.GetValveParts = EngineCalculator:GetValveParts()
  Data.GetVariantUID = EngineCalculator:GetVariantUID()
  Data.GetVariantsCnCCount = EngineCalculator:GetVariantsCnCCount()
  Data.IsEngineCompleted = EngineCalculator:IsEngineCompleted()
  Data.IsEngineTurbo = EngineCalculator:IsEngineTurbo()
  Data.IsFamilyComplete = EngineCalculator:IsFamilyComplete()
  Data.IsIgnitionTimineDisabled = EngineCalculator:IsIgnitionTimineDisabled()
  Data.RPMEngineeringPenalty = EngineCalculator:RPMEngineeringPenalty()
  --Data.CalculateFuelFlowLimit = EngineCalculator:CalculateFuelFlowLimit(1000)
  --Data.CalculateFuelFlowLimitInKGS = EngineCalculator:CalculateFuelFlowLimitInKGS(1000)
  --Data.GetAFRValue = EngineCalculator:GetAFRValue(10)

  -- Can't get this to return anything
  -- GetEngineGraphResultsValueByRPMAndThrottle(CurveName, CurvesIter, lerpValue, InThrottle)
  -- local lerpVal = 1
  -- local result = { EngineCalculator:GetEngineGraphResultsValueByRPMAndThrottle("Econ", 10, lerpVal, 0.9) }

  --Data.StaticCalculateEngineSize = EngineCalculator.StaticCalculateEngineSize()
  --Data.StaticCalculateEngineSizeSeperate = EngineCalculator.StaticCalculateEngineSizeSeperate()
  --Data.StaticGetEngineScale = EngineCalculator.StaticGetEngineScale()
  --Data.StaticGetPartByName = EngineCalculator.StaticGetPartByName()
  --Data.TorqueTwist = EngineCalculator:TorqueTwist(engineRPM)
  return Data
  --              "GetYear": "function",
  --              "GetYearForPart": "function",
  --              "CalculateEngineMiscEngineeringTime": "function",
  --              "CalculateEngineScales": "function",
  --              "CalculateEngineScalesBottomEnd": "function",
  --              "CalculateEngineWeight": "function",
  --              "CalculateEngineeringQualityMulti": "function",
  --               "CalculateEngineeringTime": "function",
  --              "CalculateEnginesMadePerDay": "function",
  --              "CalculateExhaustBypassValvesEngineeringTime": "function",
  --              "CalculateExhaustDiameter": "function",
  --              "CalculateExhaustKW": "function",
  --              "CalculateExhaustLT": "function",
  --              "CalculateExhaustLTInKGS": "function",
  --              "CalculateExhaustNoise": "function",
  --              "CalculateExhaustPowerPenalty": "function",
  --              "CalculateExhaustsEngineeringTime": "function",
  --              "CalculateFamilyCapacityFromLoadedInfo": "function",
  --              "CalculateFamilyEngineeringTime": "function",
  --              "CalculateFilterHours": "function",
  --              "CalculateFlyWheelWeight": "function",
  --              "CalculateFuelFlowLimit": "function",
  --              "CalculateFuelFlowLimitInKGS": "function",
  --              "CalculateFuelSystemCost": "function",
  --              "CalculateFuelSystemEngineeringTime": "function",
  --             "CalculateFuelSystemHours": "function",
  --              "CalculateFuelSystemWeight": "function",
  --              "CalculateHeadCost": "function",
  --              "CalculateHeadEngineeringTime": "function",
  --              "CalculateHeadHours": "function",
   --             "CalculateHeadWeight": "function",
 --            "CalculateHeaderFlowLimit": "function",
 --             "CalculateHeaderFlowLimitInKGS": "function",
 --               "CalculateHeadersCost": "function",
 --               "CalculateHeadersEngineeringTime": "function",
 --               "CalculateHeadersHours": "function",
 --               "CalculateHeadersKW": "function",
--                "CalculateHeadersWeight": "function",
--                "CalculateHourMulti": "function",
--                "CalculateIdleRPM": "function",
--                "CalculateIgnitionTimingPowerFactor": "function",
--                "CalculateIntakeAirLimit": "function",
--                "CalculateIntakeAirLimitInKGS": "function",
--                "CalculateIntakeCost": "function",
--                "CalculateIntakeKW": "function",
--                "CalculateIntakeValvesFlowLimit": "function",
--                "CalculateIntakeWeight": "function",
--                "CalculateIntercoolerCost": "function",
--                "CalculateIntercoolerEngineeringTime": "function",
--                "CalculateIntercoolerHours": "function",
--                "CalculateIntercoolerSliderFromKWBinarySearch": "function",
--                "CalculateIntercoolerWeight": "function",
--                "CalculateMechanicalSmoothness": "function",
--                "CalculateMinVariantCapacity": "function",
--                "CalculateMiscEngineHours": "function",
--                "CalculateMuffler1Cost": "function",
--                "CalculateMuffler1EngineeringTime": "function",
--                "CalculateMuffler1Hours": "function",
--                "CalculateMuffler1Weight": "function",
--                "CalculateMuffler2Cost": "function",
--                "CalculateMuffler2EngineeringTime": "function",
--                "CalculateMuffler2Hours": "function",
--                "CalculateMuffler2Weight": "function",
--                "CalculateNA": "function",
--                "CalculateNALossesForTurbo": "function",
--                "CalculateNoise": "function",
--                "CalculateNoiseSaveToResults": "function",
--                "CalculatePartEngineeringTime": "function",
--                "CalculatePartEngineeringTimeFromPart": "function",
--                "CalculatePartFailureInfo": "function",
--                "CalculatePerformanceIndex": "function",
--                "CalculatePistonsCost": "function",
--                "CalculatePistonsEngineeringTime": "function",
--                "CalculatePistonsHours": "function",
--                "CalculatePistonsWeight": "function",
--                "CalculateProductionQualityMulti": "function",
--                "CalculateProductionReliability": "function",
--                "CalculateQualityDifference": "function",
--                "CalculateResponsiveness": "function",
--                "CalculateRodsCost": "function",
--                "CalculateRodsEngineeringTime": "function",
--                "CalculateRodsHours": "function",
--                "CalculateRodsWeight": "function",
--                "CalculateSmoothness": "function",
--                "CalculateThermalEff": "function",
--                "CalculateVVLCost": "function",
--                "CalculateVVLEngineeringTime": "function",
--                "CalculateVVLHours": "function",
--                "CalculateVVLWeight": "function",
--                "CalculateVVTCost": "function",
--                "CalculateVVTEngineeringTime": "function",
--                "CalculateVVTHours": "function",
--                "CalculateVVTWeight": "function",
--                "CalculateValveFlowRatingInKGS": "function",
--                "CaluclateCarbFlowLimit": "function",
end


