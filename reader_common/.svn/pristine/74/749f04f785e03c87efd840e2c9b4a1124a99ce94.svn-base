/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains custom exception classes for the BBRamBackupManager class.
 */

#ifndef READER_COMMON_BACKUPMANAGER_BBRAMEXCEPTIONS_H
#define READER_COMMON_BACKUPMANAGER_BBRAMEXCEPTIONS_H

#include <backupmanager/exceptions.h>

namespace ReaderCommon
{
    /**
     * @brief Exception thrown on failure reading from BBRAM.
     */
    class BBRamBackupReadException : public BackupReadException
    {
        public:
            BBRamBackupReadException() : 
                BackupReadException("Error reading data from BBRAM.") {};
    };

    /**
     * @brief Exception thrown on failure writing to BBRAM.
     */
    class BBRamBackupWriteException : public BackupWriteException
    {
        public:
            BBRamBackupWriteException() : 
                BackupWriteException("Error writing data to BBRAM.") {};
    };

    /**
     * @brief Exception thrown on failure writing to BBRAM due to insufficient 
     *        size.
     */
    class BBRamBackupSizeException : public BackupException 
    {
        public:
            BBRamBackupSizeException() :
                BackupException("Error writing data to BBRAM -"
                        " insufficient size.") {};
    };

    /**
     * @brief Exception thrown on failure initializing BBRAM.
     */
    class BBRamBackupInitException : public BackupException
    {
        public:
            BBRamBackupInitException() :
                BackupException("Error initializing BBRAM - call to"
                        " DrvBbSramOpenByName failed.") {};
    };
};

#endif
