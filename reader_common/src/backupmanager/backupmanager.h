/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief BackupManager class interface.
 */

#ifndef READER_COMMON_BACKUPMANAGER_H
#define READER_COMMON_BACKUPMANAGER_H

#include <backupmanager/exceptions.h>
#include <string>

namespace ReaderCommon {

    /**
     * @brief Abstract base class for managing backups of data.
     */
    class BackupManager 
    {
        public:
            BackupManager() {};
            virtual ~BackupManager() {};

            /**
             * @brief Saves data to backup medium.
             * @param data The data to write.
             */
            virtual void save(const std::string &data) = 0;

            /**
             * @brief Loads data from backup.
             * @return Backup data.
             */
            virtual std::string load() = 0;
    };
}

#endif
