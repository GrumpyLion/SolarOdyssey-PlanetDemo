#pragma once

// TODO this is pretty outdated and needs a rewrite..
//
#if GE_ENABLE_NETWORKING
#include <enet/enet.h>

namespace NetworkManager
{
    class NetPacketReader;

    void Initialize();
    void Shutdown();
    void Update();

    bool Connect();
    void Disconnect();

    void StartServer();
    void StopServer();

    void LoadSettings(tinyxml2::XMLElement* element);
    void SaveSettings(tinyxml2::XMLPrinter& printer);

    //namespace Client
    //{
    //    void ReadPacket(NetPacketReader& reader);

    //    void SendPlayerData(const Game_PlayerNode::PlayerData& input);
    //}

    //namespace Server
    //{
    //    void ReadPacket(NetPacketReader& reader);

    //    void SendPlayerData(const Game_PlayerNode::PlayerData& input);
    //}

    //////////////////////////////////////////////////////////////////////////

    extern bool ourIsServer;
    extern bool ourIsClient;

    extern int ourMaxPlayers;
    extern int ourPort;

    extern std::string ourUserName;
    extern std::string ourIP;
    const int ourTotalUserCount = 5;
    extern Entity ourOtherPlayers[ourTotalUserCount];
}

#endif // GE_ENABLE_NETWORKING