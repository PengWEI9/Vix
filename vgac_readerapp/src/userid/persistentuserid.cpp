/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $rev$
 * @brief Contains the PersistentUserId class implementation.
 */

#include "persistentuserid.h"
#include <csf.h>
#include <boost/thread/lock_guard.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

using namespace std;
using namespace boost;

void PersistentUserId::load()
{
    if (!boost::filesystem::exists(m_filePath))
    {
        CsDbg(4, "PersistentUserId: no file at %s", m_filePath.c_str());
        return;
    }

    CsDbg(4, "PersistentUserId: loading from file %s", m_filePath.c_str());

    lock_guard<mutex> guard(m_mutex);

    ifstream stream(m_filePath.c_str());
    stream >> m_userId;

    if (stream.fail()) 
    {
        CsErrx("PersistentUserId: failed to read from %s", m_filePath.c_str());
    }
    else
    {
        CsDbg(4, "PersistentUserId: loaded '%s' from file", m_userId.c_str());
    }
}

void PersistentUserId::operator=(const string &userId)
{
    CsDbg(4, "PersistentUserId: saving to file %s.", m_filePath.c_str());

    lock_guard<mutex> guard(m_mutex);

    m_userId = userId;
    ofstream stream(m_filePath.c_str());
    stream << m_userId;

    if (stream.fail()) 
    {
        CsErrx("PersistentUserId: failed to write to %s", m_filePath.c_str());
    }
    else
    {
        CsDbg(4, "PersistentUserId: saved '%s' to file.", m_userId.c_str());
    }
}
