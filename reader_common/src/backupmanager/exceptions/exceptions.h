/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains custom exception classes.
 */

#ifndef READER_COMMON_BACKUPMANAGER_EXCEPTIONS_H
#define READER_COMMON_BACKUPMANAGER_EXCEPTIONS_H

#include <exception>
#include <string>

namespace ReaderCommon
{
    /**
     * @brief Base exception class.
     */
    class BackupException : public std::exception
    {
        private:
            const std::string m_msg;

        public:
            explicit BackupException(const std::string &msg) : m_msg(msg) {};
            virtual const char * what() const throw() { return m_msg.c_str(); };
            virtual ~BackupException() throw() {};
    };

    /**
     * @brief Exception thrown on failure loading backup.
     */
    class BackupReadException : public BackupException 
    { 
        public:
            explicit BackupReadException(const std::string &msg) : 
                BackupException(msg) {};
    };

    /**
     * @brief Exception thrown on failure reading backup.
     */
    class BackupWriteException : public BackupException 
    {
        public:
            explicit BackupWriteException(const std::string &msg) :
                BackupException(msg) {};
    };

};

#endif
