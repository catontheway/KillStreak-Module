/**
    This plugin can be used for common player customizations
 */

#include "ScriptPCH.h"
#include "Config.h"
#include <unordered_map>

struct SystemInfo
{
    uint32 KillStreak = 0;
    uint32 LastGUIDKill = 0;
    uint32 LastKillTime = 0;
};

static std::unordered_map<uint32, SystemInfo> KillingStreak;

class MODKillStreak_Config : public WorldScript
{
public: MODKillStreak_Config() : WorldScript("MODKillStreak_Config") { };
    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/mod_killstreak.conf";
            std::string cfg_file_2 = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_file_2.c_str());
            sConfigMgr->LoadMore(cfg_file.c_str());

        }
    }
};

class MODKillStreak : public PlayerScript{
public:

    MODKillStreak() : PlayerScript("MODKillStreak") { }


    void OnPVPKill(Player *pKiller, Player *pVictim)
    {
        uint32 killerGUID;
        uint32 victimGUID;
        killerGUID = pKiller->GetGUID();
        victimGUID = pVictim->GetGUID();

        uint32 currentTime = sWorld->GetGameTime();
        if(killerGUID == victimGUID)
        {
            return;
        }
        if(KillingStreak[killerGUID].LastKillTime == 0){
            KillingStreak[killerGUID].LastKillTime = sWorld->GetGameTime();
        }
        else if  (sWorld->GetGameTime() - KillingStreak[killerGUID].LastKillTime >= 2 * 60) { // hold the kill streak only for 2 minutes
            KillingStreak[killerGUID].KillStreak = 0;
            KillingStreak[killerGUID].LastKillTime = sWorld->GetGameTime();
            KillingStreak[killerGUID].LastGUIDKill = 0;
        }
        if(KillingStreak[killerGUID].LastGUIDKill == victimGUID) {
            //return;
        }
        KillingStreak[killerGUID].KillStreak++;
        KillingStreak[victimGUID].KillStreak = 0;
        KillingStreak[killerGUID].LastGUIDKill = victimGUID;
        KillingStreak[victimGUID].LastGUIDKill = 0;
        KillingStreak[victimGUID].LastKillTime = 0;

        if (KillingStreak[killerGUID].KillStreak % 10 == 0)
            pKiller->AddItem(701003, KillingStreak[killerGUID].KillStreak);

        if(KillingStreak[killerGUID].KillStreak >= sConfigMgr->GetIntDefault("KillStreak.MinAmount", 10)) {
            char msg[1000];
            sprintf(msg, "|cffCC0000[PVP System]|r %s killed %s and is on a %u KILLSTREAK. ", pKiller->GetName().c_str(), pVictim->GetName().c_str(), KillingStreak[killerGUID].KillStreak);
            sWorld->SendGlobalText(msg, NULL);
        }
    }
};

void AddKillStreakScripts() {
    new MODKillStreak_Config();
    new MODKillStreak();
}

