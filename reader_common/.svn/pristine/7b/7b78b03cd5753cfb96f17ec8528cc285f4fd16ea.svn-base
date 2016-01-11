/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains custom exception classes for the FileBackupManager class.
 */

#ifndef READER_COMMON_BACKUPMANAGER_FILEEXCEPTIONS_H
#define READER_COMMON_BACKUPMANAGER_FILEEXCEPTIONS_H

#include <backupmanager/exceptions.h>

namespace ReaderCommon
{
    /**
     * @brief Exception thrown on failure reading file backup.
     */
    class FileBackupReadException : public BackupReadException
    {
        public:
            FileBackupReadException() : 
                BackupReadException("Error reading data from file.") {};
    };

    /**
     * @brief Exception thrown on failure writing to backup file.
     */
    class FileBackupWriteException : public BackupWriteException
    {
        public:
            FileBackupWriteException() : 
                BackupWriteException("Error writing data to file.") {};
    };
};

#endif
