#include <BWAPI.h>
#include <BWAPI/Client.h>
#include "UAlbertaBotModule.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "Global.h"
#include "../../SparCraft/source/SparCraft.h"
#include "../../BOSS/source/BOSS.h"

using namespace BWAPI;
using namespace UAlbertaBot;

void UAlbertaBot_BWAPIReconnect()
{
	while (!BWAPIClient.connect())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
	}
}

void UAlbertaBot_PlayGame()
{
    PROFILE_FUNCTION();

	UAlbertaBotModule bot;

	// ���Ⱑ ���� ����, ������ ����ƴ��� �ݺ� üũ
	while (BWAPI::BWAPIClient.isConnected() && BWAPI::Broodwar->isInGame())
	{
		// ���ӿ� ������ ������ �Ͼ�� �̺�Ʈ�� ���� �ڵ鸵 ��
		for (const BWAPI::Event & e : BWAPI::Broodwar->getEvents())
		{
			switch (e.getType())
			{
				case BWAPI::EventType::MatchStart:      { bot.onStart();					  break; }
				case BWAPI::EventType::MatchFrame:      { bot.onFrame();                      break; }
				case BWAPI::EventType::MatchEnd:        { bot.onEnd(e.isWinner());            break; }
				case BWAPI::EventType::UnitShow:        { bot.onUnitShow(e.getUnit());        break; }
				case BWAPI::EventType::UnitHide:        { bot.onUnitHide(e.getUnit());        break; }
				case BWAPI::EventType::UnitCreate:      { bot.onUnitCreate(e.getUnit());      break; }
				case BWAPI::EventType::UnitMorph:       { bot.onUnitMorph(e.getUnit());       break; }
				case BWAPI::EventType::UnitDestroy:     { bot.onUnitDestroy(e.getUnit());     break; }
				case BWAPI::EventType::UnitRenegade:    { bot.onUnitRenegade(e.getUnit());    break; }
				case BWAPI::EventType::UnitComplete:    { bot.onUnitComplete(e.getUnit());    break; }
				case BWAPI::EventType::SendText:        { bot.onSendText(e.getText());        break; }
			}
		}
        
		BWAPI::BWAPIClient.update();
		if (!BWAPI::BWAPIClient.isConnected())
		{
			std::cout << "Disconnected\n";
			break;
		}
	}

	std::cout << "Game Over\n";
}


int main(int argc, char * argv[])
{
    PROFILE_FUNCTION();

	bool exitIfStarcraftShutdown = true;
    
    // Initialize SparCraft, the combat simulation package
    SparCraft::init();

    // Initialize BOSS, the Build Order Search System
    BOSS::init();

	size_t gameCount = 0;
	while (true)
	{
		// ������ ����� ������ �õ���
		if (!BWAPI::BWAPIClient.isConnected())
		{
			UAlbertaBot_BWAPIReconnect();
		}

		// BWAPI ���� �б�
		while (BWAPI::BWAPIClient.isConnected())
		{
			// ������ ������ ������ ��ٸ�
			std::cout << "Waiting for game start\n";
			while (BWAPI::BWAPIClient.isConnected() && !BWAPI::Broodwar->isInGame())
			{
				BWAPI::BWAPIClient.update();
			}

			// Check to see if Starcraft shut down somehow
			if (BWAPI::BroodwarPtr == nullptr)
			{
				break;
			}

			// ���ӿ� ���� �Ʒ� ������ �����
			if (BWAPI::Broodwar->isInGame())
			{
				std::cout << "Playing game " << gameCount++ << " on map " << BWAPI::Broodwar->mapFileName() << "\n";

				UAlbertaBot_PlayGame();
			}
		}

		if (exitIfStarcraftShutdown && !BWAPI::BWAPIClient.isConnected())
		{
			return 0;
		}
	}

	return 0;
}