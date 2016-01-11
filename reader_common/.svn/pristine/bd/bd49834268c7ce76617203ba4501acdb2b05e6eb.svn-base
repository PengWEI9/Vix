/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief BBRamBackupManager class interface.
 */

#ifndef READER_COMMON_BBRAMBACKUPMANAGER_H
#define READER_COMMON_BBRAMBACKUPMANAGER_H

#include "backupmanager.h"
#include <backupmanager/bbramexceptions.h>
#include <serpent/drv_bbsram.h>
#include <string> 
#include <cstring>

namespace ReaderCommon {

    class BBRamBackupManager : public BackupManager
    {
        private:
            const std::string m_partitionName;
            DrvBbSramPartition m_partition;

        public:
            BBRamBackupManager(const std::string &partitionName);
            virtual ~BBRamBackupManager();

            /**
             * Saves data to BBRAM partition.
             * @param data The data to save.
             * @throw BBRamBackupInitException If call to DrvBbSramOpenByName 
             *        fails.
             * @throw BBRamBackupSizeException If given data exceeds size of 
             *        BBRAM partition.
             */
            virtual void save(const std::string &data);

            /**
             * Saves data to BBRAM partition.
             * @param data The data to save.
             * @param len The number of characters to write.
             * @throw invalid_argument If data is NULL.
             * @throw BBRamBackupInitException If call to DrvBbSramOpenByName 
             *        fails.
             * @throw BBRamBackupSizeException If given data exceeds size of 
             *        BBRAM partition.
             */
            virtual void save(const char * data, size_t len);

            /**
             * Loads data from BBRAM partition.
             * return Data.
             */ 
            virtual std::string load();

            /**
             * @brief Returns the size (number of bytes) of the data stored in 
             *        the BBRAM partition.
             * @return Size of data.
             */
            inline size_t size() const
            {
                return strnlen(static_cast<char*>(m_partition.memory), 
                        m_partition.size);
            }

            /**
             * @brief Returns the size (number of bytes) of the BBRAM partition.
             * @return BBRAM partition size.
             */
            inline size_t partitionSize() const
            {
                return m_partition.size;
            }
    };
}

#endif
