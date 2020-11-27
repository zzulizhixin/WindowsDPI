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
	* @brief:��ȡ���򴰿�dpi����
	* @author:������ 2020/04/17
	* @lParam[in]:���ھ��
	* @return 0:��ȡʧ�ܣ���������Ҫ�Է���ֵ�����ж�
	* @return 1.25/1.5/1/75:���ڱ���
	*/
	double GetProcessWindowDpi(HWND hWnd);

	/**
	* @brief:ͨ��windows�ṩ��API��ȡ����dpi����
	* @return 0:��ȡʧ�ܣ���������Ҫ�Է���ֵ�����ж�
	* @return 1.25/1.5/1/75:���ڱ���
	* @other :�ӿ��аѵ�ǰ�����DPI��֪����Ϊϵͳ���������ȷ��ȡ����DPI
	*/
	double GetDesktopDpiByApi();

	/**
	* @brief:ͨ����ȡע����ȡ����dpi����
	* @return 0:��ȡʧ�ܣ���������Ҫ�Է���ֵ�����ж�
	* @return 1.25/1.5/1/75:���ڱ���
	* @other :��ȡע���ľ������Ǹı�����DPI�����������Ļ���ע������ֵҲ����ı�
	*/
	double GetDesktopDpiByReg();


	/**
	* @brief:ͨ��ͨ���߼�����������������ȡ����dpi����
	* @return 0:��ȡʧ�ܣ���������Ҫ�Է���ֵ�����ж�
	* @return 1.25/1.5/1/75:���ڱ���
	* @other :��Ҫ���Լ�����dpi����֪�������ȥ��ȡ�������ȡΪ1
	*/
	double GetDesktopDpiByCalcu();

	/**
	* @brief:ʹ�����������������������ƶ�
	*/
	void MouseMoveNew(double x, double y, int nFlag);

private:
	void GetSystemType();
	void GetWindowDpiByApiWin7();//������win10��Ҳ����
	void GetWindowDpiByApiWin10();//ֱ��ͨ��windows��Win10���ṩ��API���л�ȡ����DPI
	double ChangeDpiToProportion(UINT uiDpi);

private:
	typedef enum
	{
		WIN_UNKNOWN = 0,
		WIN_7,
		WIN_10
	} OSVersionEnum;

	OSVersionEnum osversion_;				// ����ϵͳ
	double process_dpi_proportion_;		// Ŀ�괰��dpi
	HWND hpro_wnd_;							// Ŀ�괰�ھ��
};