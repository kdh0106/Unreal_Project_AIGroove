// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGStageDataManager.h"

const TArray<FStageData>& AAIGStageDataManager::GetAllStages() const
{
	return StageList;
}
