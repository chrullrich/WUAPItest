// WUAPItest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main()
{
    std::wcout << L"Hello World!" << std::endl;

	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	HRESULT hr;
	
	IUpdateSessionPtr pSession;
	if (FAILED(hr = pSession.CreateInstance(CLSID_UpdateSession, nullptr, CLSCTX_ALL))) {
		std::wcerr << L"Could not create UpdateSession: " << std::hex << hr << std::endl;
		return 1;
	}

	IUpdateSearcherPtr pSearcher;
	if (FAILED(hr = pSession->CreateUpdateSearcher(&pSearcher))) {
		std::wcerr << L"Could not create UpdateSearcher: " << std::hex << hr << std::endl;
		return 1;
	}

	ISearchResultPtr pSearchResult;
	if (FAILED(hr = pSearcher->Search(_bstr_t(L"IsInstalled=1"), &pSearchResult))) {
		std::wcerr << L"Could not search: " << std::hex << hr << std::endl;
		return 1;
	}

	IUpdateCollectionPtr pUpdates;
	if (FAILED(hr = pSearchResult->get_Updates(&pUpdates))) {
		std::wcerr << L"Could not get updates: " << std::hex << hr << std::endl;
		return 1;
	}

	LONG lCount = 0;
	if (FAILED(hr = pUpdates->get_Count(&lCount))) {
		std::wcerr << L"Could not get update count: " << std::hex << hr << std::endl;
		return 1;
	}

	for (int i = 0; i < lCount; ++i) {
		IUpdatePtr pUpd;
		if (FAILED(hr = pUpdates->get_Item(i, &pUpd))) {
			std::wcerr << L"Could not get update at index " << i << L": " << std::hex << hr << std::endl;
			return 1;
		}

		_bstr_t bsTitle;
		if (FAILED(hr = pUpd->get_Title(bsTitle.GetAddress()))) {
			std::wcerr << L"Could not get title for update at index " << i << L": " << std::hex << hr << std::endl;
			return 1;
		}
		
		std::wcout << L"Update: " << (LPCWSTR)bsTitle << std::endl;

		IInstallationBehaviorPtr pBehavior;
		if (FAILED(hr = pUpd->get_InstallationBehavior(&pBehavior))) {
			std::wcerr << L"Could not get InstallationBehavior for update at index " << i << L": " << std::hex << hr << std::endl;
			return 1;
		}

		VARIANT_BOOL bInput = VARIANT_FALSE;
		if (FAILED(hr = pBehavior->get_CanRequestUserInput(&bInput))) {
			std::wcerr << L"Could not get input behavior for update at index " << i << L": " << std::hex << hr << std::endl;
			return 1;
		}

		if (bInput) {
			std::wcout << L"  Update can request user input." << std::endl;
		} else {
			std::wcout << L"  Update does not request user input." << std::endl;
		}

		InstallationImpact nImpact;
		if (FAILED(hr = pBehavior->get_Impact(&nImpact))) {
			std::wcerr << L"Could not get impact for update at index " << i << L": " << std::hex << hr << std::endl;
			return 1;
		}

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

		InstallationRebootBehavior nReboot;
		if (FAILED(hr = pBehavior->get_RebootBehavior(&nReboot))) {
			std::wcerr << L"Could not get reboot behavior for update at index " << i << L": " << std::hex << hr << std::endl;
			return 1;
		}

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

		VARIANT_BOOL bNetwork = VARIANT_FALSE;
		if (FAILED(hr = pBehavior->get_RequiresNetworkConnectivity(&bNetwork))) {
			std::wcerr << L"Could not get network requirement for update at index " << i << L": " << std::hex << hr << std::endl;
			return 1;
		}

		if (bNetwork) {
			std::wcout << L"  Update requires network connectivity." << std::endl;
		} else {
			std::wcout << L"  Update does not require network connectivity." << std::endl;
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
