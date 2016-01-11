/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief BBRamBackupManager class implementation.
 */

#include "bbrambackupmanager.h"
#include <csf.h>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace ReaderCommon;

BBRamBackupManager::BBRamBackupManager(const string &partitionName) :
    m_partitionName(partitionName)
{
    CsDbg(4, "BBRamBackupManager: attempt to load partition %s", 
            partitionName.c_str());

    if (DrvBbSramOpenByName(&m_partition, m_partitionName.c_str()) < 0)
    {
        throw BBRamBackupInitException();
    }

    CsDbg(4, "BBRamBackupManager: partition name: %s, partition size: %d", 
            m_partitionName.c_str(), m_partition.size);
}

BBRamBackupManager::~BBRamBackupManager()
{
    DrvBbSramClose(&m_partition);
}

void BBRamBackupManager::save(const string &data)
{
    save(data.c_str(), data.size());
}

void BBRamBackupManager::save(const char * data, size_t len)
{
    CsDbg(4, "BBRamBackupManager::save: attempting to save to partition %s",
            m_partitionName.c_str());

    if (data == NULL)
    {
        throw invalid_argument("NULL pointer given.");
    }

    int numBytes = len * sizeof(char);

    // is data too big?
    if (numBytes > m_partition.size)
    {
        throw BBRamBackupSizeException();
    }

    memcpy(m_partition.memory, data, numBytes);
    
    // fill remaining memory with 0s
    memset(static_cast<char *>(m_partition.memory) + numBytes, 0, 
            m_partition.size - numBytes);
}

string BBRamBackupManager::load()
{
    CsDbg(4, "BBRamBackupManager::load: loading from partition %s", 
            m_partitionName.c_str());

    // get determine length of string in memory
    size_t maxChars = m_partition.size / sizeof(char);
    size_t len = strnlen(static_cast<char*>(m_partition.memory), maxChars);

    return string(static_cast<char *>(m_partition.memory), len);
}
