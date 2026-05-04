/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "BisListMgr.h"

#include "DatabaseEnv.h"
#include "Field.h"
#include "Log.h"
#include "QueryResult.h"

void BisListMgr::LoadBisList()
{
    _bis.clear();

    QueryResult result = PlayerbotsDatabase.Query("SELECT class, tab, slot, item FROM playerbots_bis_gear");
    if (!result)
    {
        LOG_INFO("server.loading", "BiS list table is empty or missing");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint8 cls = fields[0].Get<uint8>();
        uint8 tab = fields[1].Get<uint8>();
        uint8 slot = fields[2].Get<uint8>();
        uint32 item = fields[3].Get<uint32>();

        _bis[MakeKey(cls, tab)][slot] = item;
        ++count;
    } while (result->NextRow());

    LOG_INFO("server.loading", "Loaded {} BiS gear entries across {} class/spec combos",
             count, static_cast<uint32>(_bis.size()));
}

std::map<uint8, uint32> BisListMgr::GetBisFor(uint8 cls, uint8 tab) const
{
    auto it = _bis.find(MakeKey(cls, tab));
    if (it == _bis.end())
        return {};
    return it->second;
}
