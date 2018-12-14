#pragma once

// Copyright (c) 2018, Paul Semel
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#include <array>
#include <span>
#include <string_view>
#include <vector>


enum position {
	NoPosition,
	End,
};

enum OptionType {
	Command,
	Opt,
	Positional,
};

class BasicOption {
	public:
		std::string_view Desc;
		std::string_view Name;
		bool Initialized;
		enum OptionType Type;
		BasicOption(const char *Name, const char *Description,
			    enum OptionType Type, enum position pos);
		virtual void addArgument(const std::string_view Arg) {};
		virtual void isPresent(void) { Initialized = true; };
		virtual bool hasArg(void) = 0;
		virtual void toHelpRepr(void);
		operator bool() const { return Initialized; };
};

template <class T>
class Option;

struct Alias {
};

template<>
class Option<Alias> : public BasicOption {
	private:
		BasicOption *Alias;
	public:
		Option(const char *Name, const char *Description, BasicOption *alias);
		Option(const Option &) = delete;
		void isPresent(void);
		bool hasArg(void) { return Alias->hasArg(); };
		void addArgument(const std::string_view Arg) { Alias->addArgument(Arg); };
};

template <>
class Option<bool> : public BasicOption {
	public:
		Option(const char *Name, const char *Description,
		       enum OptionType Type, enum position pos = NoPosition);
		Option(const Option &) = delete;
		bool hasArg(void) { return false; };
};

template <>
class Option<std::string> : public BasicOption {
	private:
		std::string_view Value;
	public:
		Option(const char *Name, const char *Description,
		       enum OptionType Type, enum position pos = NoPosition);
		void addArgument(const std::string_view Arg);
		bool hasArg(void);
		std::string_view getValue(void);
		void toHelpRepr(void);
};

template <>
class Option<int> : public BasicOption {
	private:
		int Value;
		bool isSet;
	public:
		Option(const char *Name, const char *Description,
		       enum OptionType Type, enum position pos = NoPosition);
		void addArgument(const std::string_view Arg);
		bool hasArg(void);
		int getValue(void);
		void toHelpRepr(void);
};

class CommandLineParser {
	private:
		std::vector<BasicOption *> Commands;
		BasicOption *endPosArg;
		BasicOption *getMatchingOption(std::string_view OptName);
		void handleError(std::string_view Arg);
		std::string_view ProgName;
	public:
		void addOption(BasicOption *O, enum position pos);
		void parseCommandLine(std::span<const char *> args);
		CommandLineParser() noexcept;
		void generateHelp(void);
		void setProgramName(std::string_view s) { ProgName = s; };
};

void CLParse(std::span<const char *> args, const char *ProgName);
void help(void);
