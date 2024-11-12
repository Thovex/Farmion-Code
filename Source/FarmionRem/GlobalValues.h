#pragma once

constexpr int32 GSaveGame_Version = 1;

constexpr float GCarry_Grab_Interpolation_Duration = 0.5f;
constexpr float GCarry_Drop_Interpolation_Duration = 3.0f;

constexpr float GCarry_Snap_Threshold = 5.f;

constexpr float GDefault_Lifespan_Check_Value = 1.f;

constexpr float GCandle_Default_Cooldown = 1.25f;
constexpr float GBucket_Default_Cooldown = 1.25f;

constexpr float GHoney_Skip_Progress_Rate = 33.33f; // Change on Blossoming Stage days, whilst constantly having a Beehive every cycle (with a deviation of 1)
constexpr float GHoney_Regeneration_Time = 60.f;

constexpr int32 GLow_Health_Crop = 3;

const FName GProtected_Tag_Name = FName(TEXT("Protected"));
const FName GCarryable_Mesh_Ignore_Tag_Name = FName(TEXT("CarryableIgnore"));
const FName GPest_Socket_Tag_Name = FName(TEXT("PestSocket"));
const FName GClip_Through_Object_Tag_Name = FName(TEXT("ClipThroughObject"));

const FName GAnimal_Idle_Anim = FName(TEXT("Idle"));
const FName GAnimal_Walk_Anim = FName(TEXT("Walk"));

constexpr float GDefault_Adept_Mod = 1.0f;
constexpr float GDefault_Arcane_Mod = 1.25f;
constexpr float GDefault_Eldar_Mod = 1.5f;

constexpr float GPing_Lifetime = 10.f;

constexpr float GChance_For_WinterStorm = 5.f;
constexpr float GChance_For_Rain = 13.f;

constexpr FLinearColor GFarmion_Button_Text_Color {1.0f, 1.0f, 1.0f, 1.0f};
constexpr FLinearColor GFarmion_Button_Text_Color_Hover {0.3f, 0.73f, 1.0f, 1.0f};
