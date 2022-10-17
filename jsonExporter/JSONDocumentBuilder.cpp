#include "stdafx.h"
#include "JSONDocumentBuilder.h"

namespace {
    struct CloseFileOnExitScope {
        explicit CloseFileOnExitScope(FILE* file) : m_file(file) {}
        ~CloseFileOnExitScope() { fclose(m_file); }
        FILE* m_file;
    };
}

JSONDocumentBuilder JSONDocumentBuilder::NoTranslations()
{
	return JSONDocumentBuilder();
}

JSONDocumentBuilder JSONDocumentBuilder::DefaultTranslations()
{
	JSONDocumentBuilder b;
	b.setDefaultValueTranslations();
	return b;
}

JSONDocumentBuilder::JSONDocumentBuilder() 
    : m_doc()
{
    m_doc.SetObject();
}

void JSONDocumentBuilder::addValueTranslation(std::wstring from, std::wstring to)
{
    m_valueTranslations.emplace(std::move(from), std::move(to));
}

void JSONDocumentBuilder::add(const std::vector<std::wstring>& keyParts, const std::wstring& value) {
    auto& allocator = m_doc.GetAllocator();
    Value* currentParent = &m_doc;
    for (auto it{ keyParts.begin() }; it != std::prev(keyParts.end()); it++) {
        const auto& part = *it;
        if (!currentParent->HasMember(part.c_str()))
        {
            Value key;
            key.SetString(part.c_str(), static_cast<rapidjson::SizeType>(part.length()), allocator);
            Value object(rapidjson::kObjectType);
            currentParent->AddMember(key, object, allocator);
        }
        currentParent = &(currentParent->FindMember(part.c_str())->value);
    }

    const auto& key_name = keyParts.back();
    Value k;
    k.SetString(key_name.c_str(), static_cast<rapidjson::SizeType>(key_name.length()), allocator);

    const auto translatedVal = getValueTranslation(value);
    Value v;
    v.SetString(translatedVal.c_str(), static_cast<rapidjson::SizeType>(translatedVal.length()), allocator);
    if (currentParent->IsObject()) {
        currentParent->AddMember(k, v, allocator);
    }
    else {
        // Fallback in-case something weird happens
        m_doc.AddMember(k, v, allocator);
    }
}

void JSONDocumentBuilder::writeToFile(const std::wstring& filepath)
{
    FILE* file = nullptr;
    errno_t err = _wfopen_s(&file, filepath.c_str(), L"wb");
    if (!file)
    {
        MessageBox(nullptr, filepath.c_str(), TEXT("Error creating directory"), MB_OK);
        return;
    }

    try
    {
        CloseFileOnExitScope f(file);
        char writeBuffer[65536];
        rapidjson::FileWriteStream os(file, writeBuffer, sizeof(writeBuffer));
        rapidjson::PrettyWriter<rapidjson::FileWriteStream, CharType> writer(os);
        m_doc.Accept(writer);
    }
    catch (const std::exception& e)
    {
        MessageBox(nullptr, filepath.c_str(), TEXT("Write Error"), MB_OK);
    }
}

void JSONDocumentBuilder::setDefaultValueTranslations()
{
	m_valueTranslations[L"Chassis_Monocoque_Name"] = L"Monocoque";
	m_valueTranslations[L"Chassis_Ladder_Name"] = L"Ladder";
	m_valueTranslations[L"Chassis_SemiSpaceFrame_Name"] = L"Semi-Spaceframe";
	m_valueTranslations[L"Chassis_SpaceFrame_Name"] = L"Space Frame";
	m_valueTranslations[L"Chassis_LightTruckMonocoque_Name"] = L"Light Truck Monocoque";

	m_valueTranslations[L"F"] = L"Front";
	m_valueTranslations[L"R"] = L"Rear";
	m_valueTranslations[L"M"] = L"Middle";

	m_valueTranslations[L"ChassisMat_CRESteel_Name"] = L"Corrosion Resistant Steel";
	m_valueTranslations[L"ChassisMat_AHSSteel_Name"] = L"AHS Steel";
	m_valueTranslations[L"ChassisMat_AHSLight_Name"] = L"Light AHS Steel";
	m_valueTranslations[L"ChassisMat_Steel_Name"] = L"Steel";
	m_valueTranslations[L"ChassisMat_Alu_Name"] = L"Aluminum";
	m_valueTranslations[L"ChassisMat_GalvanizedSteel_Name"] = L"Galvanized Steel";
	m_valueTranslations[L"ChassisMat_CarFibre_Name"] = L"Carbon Fiber";
	m_valueTranslations[L"ChassisMat_GluedAlu_Name"] = L"Glued Aluminum";

	m_valueTranslations[L"Suspend_SolAxCoil_Name"] = L"Coil Solid Axle";
	m_valueTranslations[L"Suspend_SolAxLeaf_Name"] = L"Leaf Solid Axle";
	m_valueTranslations[L"Suspend_DblWishbone_Name"] = L"Double Wishbone";
	m_valueTranslations[L"Suspend_Pushrod_Name"] = L"Pushrod";
	m_valueTranslations[L"Suspend_Multilink_Name"] = L"Multilink";
	m_valueTranslations[L"Suspend_SemiTrailArm_Name"] = L"Semi-Trailing Arm";
	m_valueTranslations[L"Suspend_MacPher_Name"] = L"MacPherson";
	m_valueTranslations[L"Suspend_Torsion_Name"] = L"Torsion Beam";

	m_valueTranslations[L"PanelMat_Steel_Name"] = L"Steel";
	m_valueTranslations[L"PanelMat_TreatedSteel_Name"] = L"Treated Steel";
	m_valueTranslations[L"PanelMat_Alu_Name"] = L"Aluminum";
	m_valueTranslations[L"PanelMat_PartialAlu_Name"] = L"Partial Aluminum";
	m_valueTranslations[L"PanelMat_PartialCF_Name"] = L"Partial Carbon Fiber";
	m_valueTranslations[L"PanelMat_FibreGlass_Name"] = L"Fiber Glass";
	m_valueTranslations[L"PanelMat_CarbonFibre_Name"] = L"Carbon Fiber";

	m_valueTranslations[L"TRAV"] = L"Transverse";
	m_valueTranslations[L"LONG"] = L"Longitudinal";

	m_valueTranslations[L"DriveType_TransFWD_Name"] = L"Transverse FWD";
	m_valueTranslations[L"DriveType_LongFWD_Name"] = L"Longitudinal FWD";
	m_valueTranslations[L"DriveType_Trans4WD_Name"] = L"Transverse AWD";
	m_valueTranslations[L"DriveType_TransRWD_Name"] = L"Transverse RWD";
	m_valueTranslations[L"DriveType_Long4WD_Name"] = L"Longitudinal AWD";
	m_valueTranslations[L"DriveType_LongRWD_Name"] = L"Longitudinal RWD";
	m_valueTranslations[L"DriveType_Long4X4_Name"] = L"Longitudinal 4x4";

	m_valueTranslations[L"GearboxType_Sequential_Name"] = L"Sequential";
	m_valueTranslations[L"GearboxType_AutoManual_Name"] = L"Automanual";
	m_valueTranslations[L"GearboxType_SequentialDblClutch_Name"] = L"Double Clutch";
	m_valueTranslations[L"GearboxType_Auto_Name"] = L"Automatic";
	m_valueTranslations[L"GearboxType_AutoAdv_Name"] = L"Advanced Automatic";
	m_valueTranslations[L"GearboxType_Manual_Name"] = L"Manual";

	m_valueTranslations[L"Diff_Standard_Name"] = L"Open";
	m_valueTranslations[L"Diff_Locker_Name"] = L"Locking";
	m_valueTranslations[L"Diff_AutoLocker_Name"] = L"Autolocking";
	m_valueTranslations[L"Diff_GearedLSD_Name"] = L"Geared LSD";
	m_valueTranslations[L"Diff_ClutchedLSD_Name"] = L"Clutched LSD";
	m_valueTranslations[L"Diff_ViscLSD_Name"] = L"Viscous LSD";
	m_valueTranslations[L"Diff_ElecLSD_Name"] = L"Electric LSD";

	m_valueTranslations[L"EBodyType::EBT_COUPE"] = L"Coupe";
	m_valueTranslations[L"EBodyType::EBT_HATCHBACK"] = L"Hatchback";
	m_valueTranslations[L"EBodyType::EBT_WAGON"] = L"Station Wagon";
	m_valueTranslations[L"EBodyType::EBT_TRUCK"] = L"Pickup";
	m_valueTranslations[L"EBodyType::EBT_VAN"] = L"Van";
	m_valueTranslations[L"EBodyType::EBT_SUV"] = L"SUV";
	m_valueTranslations[L"EBodyType::EBT_PEOPLEMOVER"] = L"People Mover";
	m_valueTranslations[L"EBodyType::EBT_SEDAN"] = L"Sedan";
	m_valueTranslations[L"EBodyType::EBT_CONVERTIBLE"] = L"Convertible";

	m_valueTranslations[L"ConvertibleTypes_None_Name"] = L"None";
	m_valueTranslations[L"ConvertibleTypes_RemovableSoft_Name"] = L"Detachable Soft Top";
	m_valueTranslations[L"ConvertibleTypes_RemovableHard_Name"] = L"Detachable Hard Top";
	m_valueTranslations[L"ConvertibleTypes_ManualSoft_Name"] = L"Manual Soft Top";
	m_valueTranslations[L"ConvertibleTypes_ManualHard_Name"] = L"Manual Hard Top";
	m_valueTranslations[L"ConvertibleTypes_AutomaticSoft_Name"] = L"Automatic Soft Top";
	m_valueTranslations[L"ConvertibleTypes_AutomaticHard_Name"] = L"Automatic Hard Top";
	m_valueTranslations[L"ConvertibleTypes_AutomaticAdv_Name"] = L"Hidden Automatic Soft Top";

	m_valueTranslations[L"ActiveAero_None_Name"] = L"None";
	m_valueTranslations[L"ActiveAero_Wing_Name"] = L"Active Wing";
	m_valueTranslations[L"ActiveAero_Cooling_Name"] = L"Active Cooling";

	m_valueTranslations[L"Undertray_None_Name"] = L"None";
	m_valueTranslations[L"Undertray_Offroad_Name"] = L"Offroad";
	m_valueTranslations[L"Undertray_SemiClad_Name"] = L"Semi-Clad";
	m_valueTranslations[L"Undertray_FullyClad_Name"] = L"Fully Clad";
	m_valueTranslations[L"Undertray_Sport_Name"] = L"Sport";
	m_valueTranslations[L"Undertray_FlowOptimized_Name"] = L"Flow Optimized";
	m_valueTranslations[L"Undertray_Race_Name"] = L"Race";

	m_valueTranslations[L"Interior_Basic_Name"] = L"Basic";
	m_valueTranslations[L"Interior_Standard_Name"] = L"Standard";
	m_valueTranslations[L"Interior_Premium_Name"] = L"Premium";
	m_valueTranslations[L"Interior_SuperLight_Name"] = L"Sport";
	m_valueTranslations[L"Interior_Luxury_Name"] = L"Luxury";
	m_valueTranslations[L"Interior_HandMade_Name"] = L"Hand Made";


	m_valueTranslations[L"Entertain_Basic_HUD_Name"] = L"Basic HUD";
	m_valueTranslations[L"Entertain_Standard_HUD_Name"] = L"Standard HUD";
	m_valueTranslations[L"Entertain_Premium_HUD_Name"] = L"Premium HUD";
	m_valueTranslations[L"Entertain_Luxury_HUD_Name"] = L"Luxury HUD";

	m_valueTranslations[L"Entertain_Basic_Infotainment_Name"] = L"Basic Infotainment";
	m_valueTranslations[L"Entertain_Standard_Infotainment_Name"] = L"Standard Infotainment";
	m_valueTranslations[L"Entertain_Premium_Infotainment_Name"] = L"Premium Infotainment";
	m_valueTranslations[L"Entertain_Luxury_Infotainment_Name"] = L"Luxury Infotainment";

	m_valueTranslations[L"Entertain_Basic_SatNav_Name"] = L"Basic SatNav";
	m_valueTranslations[L"Entertain_Standard_SatNav_Name"] = L"Standard SatNav";
	m_valueTranslations[L"Entertain_Premium_SatNav_Name"] = L"Premium SatNav";
	m_valueTranslations[L"Entertain_Luxury_SatNav_Name"] = L"Luxury SatNav";

	m_valueTranslations[L"Entertain_Basic_CD_Name"] = L"Basic CD";
	m_valueTranslations[L"Entertain_Standard_CD_Name"] = L"Standard CD";
	m_valueTranslations[L"Entertain_Premium_CD_Name"] = L"Premium CD";
	m_valueTranslations[L"Entertain_Luxury_CD_Name"] = L"Luxury CD";

	m_valueTranslations[L"Entertain_Basic_Cassette_Name"] = L"Basic Cassette";
	m_valueTranslations[L"Entertain_Standard_Cassette_Name"] = L"Standard Cassette";
	m_valueTranslations[L"Entertain_Premium_Cassette_Name"] = L"Premium Cassette";
	m_valueTranslations[L"Entertain_Luxury_Cassette_Name"] = L"Luxury Cassette";

	m_valueTranslations[L"Entertain_Basic_8Track_Name"] = L"Basic 8-Track";
	m_valueTranslations[L"Entertain_Standard_8Track_Name"] = L"Standard 8-Track";
	m_valueTranslations[L"Entertain_Premium_8Track_Name"] = L"Premium 8-Track";
	m_valueTranslations[L"Entertain_Luxury_8Track_Name"] = L"Luxury 8-Track";

	m_valueTranslations[L"Entertain_Basic_AM_Name"] = L"Basic AM Radio";
	m_valueTranslations[L"Entertain_Standard_AM_Name"] = L"Standard AM Radio";
	m_valueTranslations[L"Entertain_Premium_AM_Name"] = L"Premium AM Radio";
	m_valueTranslations[L"Entertain_Luxury_AM_Name"] = L"Luxury AM Radio";

	m_valueTranslations[L"Entertain_None_Name"] = L"None";


	m_valueTranslations[L"DriveAssist_PowerSteer_None_Name"] = L"Recirculating Ball";
	m_valueTranslations[L"DriveAssist_PowerSteer_RackAndPinion_Name"] = L"Rack And Pinion";
	m_valueTranslations[L"DriveAssist_PowerSteer_HydraulicBall_Name"] = L"Hydraulic Ball";
	m_valueTranslations[L"DriveAssist_PowerSteer_Name"] = L"Hydraulic R&P";
	m_valueTranslations[L"DriveAssist_PowerSteer_Variable_Name"] = L"Variable Hydraulic";
	m_valueTranslations[L"DriveAssist_PowerSteer_Electric_Name"] = L"Electric";
	m_valueTranslations[L"DriveAssist_PowerSteer_Electric_Variable_Name"] = L"Variable Electric";

	m_valueTranslations[L"DriveAssist_None_Name"] = L"None";
	m_valueTranslations[L"DriveAssist_TractionPackage1_Name"] = L"ABS";
	m_valueTranslations[L"DriveAssist_TractionPackage2_Name"] = L"ABS TC";
	m_valueTranslations[L"DriveAssist_TractionPackage3_Name"] = L"ABS TC ESC";
	m_valueTranslations[L"DriveAssist_TractionPackage4_Name"] = L"ABS TC ESC LC";


	m_valueTranslations[L"Safety_None_Name"] = L"None";

	m_valueTranslations[L"Safety_Basic_20s_Name"] = L"Basic 20s";
	m_valueTranslations[L"Safety_Standard_20s_Name"] = L"Standard 20s";
	m_valueTranslations[L"Safety_Advanced_20s_Name"] = L"Advanced 20s";

	m_valueTranslations[L"Safety_Basic_10s_Name"] = L"Basic 10s";
	m_valueTranslations[L"Safety_Standard_10s_Name"] = L"Standard 10s";
	m_valueTranslations[L"Safety_Advanced_10s_Name"] = L"Advanced 10s";

	m_valueTranslations[L"Safety_Basic_00s_Name"] = L"Basic 00s";
	m_valueTranslations[L"Safety_Standard_00s_Name"] = L"Standard 00s";
	m_valueTranslations[L"Safety_Advanced_00s_Name"] = L"Advanced 00s";

	m_valueTranslations[L"Safety_Basic_90s_Name"] = L"Basic 90s";
	m_valueTranslations[L"Safety_Standard_90s_Name"] = L"Standard 90s";
	m_valueTranslations[L"Safety_Advanced_90s_Name"] = L"Advanced 90s";

	m_valueTranslations[L"Safety_Basic_80s_Name"] = L"Basic 80s";
	m_valueTranslations[L"Safety_Standard_80s_Name"] = L"Standard 80s";
	m_valueTranslations[L"Safety_Advanced_80s_Name"] = L"Advanced 80s";

	m_valueTranslations[L"Safety_Basic_70s_Name"] = L"Basic 70s";
	m_valueTranslations[L"Safety_Standard_70s_Name"] = L"Standard 70s";
	m_valueTranslations[L"Safety_Advanced_70s_Name"] = L"Advanced 70s";

	m_valueTranslations[L"Safety_Basic_60s_Name"] = L"Basic 60s";
	m_valueTranslations[L"Safety_Standard_60s_Name"] = L"Standard 60s";
	m_valueTranslations[L"Safety_Advanced_60s_Name"] = L"Advanced 60s";

	m_valueTranslations[L"Safety_Basic_50s_Name"] = L"Basic 50s";
	m_valueTranslations[L"Safety_Standard_50s_Name"] = L"Standard 50s";
	m_valueTranslations[L"Safety_Advanced_50s_Name"] = L"Advanced 50s";

	m_valueTranslations[L"Safety_Basic_40s_Name"] = L"Basic 40s";
	m_valueTranslations[L"Safety_Standard_40s_Name"] = L"Standard 40s";
	m_valueTranslations[L"Safety_Advanced_40s_Name"] = L"Advanced 40s";


	m_valueTranslations[L"Springs_Passive_Name"] = L"Passive";
	m_valueTranslations[L"Springs_Progressive_Name"] = L"Progressive";
	m_valueTranslations[L"Springs_Air_Name"] = L"Air";
	m_valueTranslations[L"Springs_Hydro_Name"] = L"Hydropneumatic";
	m_valueTranslations[L"Springs_ActiveSport_Name"] = L"Active Sport";
	m_valueTranslations[L"Springs_ActiveComfort_Name"] = L"Active Comfort";
	m_valueTranslations[L"Springs_Active_Name"] = L"Active Comfort";

	m_valueTranslations[L"Dampers_Passive_Name"] = L"Twin-Tube";
	m_valueTranslations[L"Dampers_Advanced_Name"] = L"Gas Mono-Tube";
	m_valueTranslations[L"Dampers_Adaptive_Name"] = L"Adaptive";
	m_valueTranslations[L"Dampers_SemiActive_Name"] = L"Semi-Active";

	m_valueTranslations[L"SwayBars_Passive_Name"] = L"Passive";
	m_valueTranslations[L"SwayBars_Offroad_Name"] = L"Offroad";
	m_valueTranslations[L"SwayBars_SemiActive_Name"] = L"Semi-Active";
	m_valueTranslations[L"SwayBars_Active_Name"] = L"Active";

	m_valueTranslations[L"RimMaterial_Steel_Name"] = L"Steel";
	m_valueTranslations[L"RimMaterial_Alloy_Name"] = L"Alloy";
	m_valueTranslations[L"RimMaterial_Mag_Name"] = L"Magnesium";
	m_valueTranslations[L"RimMaterial_CF_Name"] = L"Carbon Fiber";

	m_valueTranslations[L"TyreType_Radial_Name"] = L"Radial";
	m_valueTranslations[L"TyreType_CrossPly_Name"] = L"Cross-Ply";

	m_valueTranslations[L"TyreType_SemiOffRoad_Name"] = L"Offroad";
	m_valueTranslations[L"TyreType_AllTerrain_Name"] = L"All-Terrain";
	m_valueTranslations[L"TyreType_Utility_Name"] = L"Utility";
	m_valueTranslations[L"TyreType_LongHardRoad_Name"] = L"Hard";
	m_valueTranslations[L"TyreType_MedComp_Name"] = L"Medium";
	m_valueTranslations[L"TyreType_SportComp_Name"] = L"Sport";
	m_valueTranslations[L"TyreType_SemiSlick_Name"] = L"Semi-Slick";

	m_valueTranslations[L"Brake_Drum_Name"] = L"Drum";
	m_valueTranslations[L"Brake_TwinLeadingDrum_Name"] = L"Twin-Leading Drum";
	m_valueTranslations[L"BrakeDisc_Solid_Name"] = L"Solid Disc";
	m_valueTranslations[L"BrakeDisc_Vented_Name"] = L"Vented Disc";
	m_valueTranslations[L"BrakeDisc_CarCeramic_Name"] = L"Carbon-Ceramic";


	m_valueTranslations[L"EngBlock_Inl_Name"] = L"Inline";
	m_valueTranslations[L"EngBlock_V90_Name"] = L"V90";
	m_valueTranslations[L"EngBlock_V60_Name"] = L"V60";
	m_valueTranslations[L"EngBlock_Boxer_Name"] = L"Boxer";

	m_valueTranslations[L"EngBlockMat_Iron_Name"] = L"Iron";
	m_valueTranslations[L"EngBlockMat_Alu_Name"] = L"Aluminum";
	m_valueTranslations[L"EngBlockMat_AluSil_Name"] = L"AlSi";
	m_valueTranslations[L"EngBlockMat_Mg_Name"] = L"Magnesium";

	m_valueTranslations[L"HeadMat_Iron_Name"] = L"Iron";
	m_valueTranslations[L"HeadMat_Alu_Name"] = L"Aluminum";
	m_valueTranslations[L"EngHeadMat_AluSil_Name"] = L"AlSi";

	m_valueTranslations[L"Head_PushRod_Name"] = L"Pushrod OHV";
	m_valueTranslations[L"Head_DirectOHC_Name"] = L"DaSOHC";
	m_valueTranslations[L"Head_OHC_Name"] = L"SOHC";
	m_valueTranslations[L"Head_DuelOHC_Name"] = L"DOHC";

	m_valueTranslations[L"BalanceShaft_None_Name"] = L"None";
	m_valueTranslations[L"BalanceShaft_Harmonic_Name"] = L"Harmonic Damper";
	m_valueTranslations[L"BalanceShaft_BalanceShafts_Name"] = L"Balance Shaft";

	m_valueTranslations[L"CrankMat_Iron_Name"] = L"Cast";
	m_valueTranslations[L"CrankMat_Forged_Name"] = L"Forged";
	m_valueTranslations[L"CrankMat_Billet_Name"] = L"Billet";

	m_valueTranslations[L"RodMat_Cast_Name"] = L"Cast";
	m_valueTranslations[L"RodMat_HeavyCast_Name"] = L"Heavy Duty Cast";
	m_valueTranslations[L"RodMat_HBeam_Name"] = L"Heavy Duty Forged";
	m_valueTranslations[L"RodMat_IBSteel_Name"] = L"Lightweight Forged";
	m_valueTranslations[L"RodMat_IBTit_Name"] = L"Titanium";

	m_valueTranslations[L"Piston_Cast_Name"] = L"Cast";
	m_valueTranslations[L"Piston_HeavyCast_Name"] = L"Heavy Duty Cast";
	m_valueTranslations[L"Piston_HypCast_Name"] = L"Hypereutectic Cast";
	m_valueTranslations[L"Piston_LowFCast_Name"] = L"Low-Friction Cast";
	m_valueTranslations[L"Piston_Forged_Name"] = L"Forged";
	m_valueTranslations[L"Piston_LightForged_Name"] = L"Lightweight Forged";

	m_valueTranslations[L"VVT_None_Name"] = L"None";
	m_valueTranslations[L"VVT_VVTSOHC_Name"] = L"SOHC VVT";
	m_valueTranslations[L"VVT_VVTDOHC_Name"] = L"DOHC VVT Both";
	m_valueTranslations[L"VVT_VVTIntakeCam_Name"] = L"DOHC VVT Intake";
	m_valueTranslations[L"VVT_None_Name"] = L"None";
	m_valueTranslations[L"VVT_VVTSOHC_Name"] = L"SOHC VVT";
	m_valueTranslations[L"VVT_VVTDOHC_Name"] = L"DOHC VVT Both";
	m_valueTranslations[L"VVT_VVTIntakeCam_Name"] = L"DOHC VVT Intake";

	m_valueTranslations[L"VarValves_None_Name"] = L"None";
	m_valueTranslations[L"VarValves_VVTSOHC_Name"] = L"SOHC VVT";
	m_valueTranslations[L"VarValves_VVTDOHC_Name"] = L"DOHC VVT Both";
	m_valueTranslations[L"VarValves_VVTIntakeCam_Name"] = L"DOHC VVT Intake";
	m_valueTranslations[L"VarValves_None_Name"] = L"None";
	m_valueTranslations[L"VarValves_VVTSOHC_Name"] = L"SOHC VVT";
	m_valueTranslations[L"VarValves_VVTDOHC_Name"] = L"DOHC VVT Both";
	m_valueTranslations[L"VarValves_VVTIntakeCam_Name"] = L"DOHC VVT Intake";

	m_valueTranslations[L"VarValves_None_Name"] = L"None";
	m_valueTranslations[L"VarValves_VVL_Name"] = L"VVL";

	m_valueTranslations[L"Aspiration_Natural_Name"] = L"Naturally Aspirated";
	m_valueTranslations[L"Aspiration_Turbo_Name"] = L"Turbocharged";

	m_valueTranslations[L"Aspiration_Natural_Setup_Name"] = L"Naturally Aspirated";
	m_valueTranslations[L"Turbo_Single_Name"] = L"Single Turbo";
	m_valueTranslations[L"Turbo_Twin_Name"] = L"Twin Turbo";
	m_valueTranslations[L"Turbo_Quad_Name"] = L"Quad Turbo";

	m_valueTranslations[L"Aspiration_Natural_Boost_Control_Name"] = L"None";
	m_valueTranslations[L"TurboBoostControl_Wastegate_Name"] = L"Wastegate";
	m_valueTranslations[L"TurboBoostControl_BoostControl_Name"] = L"Boost Control";
	m_valueTranslations[L"TurboBoostControl_SmartBoost_Name"] = L"Smart Boost";

	m_valueTranslations[L"NoOption_Name"] = L"None";
	m_valueTranslations[L"Turbo_Standard_Geometry_Name"] = L"Standard Turbo";
	m_valueTranslations[L"Turbo_Twin_Scroll_Geometry_Name"] = L"Twin Scroll Turbo";
	m_valueTranslations[L"Turbo_Variable_Geometry_Name"] = L"Variable Geometry Turbo";

	m_valueTranslations[L"TurboBearing_Journal_Name"] = L"Journal Bearing";
	m_valueTranslations[L"TurboBearing_BallBearing_Name"] = L"Ball Bearing";

	m_valueTranslations[L"FuelSys_Carb_Name"] = L"Carburetor";
	m_valueTranslations[L"FuelSys_Inj_Name"] = L"Injection";

	m_valueTranslations[L"FuelSys_Carb_1Barrel_Name"] = L"1-Barrel Carburetor";
	m_valueTranslations[L"FuelSys_Carb_1BarrelEco_Name"] = L"1-Barrel Eco Carburetor";
	m_valueTranslations[L"FuelSys_Carb_2Barrel_Name"] = L"2-Barrel Carburetor";
	m_valueTranslations[L"FuelSys_Carb_4Barrel_Name"] = L"4-Barrel Carburetor";
	m_valueTranslations[L"FuelSys_Carb_DCOE_Name"] = L"DCOE";
	m_valueTranslations[L"FuelSys_Inj_Mech_Name"] = L"Mechanical Injection";
	m_valueTranslations[L"FuelSys_Inj_SingEFI_Name"] = L"Single-Point EFI";
	m_valueTranslations[L"FuelSys_Inj_MultiEFI_Name"] = L"Multi-Point EFI";
	m_valueTranslations[L"FuelSys_Inj_Direct_Name"] = L"Direct Injection EFI";

	m_valueTranslations[L"FuelSys_CarbIntake_Single_Name"] = L"Single Carburetor";
	m_valueTranslations[L"FuelSys_CarbIntake_Twin_Name"] = L"Twin Carburetor";
	m_valueTranslations[L"FuelSys_CarbIntake_Quad_Name"] = L"Quad Carburetor";
	m_valueTranslations[L"FuelSys_InjIntake_Single_Name"] = L"Single Throttle";
	m_valueTranslations[L"FuelSys_InjIntake_Twin_Name"] = L"Twin Throttle";
	m_valueTranslations[L"FuelSys_InjIntake_PerCyl_Name"] = L"Individual Throttle Bodies";

	m_valueTranslations[L"FuelSys_CarbFilter_Compact_Name"] = L"Compact";
	m_valueTranslations[L"FuelSys_InjFilterr_Compact_Name"] = L"Compact";
	m_valueTranslations[L"FuelSys_CarbFilter_StandardLow_Name"] = L"Standard Low";
	m_valueTranslations[L"FuelSys_InjFilter_StandardLow_Name"] = L"Standard Low";
	m_valueTranslations[L"FuelSys_CarbFilter_StandardMid_Name"] = L"Standard Mid";
	m_valueTranslations[L"FuelSys_InjFilter_StandardMid_Name"] = L"Standard Mid";
	m_valueTranslations[L"FuelSys_CarbFilter_PerfMidRange_Name"] = L"Performance Mid";
	m_valueTranslations[L"FuelSys_InjFilter_PerfMidRange_Name"] = L"Performance Mid";
	m_valueTranslations[L"FuelSys_CarbFilter_PerfHighRange_Name"] = L"Performance High";
	m_valueTranslations[L"FuelSys_InjFilter_PerfHighRange_Name"] = L"Performance High";
	m_valueTranslations[L"FuelSys_InjFilter_Variable_Name"] = L"Variable";
	m_valueTranslations[L"FuelSys_CarbFilter_Race_Name"] = L"Race";
	m_valueTranslations[L"FuelSys_InjFilter_Race_Name"] = L"Race";

	m_valueTranslations[L"Header_CastLog_Name"] = L"Compact Cast";
	//m_Translations[L"Header_ShortCast_Name"] = L"Short Cast";
	m_valueTranslations[L"Header_CastLow_Name"] = L"Cast Low";
	m_valueTranslations[L"Header_CastMid_Name"] = L"Cast Mid";
	m_valueTranslations[L"Header_Tube_Name"] = L"Tubular Mid";
	m_valueTranslations[L"Header_LongTube_Name"] = L"Tubular Long";
	m_valueTranslations[L"Header_RaceTube_Name"] = L"Tubular Race";
	m_valueTranslations[L"Header_TurboShortLog_Name"] = L"Turbo Cast Log";
	m_valueTranslations[L"Header_TurboShortCast_Name"] = L"Turbo Short Cast";
	m_valueTranslations[L"Header_TurboRaceTubular_Name"] = L"Turbo Race Tubular";

	m_valueTranslations[L"Exhausts_1_Name"] = L"Single";
	m_valueTranslations[L"Exhausts_2_Name"] = L"Dual";

	m_valueTranslations[L"ExhaustBypassValvesNone_Name"] = L"None";
	m_valueTranslations[L"ExhaustBypassValves_Name"] = L"Bypass Valves";

	m_valueTranslations[L"CatConvert_None_Name"] = L"None";
	m_valueTranslations[L"CatConvert_2Way_Name"] = L"2-Way";
	m_valueTranslations[L"CatConvert_3Way_Name"] = L"3-Way";
	m_valueTranslations[L"CatConvert_High3Way_Name"] = L"High Flow 3-Way";

	m_valueTranslations[L"Muffler_None_Name"] = L"None";
	m_valueTranslations[L"Muffler_Confused_Name"] = L"Baffled";
	m_valueTranslations[L"Muffler_Reverse_Name"] = L"Reverse Flow";
	m_valueTranslations[L"Muffler_Straight_Name"] = L"Straight Through";

	m_valueTranslations[L"CompressedGas"] = L"Compressed Gas";
	m_valueTranslations[L"Nitro"] = L"Nitromethane";
}

const std::wstring& JSONDocumentBuilder::getValueTranslation(const std::wstring& value)
{
    if (m_valueTranslations.find(value) == m_valueTranslations.end())
        return value;

    return m_valueTranslations[value];
}
