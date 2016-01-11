/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $rev$
 * @brief Contains the PersistentUserId class interface.
 */

#ifndef VGAC_USERID_H
#define VGAC_USERID_H

// Suppress non-serious Boost warnings (see http://stackoverflow.com/a/1814618).
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif

#include <boost/thread/mutex.hpp>
#include <string>
#include <stdexcept>

/**
 * @brief Represents a User ID, while also storing it persistently to file.
 *
 * This class is THREAD-SAFE!
 */
class PersistentUserId
{
    private:
        boost::mutex m_mutex;
        std::string m_filePath;
        std::string m_userId;

        /**
         * @brief Loads the User ID from file.
         */ 
        void load();
        
    public:
        PersistentUserId(const std::string &filePath) : m_filePath(filePath)
        {
            load();
        }

        /**
         * @return User ID.
         */
        const std::string &get() const { return m_userId; }
    
        /**
         * @brief Assignment operator - sets the User ID and saves to file.
         * @param userId The new User ID.
         */
        void operator=(const std::string &userId);
};

#endif
