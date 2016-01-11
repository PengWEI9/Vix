#include <stdlib.h>
#include <string.h>
#include "cs.h"
#include "csf.h"
#include <string>
#include "version.h"
#include "csgetopt.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TITLE "Myki XML Import"



bool railTariff = false;

namespace
{
	const CsfUsage_t usage =
	{
		CSFCOPYRIGHT,
		TITLE,
		BUILDNAME,
		CSFBUILD,
		"usage: %P [-? | -h | --help] [-b | --build] [-V | --version] [--copyright]\n"\
		"       %P [-cvwm] [-d level] [-l log] [-x xml_location] [-f db_file_location]\n"\
		"		%P [-r | --rail]"
	};

	namespace Option
	{
		static const char ERROROPT     = '?';
		static const char HELP         = 'h';
		static const char BUILD        = 'b';
		static const char COPYRIGHT    = 1;
		static const char VERSION      = 'V';
		static const char NOT          = '!';
		static const char CONSOLE      = 'c';
		static const char VERBOSE      = 'v';
		static const char WARNINGS     = 'w';
		static const char DEBUG_LEVEL  = 'd';
		static const char LOGGER       = 'l';
		static const char XML_LOCATION = 'x';
		static const char DB_FILE_LOCATION    = 'f';
		static const char RAIL_TARIFF  = 'r';

		static const char* shortOptions = "?hbV!cvwmrd:l:x:f:";
		static struct option longOptions[] =
		{
			{ "help",       no_argument,        NULL,   HELP      },
			{ "build",      no_argument,        NULL,   BUILD     },
			{ "copyright",  no_argument,        NULL,   COPYRIGHT },
			{ "version",    no_argument,        NULL,   VERSION   },
			{ "rail",		no_argument,		NULL,	RAIL_TARIFF },
			{ NULL }
		};
	} // namespace Option

	const CsfHelp_t help[] =
	{
		{ " -?, -h, --help",		"Display this usage" },
		{ " -b, --build",			"Build information" },
		{ " -V, -version",			"Version details" },
		{ " --copyright",			"Copyright information" },
		{ " -[!]c",					"Enable console output" },
		{ " -[!]v",					"Turn on verbose messages" },
		{ " -[!]w",					"Turn on warning messages" },
		{ " -d level",				"Turn on debug messages below the given level" },
		{ " -l log",				"Set logging facility" },
		{ " -x XML location",		"Set the location of the XML files (dir, .zip or .tar)" },
		{ " -f DB file location",			"Set the location (directory) of database files" },
		{ " -r, --rail",			"Load Rail relevant tariff info only" },
		{ NULL, NULL }
	};
} // anonymous namespace


extern int ProcessXML(const std::string& dbFileLocation, const std::string& xmlDirectory, const std::string& xmlSource);


int main(int argc, char** argv)
{
	// Get the program name
	const char* progName = strrchr(argv[0], CsGetcds());
	if ( progName == 0 )
		progName = argv[0];
	else
		progName++;

    /* Options */
	bool displayHelp      = false;
	bool displayBuild     = false;
	bool displayCopyright = false;
	bool displayVersion   = false;
	bool argError         = false;
	std::string xmlLocation;
	std::string dbFileLocation;

	int flag = 1;
	CsMsgOptset(CSMSGOP_CONSOLE, &flag, (uint32_t)sizeof(int32_t));

	char c;
	CsGetoptReset();
	while ( (c = CsGetoptl(argc, argv, Option::shortOptions, Option::longOptions, 1)) != char(EOF) )
	{
		switch ( c )
		{
		case Option::ERROROPT:
			if ( optopt == Option::ERROROPT )
				displayHelp = true;
			else
				argError = true;
			break;
		case Option::HELP: displayHelp = true; break;
		case Option::RAIL_TARIFF: railTariff = true; break;
		case Option::BUILD: displayBuild = true; break;
		case Option::COPYRIGHT: displayCopyright = true; break;
		case Option::VERSION: displayVersion = true; break;
		case Option::NOT: flag = 0; break;
		case Option::CONSOLE:
			CsMsgOptset(CSMSGOP_CONSOLE, &flag, (uint32_t)sizeof(int32_t));
			flag = 1;
			break;
		case Option::VERBOSE:
			CsMsgOptset(CSMSGOP_VERBOSE, &flag, (uint32_t)sizeof(int32_t));
			flag = 1;
			break;
		case Option::WARNINGS:
			CsMsgOptset(CSMSGOP_WARNING, &flag, (uint32_t)sizeof(int32_t));
			flag = 1;
			break;
		case Option::DEBUG_LEVEL:
			flag = atoi(optarg);
			if ( (flag <= 0) || (flag > 9) )
			{
				fprintf(stderr, "%s: illegal debug level -- %s\n", progName, optarg);
				argError = true;
			}
			CsMsgOptset(CSMSGOP_DEBUG, &flag, (uint32_t)sizeof(int32_t));
			flag = 1;
			break;
		case Option::LOGGER:
			CsMsgOptset(CSMSGOP_LOGGER, &optarg, (uint32_t)sizeof(char*));
			break;
		case Option::XML_LOCATION:
			xmlLocation = optarg;
			break;
		case Option::DB_FILE_LOCATION:
			dbFileLocation = optarg;
			
			if (dbFileLocation.length() > 0 && dbFileLocation[dbFileLocation.length()-1] != '/') {
				dbFileLocation += "/";
			}
			break;
		default:
			argError = true;
		} // switch

		if ( (c != Option::NOT) && (flag != 1) )
		{
			argError = true;
			fprintf(stderr, "%s: ! applied to incorrect argument '%c'\n", progName, c);
		}
	} // while getting options

	if ( argc != optind )
	{
		argError = true;
		if ( argc > optind )
		{
			fprintf(stderr, "%s: unexpected argument -- %s\n", progName, argv[optind]);
		}
	}
	else if ( argc == 1 )
	{
		argError = true;
		fprintf(stderr, "%s: no arguments supplied\n", progName);
	}

	// See if we should display a usage message
	if ( argError )
		CsfUsage(0, argv[0], &usage, help);
	if ( displayBuild )
		fprintf(stderr, "%s\n", VersionString());
	if ( displayVersion && !displayCopyright )
		fprintf(stderr, "%s %s\n", usage.ou_version, usage.ou_build);
	if ( displayCopyright )
		CsfUsage(2, argv[0], &usage, help);
	if ( displayHelp )
		CsfUsage(1, argv[0], &usage, help);
	if ( displayBuild || displayVersion )
		exit(3);


	// Extract the archive, if required
	struct stat fileInfo;
	std::string extractedXmlLocation;
	bool extractedXmlOk = false;
	if ( stat(xmlLocation.c_str(), &fileInfo) != 0 )
	{
		fprintf(stderr, "%s: XML location '%s' does not exist\n", progName, xmlLocation.c_str());
	}
	else if ( (fileInfo.st_mode & S_IFDIR) != 0 )
	{
		extractedXmlLocation = xmlLocation;
		extractedXmlOk = true;
	}
	else if ( (fileInfo.st_mode & S_IFREG) != 0 )
	{
		std::string::size_type length = xmlLocation.length();
		std::string unpackCommand;
		char* tempDir = tempnam(NULL, "xml.m");
		extractedXmlLocation = tempDir;
		free(tempDir);

		if ( length > 4 )
		{
			if ( xmlLocation.substr(length - 4) == ".zip" )
			{
				unpackCommand =
					std::string("mkdir \"") + extractedXmlLocation + "\" && " +
					"unzip -d \"" + extractedXmlLocation + "\" \"" + xmlLocation + "\"";
			}
			else if ( xmlLocation.substr(length - 4) == ".tar" )
			{
				unpackCommand =
					std::string("mkdir \"") + extractedXmlLocation + "\" && " +
					"tar -x -f \"" + xmlLocation + "\" -C \"" + extractedXmlLocation + "\"";
			}
		}
		if ( unpackCommand.empty() )
		{
			fprintf(stderr, "%s: don't know what to do with file '%s'\n", progName, xmlLocation.c_str());
		}
		else if ( system(unpackCommand.c_str()) != 0 )
		{
			fprintf(stderr, "%s: unpacking of '%s' failed\n", progName, xmlLocation.c_str());
		}
		else
		{
			extractedXmlOk = true;
		}
	}

	int ret = 4;
	if ( extractedXmlOk )
	{
		try
		{
			ret = ProcessXML(dbFileLocation, extractedXmlLocation, xmlLocation);
		}
		catch (const char* e)
		{
			fprintf(stderr, "%s: processing XML failed: %s", progName, e);
			ret = 5;
		}
		catch (...)
		{
			fprintf(stderr, "%s: processing XML failed, unknown exception", progName);
			ret = 6;
		}

		if ( extractedXmlLocation != xmlLocation )
		{
			std::string command = std::string("rm -rf \"") + extractedXmlLocation + "\"";
			if ( system(command.c_str()) != 0 )
				fprintf(stderr, "%s: could not remove temporary directory '%s'", progName, extractedXmlLocation.c_str());
		}
	}

	return ret;
}
