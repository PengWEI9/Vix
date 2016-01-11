/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author: bbrcan $
 * $Date: 2015-12-24 12:36:38 +1100 (Thu, 24 Dec 2015) $
 * $Rev: 87690 $
 * @brief PrinterBackupManager class interface.
 */

#ifndef DC_PRINTERBACKUPMANAGER_H
#define DC_PRINTERBACKUPMANAGER_H

#include <backupmanager/filebackupmanager.h>
#include <printerbackup/typedefs.h>
#include <message_dc.h>
#include <string>
#include <ctime>
#include <cstring>

namespace PrinterBackup {

    /**
     * @brief Manages persistent backups of IPC Printer messages.
     */
    class PrinterBackupManager
    {
        private:
            IpcMessageQueueId m_printerTaskId;
            ReaderCommon::FileBackupManager m_backupManager;

        public:
            PrinterBackupManager(IpcMessageQueueId printerTaskId);

            /**
             * @brief Saves an IPC printer message for 'tpurse load' to file.
             * @param tpurseLoad The data to write to file.
             * @param timestamp Timesatmp that gets written to the filename.
             */
            void save(const IPC_Printer_TpurseLoad_t &tpurseLoad, 
                    time_t timestamp = time(NULL));

            /**
             * @brief Saves an IPC printer message for 'tpurse reversal' to file.
             * @param tpurseReversal The data to write to file.
             * @param timestamp Timesatmp that gets written to the filename.
             */
            void save(const IPC_Reversal_TpurseLoad_t &tpurseReversal, 
                    time_t timestamp = time(NULL));

            /**
             * @brief Saves an IPC printer message for 'driver shift report' to 
             *        file.
             * @param shiftReport The data to write to file.
             * @param timestamp Timesatmp that gets written to the filename.
             */
            void save(const IPC_DriverShiftReport_t &shiftReport, 
                    time_t timestamp = time(NULL));

            /**
             * @brief Saves a string IPC message to file.
             * @param data The data to write to file.
             * @param timestamp Timesatmp that gets written to the filename.
             */
            void save(const std::string &data, time_t timestamp = time(NULL));

            /**
             * @brief Reprints the last printer message saved.
             * @throw runtime_error If there's an error reading the data.
             * @throw runtime_error If there's an error sending the IPC message.
             */
            void reprint();
    };
}

#endif
