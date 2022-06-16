/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DatabaseLoader.h"
#include "Updater/DBUpdater.h"
#include "Config.h"

#include <mysqld_error.h>

DatabaseLoader::DatabaseLoader(std::string const& logger, uint32 const defaultUpdateMask)
    : _logger(logger), _autoSetup(ConfigMgr::GetBoolDefault("Updates.AutoSetup", true)),
    _updateFlags(ConfigMgr::GetIntDefault("Updates.EnableDatabases", defaultUpdateMask))
{
}

template <class T>
DatabaseLoader& DatabaseLoader::AddDatabase(DatabaseWorkerPool<T>& pool, std::string const& name)
{
    bool const updatesEnabledForThis = DBUpdater<T>::IsEnabled(_updateFlags);

    _open.push([this, name, updatesEnabledForThis, &pool]() -> bool
    {
        std::string const dbString = ConfigMgr::GetStringDefault(std::string(name + std::string("DatabaseInfo")).c_str(), "");
        if (dbString.empty())
        {
            sLog->outError(LOG_FILTER_SQL, "Database %s not specified in configuration file!", name.c_str());
            return false;
        }

        uint8 const asyncThreads = uint8(ConfigMgr::GetIntDefault(std::string(name + std::string("Database.WorkerThreads")).c_str(), 1));
        if (asyncThreads < 1 || asyncThreads > 32)
        {
            sLog->outError(LOG_FILTER_SQL, "%s database: invalid number of worker threads specified. "
                "Please pick a value between 1 and 32.", name.c_str());
            return false;
        }

        uint8 const synchThreads = uint8(ConfigMgr::GetIntDefault(std::string(name + std::string("Database.SynchThreads")).c_str(), 1));

        pool.SetConnectionInfo(dbString, asyncThreads, synchThreads);

        if (pool.Open())
            return false;

        // Add the close operation
        _close.push([&pool]
        {
            pool.Close();
        });
        return true;
    });

    // Populate and update only if updates are enabled for this pool
    if (updatesEnabledForThis)
    {
        _populate.push([this, name, &pool]() -> bool
        {
            if (!DBUpdater<T>::Populate(pool))
            {
                sLog->outError(LOG_FILTER_SQL, "Could not populate the %s database, see log for details.", name.c_str());
                return false;
            }
            return true;
        });

        _update.push([this, name, &pool]() -> bool
        {
            if (!DBUpdater<T>::Update(pool))
            {
                sLog->outError(LOG_FILTER_SQL, "Could not update the %s database, see log for details.", name.c_str());
                return false;
            }
            return true;
        });
    }

    _prepare.push([this, name, &pool]() -> bool
    {
        if (!pool.PrepareStatements())
        {
            sLog->outError(LOG_FILTER_SQL, "Could not prepare statements of the %s database, see log for details.", name.c_str());
            return false;
        }
        return true;
    });

    return *this;
}

bool DatabaseLoader::Load()
{
    if (!_updateFlags)
        sLog->outError(LOG_FILTER_SQL, "Automatic database updates are disabled for all databases!");

    if (!OpenDatabases())
        return false;

    if (!PopulateDatabases())
        return false;

    if (!UpdateDatabases())
        return false;

    if (g_DatabaseUpdaterOnly)
    {
        sLog->outInfo(LOG_FILTER_SQL, "Automatic database updates are all applied with success! Exit ...");
        exit(0);
    }

    if (!PrepareStatements())
        return false;

    return true;
}

bool DatabaseLoader::OpenDatabases()
{
    return Process(_open);
}

bool DatabaseLoader::PopulateDatabases()
{
    return Process(_populate);
}

bool DatabaseLoader::UpdateDatabases()
{
    return Process(_update);
}

bool DatabaseLoader::PrepareStatements()
{
    return Process(_prepare);
}

bool DatabaseLoader::Process(std::queue<Predicate>& queue)
{
    while (!queue.empty())
    {
        if (!queue.front()())
        {
            // Close all open databases which have a registered close operation
            while (!_close.empty())
            {
                _close.top()();
                _close.pop();
            }

            return false;
        }

        queue.pop();
    }
    return true;
}

template
DatabaseLoader& DatabaseLoader::AddDatabase<LoginDatabaseConnection>(LoginDatabaseWorkerPool&, std::string const&);
template
DatabaseLoader& DatabaseLoader::AddDatabase<CharacterDatabaseConnection>(CharacterDatabaseWorkerPool&, std::string const&);
template
DatabaseLoader& DatabaseLoader::AddDatabase<HotfixDatabaseConnection>(HotfixDatabaseWorkerPool&, std::string const&);
template
DatabaseLoader& DatabaseLoader::AddDatabase<WorldDatabaseConnection>(WorldDatabaseWorkerPool&, std::string const&);
template
DatabaseLoader& DatabaseLoader::AddDatabase<WebDatabaseConnection>(WebDatabaseWorkerPool&, std::string const&);
template
DatabaseLoader& DatabaseLoader::AddDatabase<LogDatabaseConnection>(LogDatabaseWorkerPool&, std::string const&);
