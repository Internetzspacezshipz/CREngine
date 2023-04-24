#pragma once
#include "UserInterface/CrUI_Base.h"

enum class CrAssetListFunctionality : uint8_t
{
	None,
	OpenEditor,
};

// Directory tree structure display
class CrUI_AssetList : public CrUI_Base
{
	DEF_CLASS(CrUI_AssetList, CrUI_Base);

private:
	Func<void(const Path&)> OpenAction;

public:
	Path CurrentDirectory;

	void SetOnOpenAction(CrAssetListFunctionality Chosen);
	void SetOnOpenAction(Func<void(const Path&)> NewOpenAction);

	CrUI_AssetList();
	virtual void DrawUI() override;
};

