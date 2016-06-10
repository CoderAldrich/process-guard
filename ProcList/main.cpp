#include <Windows.h>
#include <iostream>
#include <signal.h>

#include <string>

#include "KernelModeQuery.h"
#include "UserModeQuery.h"
#include "ProcessInfo.h"
#include "ProcGuard.h"

#include "DriverInstaller.h"

static const LPCWSTR DRIVER_NAME_SCANNER = L"ProcScanner";
static const LPCWSTR DRIVER_NAME_GUARD = L"ProcGuard";

static const LPCWSTR DRIVER_PATH_SCANNER = L"ProcListDriver.sys";
static const LPCWSTR DRIVER_PATH_GUARD = L"ProcGuardDriver.sys";

static const LPCWSTR DEVICE_LINK_SCANNER = L"\\\\.\\ProcScannerCDOSL";
static const LPCWSTR DEVICE_LINK_GUARD = L"\\\\.\\ProcGuardCDOSL";

static BOOL Guard = FALSE;

ProcGuard procGuard = ProcGuard::getInstance();

VOID list(BOOL hidden = FALSE);

BOOL hide(const std::string &name);

VOID showHelp();

BOOL loadScannerDriver();

BOOL loadGuardDriver();

BOOL unloadScannerDriver();

BOOL unloadGuardDriver();

BOOL testDriver(LPCWSTR);

VOID cls();

VOID guard(const std::string&);

VOID sig(int s);

int main(void)
{
	signal(SIGINT, sig);

	if (!testDriver(DEVICE_LINK_SCANNER)) {
		loadScannerDriver();
	}
	if (!testDriver(DEVICE_LINK_GUARD)) {
		loadGuardDriver();
	}

	if (testDriver(DEVICE_LINK_SCANNER) &&
		testDriver(DEVICE_LINK_GUARD)) {
		cls();
	}

	showHelp();

	std::string cmd, para;
	std::cout << "# ";
	while (std::cin >> cmd) {
		if (cmd == "list" || cmd == "ls") {
			list();
		}
		else if (cmd == "hide") {
			std::cin >> para;

			if (hide(para)) {
				std::cout << ">> SUCCEEDED"
					<< std::endl;
			}
			else {
				std::cout << ">> FAILED"
					<< std::endl;
			}
		}
		else if (cmd == "load") {
			loadScannerDriver();
			loadGuardDriver();
		}
		else if (cmd == "load-S") {
			loadScannerDriver();
		}
		else if (cmd == "load-G") {
			loadGuardDriver();
		}
		else if (cmd == "unload") {
			unloadGuardDriver();
			unloadScannerDriver();
		}
		else if (cmd == "unload-S") {
			unloadScannerDriver();
		}
		else if (cmd == "unload-G") {
			unloadGuardDriver();
		}
		else if (cmd == "guard") {
			std::cin >> para;

			guard(para);
		}
		else if (cmd == "cls" || cmd == "clear") {
			cls();
		}
		else if (cmd == "exit") {
			break;
		}
		else {
			showHelp();
		}

		std::cout << "# ";
	}

	unloadGuardDriver();
	unloadScannerDriver();

	return 0;
}

VOID
list(BOOL hidden)
{
	KernelModeQuery kernel = KernelModeQuery::getInstance();
	UserModeQuery user = UserModeQuery::getInstance();

	Processes procs;

	procs = user.getProcesses();
	kernel.reloadProcesses();
	kernel.mergeProcesses(procs);
	procs = kernel.getProcesses();

	if (procs.empty()) {
		printf("NULL\r\n");
	}

	printf(" PID   PaPID           Image File Path\r\n");

	ProcessConstIterator it = procs.begin();
	while (it != procs.end()) {
		printf("%-6lu  %-6lu",
			it->Pid,
			it->PaPid);
		if (it->Hidden) {
			printf(" �� �� �� �� �� ��");
		}
		else {
			printf("---------------------------------------------");
		}
		printf("\r\n");
		printf("[ %ws ]  ",it->ImageName.c_str());
		printf("    %ws\r\n", it->ImageFileName.c_str());
		++it;
	}
}

BOOL
hide(const std::string &name)
{
	return KernelModeQuery::getInstance().hideProcess(name);
}

BOOL
loadScannerDriver()
{
	printf("���� ProcListDriver.sys -> ProcScanner ...\r\n");
	//��������
	BOOL bRet = LoadNTDriver(DRIVER_NAME_SCANNER, DRIVER_PATH_SCANNER);
	if (!bRet)
	{
		printf("ProcListDriver.sys error\n");
		return FALSE;
	}
	//���سɹ�

	testDriver(DEVICE_LINK_SCANNER);

	return TRUE;
}

BOOL
loadGuardDriver()
{
	printf("���� ProcGuardDriver.sys -> ProcGuard ...\r\n");
	//��������
	BOOL bRet = LoadNTDriver(DRIVER_NAME_GUARD, DRIVER_PATH_GUARD);
	if (!bRet)
	{
		printf("ProcGuardDriver.sys error\n");
		return FALSE;
	}
	//���سɹ�

	testDriver(DEVICE_LINK_GUARD);

	return TRUE;
}

BOOL
testDriver(LPCWSTR link)
{
	//printf("���������Ƿ���سɹ�...\r\n");
	return TestDriver(link);
}

BOOL
unloadScannerDriver()
{
	printf("ж�� ProcScanner ...\r\n");
	if (!UnloadNTDriver(DRIVER_NAME_SCANNER))
	{
		printf("UnloadNTDriver error\n");
		return FALSE;
	}

	return TRUE;
}

BOOL
unloadGuardDriver()
{
	printf("ж�� ProcGuard ...\r\n");
	if (!UnloadNTDriver(DRIVER_NAME_GUARD))
	{
		printf("UnloadNTDriver error\n");
		return FALSE;
	}

	return TRUE;
}

VOID showHelp()
{
	std::cout
		<< "================================================================"
		<< std::endl
		<< "�������� Windows 7 sp1 x86 �ϲ�����Ч"
		<< std::endl
		<< "----------------------------------------------------------------"
		<< std::endl
		<< "ls/list              --�鿴�����б�����ʾ����ǳ������صĽ���"
		<< std::endl
		<< "guard <process name> --���ӽ��̼��ؾ����ļ����"
		<< std::endl
		<< "hide <process name>  --���ؽ���"
		<< std::endl
		<< "cls/clear            --����"
		<< std::endl
		<< "help                 --��ʾ����Ϣ"
		<< std::endl
		<< "load                 --��������"
		<< std::endl
		<< "unload               --ж������"
		<< std::endl
		<< "exit                 --�˳�"
		<< std::endl
		<< "================================================================"
		<< std::endl;
}

VOID cls()
{
	system("cls");
}

VOID guard(const std::string& targetName)
{
	Guard = TRUE;

	printf("��ʼ����, <Ctrl-C> ֹͣ \r\n");

	if (Guard && !procGuard.listen(targetName)) {
		printf("%ws\r\n", procGuard.getLastError().c_str());
	}

	printf("ֹͣ����\r\n");
}

VOID sig(int s)
{
	if (s == SIGINT) {
		if (Guard) {
			Guard = FALSE;
			procGuard.stop();
		}

		signal(SIGINT, sig);
	}
}