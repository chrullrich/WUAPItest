// WUAPItestTLB.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main()
{
    std::wcout << L"Hello World!" << std::endl;

	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	WU::IUpdateSessionPtr pSession;
	try {
		pSession.CreateInstance(__uuidof(WU::UpdateSession));
	} catch (const _com_error& e) {
		std::wcerr << L"Could not create update session: " << std::hex << e.Error() << std::endl;
		return 1;
	}

	WU::IUpdateSearcherPtr pSearcher;
	try {
		pSearcher = pSession->CreateUpdateSearcher();
	} catch (const _com_error& e) {
		std::wcerr << L"Could not create update searcher: " << std::hex << e.Error() << std::endl;
		return 1;
	}

	WU::ISearchResultPtr pResult;
	try {
		pResult = pSearcher->Search(L"IsInstalled=1");
	} catch (const _com_error& e) {
		std::wcerr << L"Could not search: " << std::hex << e.Error() << std::endl;
		return 1;
	}

	WU::IUpdateCollectionPtr pUpdates;
	try {
		pUpdates = pResult->Updates;
	} catch (const _com_error& e) {
		std::wcerr << L"Could not get updates: " << std::hex << e.Error() << std::endl;
		return 1;
	}

	LONG lCount = 0;
	try {
		lCount = pUpdates->Count;
	} catch (const _com_error& e) {
		std::wcerr << L"Could not get update count: " << std::hex << e.Error() << std::endl;
		return 1;
	}

	for (int i = 0; i < lCount; ++i) {
		WU::IUpdatePtr pUpd;
		try {
			pUpd = pUpdates->Item[(LONG)i];
		} catch (const _com_error& e) {
			std::wcerr << L"Could not get update at index " << i << L": " << std::hex << e.Error() << std::endl;
			return 1;
		}

		try {
			std::wcout << L"Update: " << (LPCWSTR)(pUpd->Title) << std::endl;
		} catch (const _com_error& e) {
			std::wcerr << L"Could not get title for update at index " << i << L": " << std::hex << e.Error() << std::endl;
			return 1;
		}

		WU::IInstallationBehaviorPtr pBehavior;
		try {
			pBehavior = pUpd->InstallationBehavior;
		} catch (const _com_error& e) {
			std::wcerr << L"Could not get InstallationBehavior for update at index " << i << L": " << std::hex << e.Error() << std::endl;
			return 1;
		}

		try {
			if (pBehavior->CanRequestUserInput) {
				std::wcout << L"  Update can request user input." << std::endl;
			} else {
				std::wcout << L"  Update does not request user input." << std::endl;
			}
		} catch (const _com_error& e) {
			std::wcerr << L"Could not get input behavior for update at index " << i << L": " << std::hex << e.Error() << std::endl;
			return 1;
		}

		try {
			WU::InstallationImpact nImpact = pBehavior->Impact;
			switch (nImpact) {
			case iiNormal:
				std::wcout << L"  Update has normal impact." << std::endl;
				break;
			case iiMinor:
				std::wcout << L"  Update has minor impact." << std::endl;
				break;
			case iiRequiresExclusiveHandling:
				std::wcout << L"  Update requires exclusive handling." << std::endl;
				break;
			default:
				std::wcout << L"  Update has unknown impact type " << nImpact << L"." << std::endl;
				break;
			}
		} catch (const _com_error& e) {
			std::wcerr << L"Could not get impact for update at index " << i << L": " << std::hex << e.Error() << std::endl;
			return 1;
		}

		try {
			WU::InstallationRebootBehavior nReboot = pBehavior->RebootBehavior;
			switch (nReboot) {
			case irbNeverReboots:
				std::wcout << L"  Update does not reboot." << std::endl;
				break;
			case irbAlwaysRequiresReboot:
				std::wcout << L"  Update requires reboot." << std::endl;
				break;
			case irbCanRequestReboot:
				std::wcout << L"  Update can request reboot." << std::endl;
				break;
			default:
				std::wcout << L"  Update has unknown reboot type " << nReboot << L"." << std::endl;
				break;
			}
		} catch (const _com_error& e) {
			std::wcerr << L"Could not get reboot behavior for update at index " << i << L": " << std::hex << e.Error() << std::endl;
			return 1;
		}

		try {
			if (pBehavior->RequiresNetworkConnectivity) {
				std::wcout << L"  Update requires network connectivity." << std::endl;
			} else {
				std::wcout << L"  Update does not require network connectivity." << std::endl;
			}
		} catch (const _com_error& e) {
			std::wcerr << L"Could not get reboot behavior for update at index " << i << L": " << std::hex << e.Error() << std::endl;
			return 1;
		}
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
