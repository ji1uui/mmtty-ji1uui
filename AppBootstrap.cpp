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
#include <vcl.h>
#pragma hdrstop

#include <string.h>

#include "AppBootstrap.h"
#include "Main.h"
//---------------------------------------------------------------------------
LPSTR __fastcall TWin32CommandLineProvider::GetCommandLineText(void) const
{
	return GetCommandLine();
}
//---------------------------------------------------------------------------
HWND __fastcall TWin32WindowProbe::FindWindowByClassName(const char *className) const
{
	return FindWindow(className, NULL);
}
//---------------------------------------------------------------------------
BOOL __fastcall TWin32WindowProbe::IsVisible(HWND hWnd) const
{
	return IsWindowVisible(hWnd);
}
//---------------------------------------------------------------------------
__fastcall TSingleInstanceStartupPolicy::TSingleInstanceStartupPolicy(const ICommandLineProvider *commandLine,
	const IWindowProbe *windowProbe,
	const char *overrideSwitch,
	const char *mainWindowClassName)
{
	m_CommandLine = commandLine;
	m_WindowProbe = windowProbe;
	m_OverrideSwitch = overrideSwitch;
	m_MainWindowClassName = mainWindowClassName;
}
//---------------------------------------------------------------------------
BOOL __fastcall TSingleInstanceStartupPolicy::HasOverrideSwitch(void) const
{
	LPSTR commandLine = m_CommandLine->GetCommandLineText();
	return (commandLine != NULL) && (strstr(commandLine, m_OverrideSwitch) != NULL);
}
//---------------------------------------------------------------------------
BOOL __fastcall TSingleInstanceStartupPolicy::CanStart(void) const
{
	if( HasOverrideSwitch() ) return TRUE;

	HWND hWnd = m_WindowProbe->FindWindowByClassName(m_MainWindowClassName);
	if( hWnd == NULL ) return TRUE;

	return m_WindowProbe->IsVisible(hWnd);
}
//---------------------------------------------------------------------------
void __fastcall TVclMmttyRunCommand::Execute(void)
{
	Application->Initialize();
	Application->CreateForm(__classid(TMmttyWd), &MmttyWd);
	Application->Run();
}
//---------------------------------------------------------------------------
void __fastcall TVclExceptionHandler::Handle(Exception *exception)
{
	Application->ShowException(exception);
}
//---------------------------------------------------------------------------
__fastcall TExceptionHandlingCommand::TExceptionHandlingCommand(IApplicationCommand *command,
	IExceptionHandler *exceptionHandler)
{
	m_Command = command;
	m_ExceptionHandler = exceptionHandler;
}
//---------------------------------------------------------------------------
void __fastcall TExceptionHandlingCommand::Execute(void)
{
	try
	{
		m_Command->Execute();
	}
	catch (Exception &exception)
	{
		m_ExceptionHandler->Handle(&exception);
	}
}
//---------------------------------------------------------------------------
__fastcall TMmttyBootstrapper::TMmttyBootstrapper(const IStartupPolicy *startupPolicy,
	IApplicationCommand *applicationCommand)
{
	m_StartupPolicy = startupPolicy;
	m_ApplicationCommand = applicationCommand;
}
//---------------------------------------------------------------------------
int __fastcall TMmttyBootstrapper::Run(void)
{
	if( m_StartupPolicy->CanStart() ){
		m_ApplicationCommand->Execute();
	}
	return 0;
}
//---------------------------------------------------------------------------
__fastcall TDefaultMmttyBootstrapFactory::TDefaultMmttyBootstrapFactory()
	: m_StartupPolicy(&m_CommandLine, &m_WindowProbe, "-Z", "TMmttyWd"),
	m_ExceptionHandlingCommand(&m_RunCommand, &m_ExceptionHandler),
	m_Bootstrapper(&m_StartupPolicy, &m_ExceptionHandlingCommand)
{
}
//---------------------------------------------------------------------------
TMmttyBootstrapper * __fastcall TDefaultMmttyBootstrapFactory::Create(void)
{
	return &m_Bootstrapper;
}
//---------------------------------------------------------------------------
int __fastcall RunMmttyApplication(void)
{
	TDefaultMmttyBootstrapFactory factory;
	TMmttyBootstrapper *bootstrapper = factory.Create();

	return bootstrapper->Run();
}
//---------------------------------------------------------------------------
