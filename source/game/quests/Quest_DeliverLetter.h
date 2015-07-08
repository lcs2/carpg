#pragma once

//-----------------------------------------------------------------------------
#include "Quest.h"

//-----------------------------------------------------------------------------
class Quest_DeliverLetter : public Quest
{
public:
	enum Progress
	{
		None,
		Started,
		Timeout,
		GotResponse,
		Finished
	};

	void Start();
	DialogEntry* GetDialog(int type2);
	void SetProgress(int prog2);
	cstring FormatString(const string& str);
	bool IsTimedout();
	bool IfHaveQuestItem();
	const Item* GetQuestItem();
	void Save(HANDLE file);
	void Load(HANDLE file);

private:
	int end_loc;
	OtherItem letter;
};