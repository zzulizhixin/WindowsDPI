#include <shellscalingapi.h>
#include "ProDpiAssistant.h"
#include "dwmapi.h"
#include <iostream>
#pragma comment(lib, "dwmapi.lib")


DpiAssistant::DpiAssistant()
	: osversion_(WIN_UNKNOWN)
	, process_dpi_proportion_(0.0)
	, hpro_wnd_(NULL)
{
	GetSystemType();
}

DpiAssistant::~DpiAssistant()
{
}

double DpiAssistant::GetProcessWindowDpi(HWND hWnd)
{
	hpro_wnd_ = hWnd;
	if (NULL == hpro_wnd_)
		return 0;

	switch (osversion_)
	{
	case WIN_UNKNOWN | WIN_7:
		GetWindowDpiByApiWin7();
		break;
	case WIN_10:
		GetWindowDpiByApiWin10();
		break;
	default:
		break;
	}
	return process_dpi_proportion_;
}

double DpiAssistant::GetDesktopDpiByCalcu()
{
	double dDpi = 0.0;
	// 获取窗口当前显示的监视器
	// 使用桌面的句柄.
	HWND hWnd = GetDesktopWindow();
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	// 获取监视器逻辑宽度与高度
	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
	int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

	// 获取监视器物理宽度与高度
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	int cyPhysical = dm.dmPelsHeight;

	// 缩放比例计算  实际上使用任何一个即可
	double horzScale = ((double)cxPhysical / (double)cxLogical);
	double vertScale = ((double)cyPhysical / (double)cyLogical);
	double x = floor(horzScale * 100.00f + 0.5) / 100.00f;
	double y = floor(vertScale * 100.00f + 0.5) / 100.00f;
	if (x == y)//计算有偏差保留小数点后3位进行比较
	{
		dDpi = x;
	}

	return dDpi;
}

double DpiAssistant::GetDesktopDpiByApi()
{
	double dwDpi = 0.0;
	do
	{
		if (FALSE == SetProcessDPIAware())
			break;

		int iDpi;
		HDC hdc = GetDC(0);
		iDpi = GetDeviceCaps(hdc, LOGPIXELSY);
		DeleteObject(hdc);
		dwDpi = ChangeDpiToProportion(iDpi);
	} while (false);
	return dwDpi;
}

void DpiAssistant::GetWindowDpiByApiWin10()
{
	if (osversion_ != WIN_10)
		return;

	UINT uDpi = 96;
	HINSTANCE hinst = LoadLibraryA("User32.dll");
	typedef UINT(__stdcall * GETDPI)(HWND);
	if (NULL == hinst)
		return;

	GETDPI GetDpiForWindow = (GETDPI)GetProcAddress(hinst, "GetDpiForWindow");
	if (NULL == hpro_wnd_)
		return;

	uDpi = GetDpiForWindow(hpro_wnd_);
	process_dpi_proportion_ = ChangeDpiToProportion(uDpi);
}

void DpiAssistant::GetWindowDpiByApiWin7()
{
	if (NULL == hpro_wnd_)
		return;
	HRESULT hResult = S_FALSE;
	BOOL enabled = FALSE;
	UINT iDpi = 96;
	RECT rcReal;			//窗口在100%下真实的窗口Rect，无论桌面DPI多大
	hResult = DwmGetWindowAttribute(hpro_wnd_, DWMWA_EXTENDED_FRAME_BOUNDS, &rcReal, sizeof(rcReal));

	if (S_OK == hResult)//函数执行成功
	{
		RECT rcLogic;//窗口逻辑现在的Rect
		if (GetWindowRect(hpro_wnd_, &rcLogic))
		{
			double xProportion = (double)(rcReal.right - rcReal.left) / (double)(rcLogic.right - rcLogic.left);
			double yProportion = (double)(rcReal.bottom - rcReal.top) / (double)(rcLogic.bottom - rcLogic.top);
			double x = floor(xProportion * 100.00f + 0.5) / 100.00f;
			double y = floor(yProportion * 100.00f + 0.5) / 100.00f;
			if (x == y)//计算有偏差保留小数点后3位进行比较
				process_dpi_proportion_ = y;
		}
	}
	return;
}

void DpiAssistant::GetSystemType()
{
	typedef LONG(__stdcall * fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
	fnRtlGetVersion pRtlGetVersion;
	HMODULE hNtdll;
	LONG ntStatus;
	ULONG    dwMajorVersion = 0;
	ULONG    dwMinorVersion = 0;
	ULONG    dwBuildNumber = 0;
	RTL_OSVERSIONINFOW VersionInformation = { 0 };
	hNtdll = GetModuleHandle(TEXT("ntdll.dll"));
	if (hNtdll == NULL)
		return;

	pRtlGetVersion = (fnRtlGetVersion)GetProcAddress(hNtdll, "RtlGetVersion");
	if (pRtlGetVersion == NULL)
		return;

	VersionInformation.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	ntStatus = pRtlGetVersion(&VersionInformation);

	if (ntStatus != 0)
		return;

	dwMajorVersion = VersionInformation.dwMajorVersion;
	dwMinorVersion = VersionInformation.dwMinorVersion;
	dwBuildNumber = VersionInformation.dwBuildNumber;

	if (dwMajorVersion == 6 && dwMinorVersion == 1)
		osversion_ = WIN_7;
	else if (dwMajorVersion == 10 && dwMinorVersion == 0)
		osversion_ = WIN_10;

	return;
}

double DpiAssistant::ChangeDpiToProportion(UINT uiDpi)
{
	double dwProportion = 1.0;
	do {
		switch (uiDpi)
		{
		case 96:
			dwProportion = 1.0;
			break;
		case 120:
			dwProportion = 1.25;
			break;
		case 144:
			dwProportion = 1.5;
			break;
		case 168:
			dwProportion = 1.75;
			break;
		case 192:
			dwProportion = 2.00;
			break;
		default:
			dwProportion = 0.00;
			break;
		}
	} while (0);
	//以上代码也可以直接使用uiDpi / 96获取dwProportion
	return dwProportion;
}

double DpiAssistant::GetDesktopDpiByReg()
{
	CRegKey reg;
	DWORD dwValue = 0;
	double dProportion = 0;
	do
	{
		if (reg.Open(HKEY_CURRENT_USER, _T("Control Panel\\Desktop\\WindowMetrics"), KEY_READ) == ERROR_SUCCESS)
		{
			if (ERROR_SUCCESS == reg.QueryDWORDValue(_T("AppliedDPI"), dwValue))//读取注册表的dpi数值
			{
				dProportion = ChangeDpiToProportion(dwValue);
				break;
			}
		}
	} while (0);

	if (reg)
		reg.Close();

	return dProportion;
}

void DpiAssistant::MouseMoveNew(double x, double y, int nFlag)
{
	HWND hDeskWnd = GetDesktopWindow();
	HMONITOR hMonitor = MonitorFromWindow(hDeskWnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;
	int cyPhysical = dm.dmPelsHeight;

	double fx = x * (65535.0f / cxPhysical);
	double fy = y * (65535.0f / cyPhysical);
	INPUT  Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	Input.mi.dwExtraInfo = nFlag;

	double dDeskDpi = GetDesktopDpiByCalcu();
	Input.mi.dx = (int)fx * dDeskDpi;
	Input.mi.dy = (int)fy * dDeskDpi;

	SendInput(1, &Input, sizeof(INPUT));
}
