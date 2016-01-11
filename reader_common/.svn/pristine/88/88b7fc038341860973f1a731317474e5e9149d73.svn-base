/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief FileBackupManager class interface.
 */

#ifndef READER_COMMON_FILEBACKUPMANAGER_H
#define READER_COMMON_FILEBACKUPMANAGER_H

#include <backupmanager/backupmanager.h>
#include <backupmanager/fileexceptions.h>
#include <string>

namespace ReaderCommon {

    /**
     * @brief Manages backing up data to/reading from file.
     *
     * Files are stored in a given location, and file names include the date and
     * CRC of the file.
     *
     * When a new file is successfully created, old files are removed.
     *
     * CRCs are used to check the integrity of files.
     */
    class FileBackupManager : public BackupManager
    {
        private:
            std::string m_backupDir;
            const std::string m_filePrefix;
            const std::string m_fileSuffix;

            /**
             * @brief Verifies if the file's CRC matches the one in its name.
             * @param fileName The name of the file to verify.
             * @return true if valid, otherwise false.
             */
            bool isValidFile(const std::string &fileName) const;

            /**
             * @brief Finds the latest usable backup file to load from. 
             * @throw runtime_error If globbing fails.
             * @return Empty string if there are no files to load from. 
             * @return Path to the latest usable backup file.
             */
            std::string getLatestValidFile() const;

            /**
             * @brief Creates a filename of format prefix_date_crc_suffix.
             * @param data The data to create a CRC from.
             * @param timestamp The time to use in the filename.
             * @return filename.
             */
            std::string createFileName(const std::string &data, 
                    time_t timestamp) const;

            /**
             * @brief Deletes old files from backup directory.
             */
            void cleanOldFiles() const;

        public:
            /**
             * @brief Constructor
             * @param backupDir The location to backup files to.
             * @param filePrefix Prefix attached to each file created.
             * @param fileSuffix Suffix attached to each file created, eg .txt.
             */
            FileBackupManager(const std::string &backupDir, 
                    const std::string &filePrefix,
                    const std::string &fileSuffix);

            /**
             * @brief Saves data to backup file.
             * @param data The data to write.
             * @throw runtime_error If writing to file fails.
             */
            virtual void save(const std::string &data);

            /**
             * @brief Saves data to backup file.
             * @param data The data to write.
             * @param timestamp The time to use in the filename.
             * @throw runtime_error If writing to file fails.
             */
            virtual void save(const std::string &data, time_t timestamp);

            /**
             * @brief loads data from backup file.
             * @throw runtime_error If globbing fails.
             * @throw runtime_error If reading from a file fails.
             * @return Backup data.
             */
            virtual std::string load();
    };
}

#endif
