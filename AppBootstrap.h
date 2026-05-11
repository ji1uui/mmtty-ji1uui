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

class IApplicationCommand
{
public:
	virtual ~IApplicationCommand() {}
	virtual void __fastcall Execute(void) = 0;
};

class IExceptionHandler
{
public:
	virtual ~IExceptionHandler() {}
	virtual void __fastcall Handle(Exception *exception) = 0;
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

class TVclMmttyRunCommand : public IApplicationCommand
{
public:
	virtual void __fastcall Execute(void);
};

class TVclExceptionHandler : public IExceptionHandler
{
public:
	virtual void __fastcall Handle(Exception *exception);
};

class TExceptionHandlingCommand : public IApplicationCommand
{
private:
	IApplicationCommand *m_Command;
	IExceptionHandler *m_ExceptionHandler;

public:
	__fastcall TExceptionHandlingCommand(IApplicationCommand *command,
		IExceptionHandler *exceptionHandler);
	virtual void __fastcall Execute(void);
};

class TMmttyBootstrapper
{
private:
	const IStartupPolicy *m_StartupPolicy;
	IApplicationCommand *m_ApplicationCommand;

public:
	__fastcall TMmttyBootstrapper(const IStartupPolicy *startupPolicy,
		IApplicationCommand *applicationCommand);
	int __fastcall Run(void);
};

class IApplicationBootstrapFactory
{
public:
	virtual ~IApplicationBootstrapFactory() {}
	virtual TMmttyBootstrapper * __fastcall Create(void) = 0;
};

class TDefaultMmttyBootstrapFactory : public IApplicationBootstrapFactory
{
private:
	TWin32CommandLineProvider m_CommandLine;
	TWin32WindowProbe m_WindowProbe;
	TSingleInstanceStartupPolicy m_StartupPolicy;
	TVclMmttyRunCommand m_RunCommand;
	TVclExceptionHandler m_ExceptionHandler;
	TExceptionHandlingCommand m_ExceptionHandlingCommand;
	TMmttyBootstrapper m_Bootstrapper;

public:
	__fastcall TDefaultMmttyBootstrapFactory();
	virtual TMmttyBootstrapper * __fastcall Create(void);
};

int __fastcall RunMmttyApplication(void);
//---------------------------------------------------------------------------
#endif
