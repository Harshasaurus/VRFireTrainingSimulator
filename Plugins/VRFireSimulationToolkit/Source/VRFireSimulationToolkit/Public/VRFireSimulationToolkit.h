#pragma once

#include "Modules/ModuleManager.h"

class FVRFireSimulationToolkitModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
