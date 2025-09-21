//
// Created by Caden Vize on 9/21/2025.
//

#ifndef BENEATHTHESURFACE_GAMESTARTUPUPDATECHECK_H
#define BENEATHTHESURFACE_GAMESTARTUPUPDATECHECK_H

#pragma once
#include <string>

bool SoftUpdateCheckAndMaybeRun(const std::wstring& installDir = L"");

#endif //BENEATHTHESURFACE_GAMESTARTUPUPDATECHECK_H