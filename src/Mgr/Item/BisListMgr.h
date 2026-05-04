/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_BISLISTMGR_H
#define _PLAYERBOT_BISLISTMGR_H

#include "Define.h"

#include <map>

class BisListMgr
{
public:
    static BisListMgr* instance()
    {
        static BisListMgr inst;
        return &inst;
    }

    void LoadBisList();
    std::map<uint8, uint32> GetBisFor(uint8 cls, uint8 tab) const;

private:
    BisListMgr() = default;

    static uint16 MakeKey(uint8 cls, uint8 tab) { return (uint16(cls) << 8) | tab; }

    std::map<uint16, std::map<uint8, uint32>> _bis;
};

#define sBisListMgr BisListMgr::instance()

#endif
