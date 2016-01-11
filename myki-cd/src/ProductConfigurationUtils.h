/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : ProductConfigurationUtils.c
**  Author(s)       : James Ho
**
**  Description     :
**
**  Member(s)       :
**      getActionlistDBAccess   [public]    Gets actionlist database object
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  12.10.12    JH    Create
**
**===========================================================================*/

#ifndef PRODUCTCONFIGURATIONUTILS_H_
#define PRODUCTCONFIGURATIONUTILS_H_

#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <memory.h>

#include <cs.h>
#include <corebasetypes.h>
#include <coretypes.h>
#include "DbAccess.h"
#include "pugixml.hpp"
#include <set>
#include <vector>

#define CD_DEBUG 3

#define I_FULL_ACTIONLIST	0
#define I_DELTA_ACTIONLIST	1

extern  const std::string   productCfgDBFile;       /**< Default product configuration database path name */

std::string sqlify(std::string s);
U32_t convertStringToU32(const std::string& string);
U32_t convertStringToU32(const char* string);
U16_t convertStringToU16(const std::string& string);
U16_t convertStringToU16(const char* string);
U8_t convertStringToU8(const std::string& string);
U8_t convertStringToU8(const char* string);
bool convertStringToBool(const std::string& string);
bool convertStringToBool(const char* string);
void stringCopy(char* target, const std::string& source, unsigned int targetLen);
void stringCopy(char* target, const char* source, unsigned int targetLen);
void stringMemCopy(void* target, const char* source, unsigned int targetLen);
DbAccess* getDBAccess(const char* filename=NULL);

    /**
     *  Destroys current Tariff database object (if any).
     */
void        releaseDBAccess( void );

void setDeltaActionListFilename (const char* filename);
void setFullActionListFilename (const char* filename);
const std::string getDeltaActionListFilename ();
const std::string getFullActionListFilename ();


    /**
     *  Gets full or delta actionlist database object.
     *  @param  bDelta      true = Delta; false = Full.
     *  @param  filename    actionlist database path name or NULL
     *                      for last connected actionlist database.
     *  @return Connected actionlist database object or NULL.
     */
DbAccess   *getActionlistDBAccess( bool bDelta = false, const char *filename = NULL );

    /**
     *  Destroys current full/delta actionlist database object (if any).
     *  @param  bDelta      true = Delta; false = Full.
     */
void    releaseActionlistDBAccess( bool bDelta = false );

int processXML(
	const char* description,
	const char* xmlFile,
	const char* xmlNode,
	const char* masterTable,
	const char* recordTable,
	int (*subtableFunction)(pugi::xpath_node node, int recordPK, std::vector<int>& subtablePKs) = 0,
	const char* subtableDeleteQuery = 0,
	std::set<std::string>* excludedAttributes = 0);

int initLookupTables();
int initActionlistLookupTables();

int getCurrentCDMajorVersion();
int getCurrentCDMinorVersion();
int validateTariff();
const char* getCurrentCDSource();
Time_t getCurrentCDStartTime();
const char* getCurrentCDVersionWhere();
int initCurrentCDVersion(Time_t currentTime);

#endif /* PRODUCTCONFIGURATIONUTILS_H_ */
