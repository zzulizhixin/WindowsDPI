#pragma once
#include<Windows.h>

class DpiAssistant
{
public:
	static DpiAssistant& Getinstance()
	{
		static DpiAssistant siglton;
		return siglton;
	}

	DpiAssistant();
	~DpiAssistant();
	/**
	* @brief:获取程序窗口dpi比例
	* @author:李芷欣 2020/04/17
	* @lParam[in]:窗口句柄
	* @return 0:获取失败，调用者需要对返回值进行判断
	* @return 1.25/1.5/1/75:窗口比例
	*/
	double GetProcessWindowDpi(HWND hWnd);

	/**
	* @brief:通过windows提供的API获取桌面dpi比例
	* @return 0:获取失败，调用者需要对返回值进行判断
	* @return 1.25/1.5/1/75:窗口比例
	* @other :接口中把当前程序的DPI感知设置为系统级别才能正确获取桌面DPI
	*/
	double GetDesktopDpiByApi();

	/**
	* @brief:通过读取注册表获取桌面dpi比例
	* @return 0:获取失败，调用者需要对返回值进行判断
	* @return 1.25/1.5/1/75:窗口比例
	* @other :读取注册表的局限性是改变桌面DPI不进行重启的话，注册表的数值也不会改变
	*/
	double GetDesktopDpiByReg();


	/**
	* @brief:通过通过逻辑坐标除以物理坐标获取桌面dpi比例
	* @return 0:获取失败，调用者需要对返回值进行判断
	* @return 1.25/1.5/1/75:窗口比例
	* @other :需要在自己程序dpi不感知的情况下去获取，否则获取为1
	*/
	double GetDesktopDpiByCalcu();

	/**
	* @brief:使用桌面的物理坐标进行坐标移动
	*/
	void MouseMoveNew(double x, double y, int nFlag);

private:
	void GetSystemType();
	void GetWindowDpiByApiWin7();//测试在win10上也适用
	void GetWindowDpiByApiWin10();//直接通过windows在Win10上提供的API进行获取窗口DPI
	double ChangeDpiToProportion(UINT uiDpi);

private:
	typedef enum
	{
		WIN_UNKNOWN = 0,
		WIN_7,
		WIN_10
	} OSVersionEnum;

	OSVersionEnum osversion_;				// 操作系统
	double process_dpi_proportion_;		// 目标窗口dpi
	HWND hpro_wnd_;							// 目标窗口句柄
};