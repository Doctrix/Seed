#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(GJAPI, Log, All);

class GAMEJOLTPLUGIN_API GameJoltPlugin : public IModuleInterface
{
private:

public:
	GameJoltPlugin();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};