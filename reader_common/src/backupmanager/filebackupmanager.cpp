/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief FileBackupManager class implementation.
 */

#include "filebackupmanager.h"
#include <crc32.h>
#include <csf.h>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

using namespace std;
using namespace ReaderCommon;

/**
 * @brief Compares two C-strings. Used by the sort() function.
 * @param left A C-string to compare.
 * @param right A C-string to compare.
 * @return true if left is greater than right, otherwise false.
 */
static bool cmpStr(const char * left, const char * right)
{
    return (strncmp(left, right, strlen(left)) > 0);
}

/**
 * @brief Returns the current time as string in the format "%Y%m%d%H%M%S".
 * @return Current time as a string.
 */
static string timeStr(time_t timestamp)
{
    //current time
    struct tm timeinfo;
    localtime_r(&timestamp, &timeinfo);

    // format string
    char buffer[BUFSIZ];
    strftime(buffer, BUFSIZ, "%Y%m%d%H%M%S", &timeinfo);

    return buffer;
}

FileBackupManager::FileBackupManager(const string &backupDir, 
        const string &filePrefix, const string &fileSuffix = "") :
    m_backupDir(backupDir),
    m_filePrefix(filePrefix),
    m_fileSuffix(fileSuffix) 
{
    CsDbg(7, "FileBackupManager::FileBackupManager: backupDir=%s, "
            "filePrefix=%s, fileSuffix=%s", 
            m_backupDir.c_str(), m_filePrefix.c_str(), m_fileSuffix.c_str());

    if (*m_backupDir.rbegin() != '/')
    {
        m_backupDir += "/";
    }

    // create backup directory. CsMkdir will fail silently if it exists already
    if (CsMkdir(m_backupDir.c_str(), 0755) != 0)
    {
        throw runtime_error("Unable to create dir '" + m_backupDir + "'.");
    }
}

bool FileBackupManager::isValidFile(const string &fileName) const
{
    string path = m_backupDir + fileName;

    CsDbg(7, "FileBackupManager::isValidFile: checking %s", path.c_str());

    unsigned long actualCrc = get_file_crc(path.c_str());
    unsigned long expectedCrc = 0;

    string format = m_filePrefix + "_%*d_%u" + m_fileSuffix;

    if (sscanf(fileName.c_str(), format.c_str(), &expectedCrc) != 1)
    {
        throw runtime_error("Unable to read CRC from filename: " + fileName);
    }

    CsDbg(7, "FileBackupManager::isValidFile: %s, actualCrc=%u, expectedCrc=%u",
            path.c_str(), actualCrc, expectedCrc);

    return (actualCrc == expectedCrc);
}

string FileBackupManager::getLatestValidFile() const
{
    CsGlob_t glob;
    string globStr = m_filePrefix + "*" + m_fileSuffix;

    if (CsGlob(m_backupDir.c_str(), &glob, globStr.c_str()) != 0)
    {
        throw runtime_error("call to CsGlob failed.");
    }

    if (glob.gl_argc == 0)
    {
        CsDbg(7, "FileBackupManager::getLatestValidFile: no files");
    }

    // sort alphabetically (date is written in filename)
    sort(glob.gl_argv, glob.gl_argv + glob.gl_argc, cmpStr);

    string latestFile = "";

    // find the latest VALID file
    for (int i = 0; i < glob.gl_argc; ++i)
    {
        if (isValidFile(glob.gl_argv[i]))
        {
            latestFile = m_backupDir + glob.gl_argv[i];
            break;
        }
    }

    CsGlobFree(&glob);
    return latestFile;
}

string FileBackupManager::createFileName(const string &data, 
        time_t timestamp) const
{
    const unsigned char *tmp = 
        reinterpret_cast<const unsigned char *>(data.c_str());

    unsigned long crc = calcCRC32(tmp, data.size() * sizeof(char));

    stringstream ss;
    ss << m_filePrefix << "_" << timeStr(timestamp) << "_" 
        << crc << m_fileSuffix;

    return ss.str();
}

void FileBackupManager::cleanOldFiles() const
{
    CsGlob_t glob;

    string globStr = m_filePrefix + "*" + m_fileSuffix;

    if (CsGlob(m_backupDir.c_str(), &glob, globStr.c_str()) != 0)
    {
        throw runtime_error("Call to CsGlob failed.");
    }

    // sort alphabetically (date is written in filename)
    sort(glob.gl_argv, glob.gl_argv + glob.gl_argc, cmpStr);

    // start at 1, so we skip the file we just created
    for (int i = 1; i < glob.gl_argc; ++i)
    {
        string filePath = m_backupDir + string(glob.gl_argv[i]);

        CsDbg(6, "FileBackupManager:cleanOldFiles: removing file %s",
                filePath.c_str());

        if (remove(filePath.c_str()) != 0)
        {
            throw runtime_error("Failed to remove file '" + filePath + "'.");
        }
    }

    CsGlobFree(&glob);
}

void FileBackupManager::save(const string &data)
{
    save(data, time(NULL));
}

void FileBackupManager::save(const string &data, time_t timestamp)
{
    string path = m_backupDir + createFileName(data, timestamp);

    CsDbg(4, "FileBackupManager::save: path=%s", path.c_str());

    ofstream stream(path.c_str());
    stream << data;

    if (stream.fail())
    {
        throw FileBackupWriteException();
    }

    stream.close();

    try {
        cleanOldFiles();
    }
    catch (runtime_error &e)
    {
        // Log errors, but don't rethrow. This is because despite failing to
        // clean old files, we still managed to save to file.
        CsErrx("FileBackupManager::save: %s", e.what());
    }

    CsDbg(4, "FileBackupManager::save: wrote to %s", path.c_str());

}

string FileBackupManager::load()
{
    const string filePath = getLatestValidFile();

    if (filePath.empty())
    {
        CsDbg(4, "FileBackupManager::load: No files to read from.");
        return "";
    }

    CsDbg(4, "FileBackupManager::load: Reading from %s", filePath.c_str());

    ifstream stream(filePath.c_str());

    if (stream.fail())
    {
        throw FileBackupReadException();
    }

    return string((istreambuf_iterator<char>(stream)), 
            istreambuf_iterator<char>());
}
