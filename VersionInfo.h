//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 2000-2013 Makoto Mori, Nobuyuki Oba, Dave Bernstein
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMTTY.
//
// MMTTY is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// MMTTY is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with MMTTY.  If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------

// VersionInfo.h
//
// Holds version identifiers and the strings derived from them.
// Kept as a tiny standalone header so bumping the version number does not
// trigger a recompile of every translation unit that includes ComLib.h.

#ifndef VersionInfoH
#define VersionInfoH

#define VERID    "Ver1.80"
#define VERBETA  "A"

#define VERTTL2  "MMTTY " VERID VERBETA
#define VERTTL   VERTTL2 " (C) JE3HHT 2000-2010."

#define SETUPTITLE        "Setup MMTTY " VERID VERBETA
#define SETUPTITLEREMOTE  "Setup " VERID VERBETA

#endif // VersionInfoH
