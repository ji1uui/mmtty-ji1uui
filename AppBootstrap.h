//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 2000-2013 Makoto Mori, Nobuyuki Oba
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMTTY.

// MMTTY is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMTTY is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMTTY.  If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------
#ifndef AppBootstrapH
#define AppBootstrapH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
class ICommandLineProvider
{
public:
	virtual ~ICommandLineProvider() {}
	virtual LPSTR __fastcall GetCommandLineText(void) const = 0;
};

class IWindowProbe
{
public:
	virtual ~IWindowProbe() {}
	virtual HWND __fastcall FindWindowByClassName(const char *className) const = 0;
	virtual BOOL __fastcall IsVisible(HWND hWnd) const = 0;
};

class IStartupPolicy
{
public:
	virtual ~IStartupPolicy() {}
	virtual BOOL __fastcall CanStart(void) const = 0;
};

class IVclApplicationRunner
{
public:
	virtual ~IVclApplicationRunner() {}
	virtual void __fastcall Initialize(void) = 0;
	virtual void __fastcall CreateMainForm(void) = 0;
	virtual void __fastcall Run(void) = 0;
	virtual void __fastcall ShowException(Exception *exception) = 0;
};

class TWin32CommandLineProvider : public ICommandLineProvider
{
public:
	virtual LPSTR __fastcall GetCommandLineText(void) const;
};

class TWin32WindowProbe : public IWindowProbe
{
public:
	virtual HWND __fastcall FindWindowByClassName(const char *className) const;
	virtual BOOL __fastcall IsVisible(HWND hWnd) const;
};

class TSingleInstanceStartupPolicy : public IStartupPolicy
{
private:
	const ICommandLineProvider *m_CommandLine;
	const IWindowProbe *m_WindowProbe;
	const char *m_OverrideSwitch;
	const char *m_MainWindowClassName;

	BOOL __fastcall HasOverrideSwitch(void) const;

public:
	__fastcall TSingleInstanceStartupPolicy(const ICommandLineProvider *commandLine,
		const IWindowProbe *windowProbe,
		const char *overrideSwitch,
		const char *mainWindowClassName);
	virtual BOOL __fastcall CanStart(void) const;
};

class TVclMmttyApplicationRunner : public IVclApplicationRunner
{
public:
	virtual void __fastcall Initialize(void);
	virtual void __fastcall CreateMainForm(void);
	virtual void __fastcall Run(void);
	virtual void __fastcall ShowException(Exception *exception);
};

class TMmttyBootstrapper
{
private:
	const IStartupPolicy *m_StartupPolicy;
	IVclApplicationRunner *m_ApplicationRunner;

public:
	__fastcall TMmttyBootstrapper(const IStartupPolicy *startupPolicy,
		IVclApplicationRunner *applicationRunner);
	int __fastcall Run(void);
};

int __fastcall RunMmttyApplication(void);
//---------------------------------------------------------------------------
#endif
