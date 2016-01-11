/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief TxnBackupManager class interface.
 */

#ifndef DC_TXNLIST_TXNBACKUPMANAGER_H
#define DC_TXNLIST_TXNBACKUPMANAGER_H

#include "types.h"
#include <backupmanager/filebackupmanager.h>
#include <string>
#include <map>

namespace TxnList {

    // forward declarations
    class TxnManager;

    /**
     * @brief Manages storing and retrieving transaction data to/from files.
     *
     * Is implemented in terms of a FileBackupManager instance, which can be
     * found in the reader-common package.
     */
    class TxnBackupManager
    {
        private:
            /*! Used to save to/read from file. */
            ReaderCommon::FileBackupManager m_fileBackupManager;

        public:
            TxnBackupManager();

            /**
             * @brief Saves transaction data to backup JSON file.
             * @param txnMap The transaction data to write to file.
             * @throw runtime_error If saving data fails.
             */
            void save(const TxnMap &txnMap);

            /**
             * @brief Loads transaction data from backup JSON file.
             *
             * If we are unable to find a valid file to read from, the function
             * will do nothing.
             * 
             * @param txnMap A Reference to a map to store loaded data into.
             * @throw runtime_error If loading data fails.
             */
            void load(TxnManager &txnManager);
    };
}

#endif
