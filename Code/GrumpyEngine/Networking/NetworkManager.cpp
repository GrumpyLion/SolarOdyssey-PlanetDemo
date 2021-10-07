#include "pch.h"

#if GE_ENABLE_NETWORKING
#include "Types.h"

namespace NetworkManager
{
    bool ourIsServer = false;
    bool ourIsClient = false;
    int ourMaxPlayers = ourTotalUserCount - 1;
    int ourPort = 25565;

    std::string ourUserName = "Default";

    ENetAddress locServerAddress;
    ENetHost* locServer = nullptr;

    int locClientID = -1;
    ENetAddress locClientAddress;
    ENetHost* locClient = nullptr;
    ENetPeer* locPeer = nullptr;

    ENetPeer* locClients[ourTotalUserCount];
    Entity ourOtherPlayers[ourTotalUserCount];

    std::string ourIP = "localhost";

    enum PacketType
    {
        PlayerInputData,
        PlayerLeft
    };

    class NetPacketReader
    {
        std::vector<char> myData;
        size_t myPos;

    public:
        NetPacketReader(ENetPacket* packet)
        {
            myData.resize(packet->dataLength);
            memcpy(myData.data(), packet->data, packet->dataLength);
            enet_packet_destroy(packet);
            myPos = 1;
        }

        NetPacketReader(const std::vector<char> inData)
        {
            myData = inData;
            myPos = 1;
        }

        PacketType GetType() { return (PacketType)myData[0]; }

        uint32_t ReadUInt()
        {
            uint32_t value = 0;
            if (myPos + 4 > myData.size())
            {
                geWarn("Received packet truncated");
            }

            value |= ((uint32_t)(uint8_t)myData[myPos++]);
            value |= ((uint32_t)(uint8_t)myData[myPos++]) << 8;
            value |= ((uint32_t)(uint8_t)myData[myPos++]) << 16;
            value |= ((uint32_t)(uint8_t)myData[myPos++]) << 24;
            return value;
        }

        int32_t ReadInt()
        {
            int32_t value = 0;
            if (myPos + 4 > myData.size())
            {
                geWarn("Received packet truncated");
            }

            value |= ((int32_t)(uint8_t)myData[myPos++]);
            value |= ((int32_t)(uint8_t)myData[myPos++]) << 8;
            value |= ((int32_t)(uint8_t)myData[myPos++]) << 16;
            value |= ((int32_t)(uint8_t)myData[myPos++]) << 24;
            return value;
        }

        int16_t ReadShort()
        {
            int32_t value = 0;
            if (myPos + 2 > myData.size())
            {
                geWarn("Received packet truncated");
            }

            value |= ((int32_t)(uint8_t)myData[myPos++]);
            value |= ((int32_t)(uint8_t)myData[myPos++]) << 8;
            return (int16_t)value;
        }

        uint16_t ReadUShort()
        {
            uint32_t value = 0;
            if (myPos + 2 > myData.size())
            {
                geWarn("Received packet truncated");
            }

            value |= ((uint32_t)(uint8_t)myData[myPos++]);
            value |= ((uint32_t)(uint8_t)myData[myPos++]) << 8;
            return (uint16_t)value;
        }

        uint8_t ReadByte()
        {
            if (myPos >= myData.size())
            {
                geWarn("Received packet truncated");
            }

            return (uint8_t)myData[myPos++];
        }

        bool ReadBool()
        {
            if (myPos >= myData.size())
            {
                geWarn("Received packet truncated");
            }

            return (bool)myData[myPos++];
        }

        float ReadFloat()
        {
            union {
                float f;
                uint32_t v;
            };

            v = ReadUInt();
            return f;
        }

        Vec2 ReadVec2()
        {
            Vec2 vec;
            vec.x = ReadFloat();
            vec.y = ReadFloat();
            return vec;
        }

        Vec3 ReadVec3()
        {
            Vec3 vec;
            vec.x = ReadFloat();
            vec.y = ReadFloat();
            vec.z = ReadFloat();
            return vec;
        }

        Vec4 ReadVec4()
        {
            Vec4 vec;
            vec.x = ReadFloat();
            vec.y = ReadFloat();
            vec.z = ReadFloat();
            vec.w = ReadFloat();
            return vec;
        }

        std::size_t GetNumRemainingBytes() { return myData.size() - myPos; }

        std::vector<char> GetData() { return myData; }

        std::string ReadData(size_t size)
        {
            if (myPos + size > myData.size())
            {
                geWarn("Received packet truncated");
            }

            std::string s = std::string(myData.data() + myPos, size);
            myPos += size;
            return s;
        }

        std::string ReadRemainingData()
        {
            return std::string(myData.data() + myPos, myData.size() - myPos);
        }

        std::string ReadString(size_t siz)
        {
            // convert to C string once so that
            // null-chars are removed
            std::string s = ReadData(siz).c_str();
            return s;
        }

        std::string ReadRemainingString()
        {
            // convert to C string once so that
            // null-chars are removed
            std::string s = ReadRemainingData().c_str();
            return s;
        }

        void DumpDebug()
        {
            char buf[1024];
            std::string str;
            sprintf(buf, "Packet 0x%02x [len=%d]", (int)GetType(), (int)myData.size());
            str = buf;
            int bytes = (int)myData.size();
            if (bytes > 64)
            {
                bytes = 64;
            }
            for (int i = 0; i < bytes; i++)
            {
                sprintf(buf, " %02x", (unsigned int)(unsigned char)myData[i]);
                str += buf;
            }

            geInfo("%s", str.c_str());
        }
    };

    class NetPacketWriter
    {
        std::vector<char> myData;

    public:
        NetPacketWriter(PacketType type) { myData.push_back(type); }

        void Write(uint8 v)
        {
            myData.push_back(v);
        }

        void Write(bool v)
        {
            myData.push_back((char)v);
        }

        void Write(uint16 v)
        {
            myData.push_back((char)(v));
            myData.push_back((char)(v >> 8));
        }

        void Write(uint v)
        {
            myData.push_back((char)(v));
            myData.push_back((char)(v >> 8));
            myData.push_back((char)(v >> 16));
            myData.push_back((char)(v >> 24));
        }

        void Write(int v)
        {
            myData.push_back((char)(v));
            myData.push_back((char)(v >> 8));
            myData.push_back((char)(v >> 16));
            myData.push_back((char)(v >> 24));
        }

        void Write(float v)
        {
            union
            {
                float f;
                uint32_t i;
            };

            f = v;
            Write(i);
        }

        void Write(Vec2 v)
        {
            Write(v.x);
            Write(v.y);
        }

        void Write(Vec3 v)
        {
            Write(v.x);
            Write(v.y);
            Write(v.z);
        }

        void Write(Vec4 v)
        {
            Write(v.x);
            Write(v.y);
            Write(v.z);
            Write(v.w);
        }

        void Write(const std::string& str)
        {
            myData.insert(myData.end(), str.begin(), str.end());
        }

        void Write(const std::string& str, size_t fillLen)
        {
            Write(str.substr(0, fillLen));
            size_t sz = str.size();
            while (sz < fillLen)
            {
                Write((uint8_t)0);
                sz++;
            }
        }

        std::size_t GetPosition() { return myData.size(); }

        void Update(std::size_t position, std::uint8_t newValue)
        {
            if (position >= myData.size())
            {
                GE_ERROR("Invalid write (%d should be less than %d)", (int)position,
                    (int)myData.size());
            }

            myData[position] = static_cast<char>(newValue);
        }

        void Update(std::size_t position, std::uint32_t newValue)
        {
            if (position + 4 > myData.size())
            {
                GE_ERROR("Invalid write (%d should be less than or equal to %d)",
                    (int)(position + 4), (int)myData.size());
            }

            // Assuming the target platform is little endian and supports
            // unaligned memory access...
            *reinterpret_cast<std::uint32_t*>(myData.data() + position) = newValue;
        }

        ENetPacket* CreatePacket(int flag = ENET_PACKET_FLAG_RELIABLE)
        {
            return enet_packet_create(myData.data(), myData.size(), flag);
        }
    };
}

void NetworkManager::Initialize()
{
}

void NetworkManager::Shutdown()
{
    Disconnect();
    StopServer();
}

void NetworkManager::Update()
{
    ENetEvent event;
    if (locServer != nullptr)
    {
        while (enet_host_service(locServer, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                geInfo("A new client connected from %x:%u. ID %d",
                    event.peer->address.host,
                    event.peer->address.port,
                    event.peer->incomingPeerID);
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                NetPacketReader reader(event.packet);
                //Server::ReadPacket(reader);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                geInfo("%s disconnected.", event.peer->data);

                //ourOtherPlayers[event.peer->incomingPeerID]->Destroy();
                //ourOtherPlayers[event.peer->incomingPeerID] = nullptr;

                NetPacketWriter writer(PacketType::PlayerLeft);
                writer.Write(event.peer->incomingPeerID);
                enet_host_broadcast(locServer, 0, writer.CreatePacket(ENET_PACKET_FLAG_RELIABLE));
                break;
            }
            }
        }
    }

    if (locClient != nullptr)
    {
        while (enet_host_service(locClient, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                NetPacketReader reader(event.packet);
                //Client::ReadPacket(reader);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                geInfo("%s disconnected.", event.peer->data);
                Disconnect();
                return;
            }
            }
        }
    }
}

bool NetworkManager::Connect()
{
    if (enet_initialize() != 0)
    {
        GE_ERROR("Enet init error");
        return false;
    }

    locClient = enet_host_create(nullptr /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        2 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* assume any amount of incoming bandwidth */,
        0 /* assume any amount of outgoing bandwidth */);

    if (locClient == nullptr)
    {
        GE_ERROR("Client creation error");
        return false;
    }

    if (enet_address_set_host(&locClientAddress, ourIP.c_str()) < 0)
    {
        GE_ERROR("IP conversion failed");
        return false;
    }

    locClientAddress.port = ourPort;
    locPeer = enet_host_connect(locClient, &locClientAddress, 2, 0);

    if (locPeer == nullptr)
    {
        GE_ERROR("No available peers for connection");
        return false;
    }

    ENetEvent event;
    if (enet_host_service(locClient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        geInfo("Connection to server %s succeded", ourIP.c_str());
        ourIsClient = true;
    }
    else
    {
        GE_ERROR("Connection to server %s failed", ourIP.c_str());
        enet_peer_reset(locPeer);
        ourIsClient = false;
        return false;
    }

    return true;
}

void NetworkManager::Disconnect()
{
    if (!ourIsClient)
        return;

    //for (int i = 0; i < ourTotalUserCount; ++i)
    //{
    //    if (ourOtherPlayers[i] != nullptr)
    //        ourOtherPlayers[i]->Destroy();

    //    ourOtherPlayers[i] = nullptr;
    //}

    enet_peer_disconnect(locPeer, 0);

    ENetEvent event;
    while (enet_host_service(locClient, &event, 1000) > 0) // disconnect gracefully
    {
        bool succeded = false;
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            geInfo("Disconnection succeeded");
            succeded = true;
            break;
        }

        if (succeded)
            break;
    }

    ourIsClient = false;
    enet_peer_reset(locPeer);
    enet_host_destroy(locClient);
    enet_deinitialize();
    locPeer = nullptr;
    locClient = nullptr;
}

void NetworkManager::StartServer()
{
    if (enet_initialize() != 0)
    {
        GE_ERROR("Enet init error");
        return;
    }

    locServerAddress.host = ENET_HOST_ANY;
    locServerAddress.port = ourPort;
    locServer = enet_host_create(&locServerAddress  /* the address to bind the server host to */,
        ourMaxPlayers                               /* allow up to 32 clients and/or outgoing connections */,
        2                                           /* allow up to 2 channels to be used, 0 and 1 */,
        0                                           /* assume any amount of incoming bandwidth */,
        0                                           /* assume any amount of outgoing bandwidth */);

    if (locServer == nullptr)
    {
        GE_ERROR("Server couldn't be started");
        return;
    }
    ourIsServer = true;
}

void NetworkManager::StopServer()
{
    if (!ourIsServer)
        return;

    //for (int i = 0; i < ourTotalUserCount; ++i)
    //{
    //    if (ourOtherPlayers[i] != nullptr)
    //        ourOtherPlayers[i]->Destroy();

    //    ourOtherPlayers[i] = nullptr;
    //}

    ourIsServer = false;
    enet_host_destroy(locServer);
    enet_deinitialize();
    locServer = nullptr;
}

void NetworkManager::LoadSettings(tinyxml2::XMLElement* element)
{
    tinyxml2::XMLElement* networkingElement = element->FirstChildElement("Networking");
    if (networkingElement == nullptr)
        return;

    ourMaxPlayers = networkingElement->IntAttribute("MaxPlayers", 8);
    ourMaxPlayers = glm::clamp(ourMaxPlayers, 0, ourTotalUserCount - 1);

    ourPort = networkingElement->IntAttribute("Port", 25565);
    ourUserName = networkingElement->Attribute("Username") == nullptr ? "Default" : networkingElement->Attribute("Username");
    ourIP = networkingElement->Attribute("ServerIP") == nullptr ? "localhost" : networkingElement->Attribute("ServerIP");
}

void NetworkManager::SaveSettings(tinyxml2::XMLPrinter& printer)
{
    printer.OpenElement("Networking");

    printer.PushAttribute("MaxPlayers", ourMaxPlayers);
    printer.PushAttribute("Port", ourPort);
    printer.PushAttribute("Username", ourUserName.c_str());
    printer.PushAttribute("ServerIP", ourIP.c_str());

    printer.CloseElement();
}

//////////////////////////////////////////////////////////////////////////

//void Win_NetworkManager::Client::ReadPacket(NetPacketReader& reader)
//{
//    switch (reader.GetType())
//    {
//    case PacketType::PlayerInputData:
//    {
//        Game_PlayerNode::PlayerData input;
//        input.myPeerID = reader.ReadUShort();
//        input.myPosition = reader.ReadVec3();
//        input.myRotation = reader.ReadVec3();
//
//        input.myHealth = reader.ReadFloat();
//        input.myAmmo = reader.ReadInt();
//        input.myPoints = reader.ReadInt();
//        input.myShooting = reader.ReadBool();
//
//        input.myUsername = reader.ReadRemainingString();
//
//        if (locPeer->outgoingPeerID == input.myPeerID)
//            return;
//
//        if (ourOtherPlayers[input.myPeerID] == nullptr)
//        {
//            ourOtherPlayers[input.myPeerID] = (Game_PlayerNode*)Scene::SpawnPrefab("PlayerMP", input.myUsername);
//            ourOtherPlayers[input.myPeerID]->Initialize();
//            ourOtherPlayers[input.myPeerID]->InitializeChildren();
//        }
//
//        ourOtherPlayers[input.myPeerID]->myIsReplica = true;
//        ourOtherPlayers[input.myPeerID]->myNewNetworkPosition = input.myPosition;
//        ourOtherPlayers[input.myPeerID]->myTransform.myRotation = input.myRotation;
//        ourOtherPlayers[input.myPeerID]->myHealth = input.myHealth;
//        ourOtherPlayers[input.myPeerID]->myPoints = input.myPoints;
//
//        if (ourOtherPlayers[input.myPeerID]->myWeapon != nullptr)
//        {
//            ourOtherPlayers[input.myPeerID]->myWeapon->myEnergy = input.myAmmo;
//
//            if (input.myShooting)
//                ourOtherPlayers[input.myPeerID]->myWeapon->Shoot();
//        }
//        return;
//    }
//    case PacketType::PlayerLeft:
//    {
//        uint16 id = reader.ReadUShort();
//        ourOtherPlayers[id]->Destroy();
//        ourOtherPlayers[id] = nullptr;
//        return;
//    }
//    }
//}
//
//void Win_NetworkManager::Client::SendPlayerData(const Game_PlayerNode::PlayerData& input)
//{
//    if (locClient == nullptr || locPeer == nullptr)
//        return;
//
//    NetPacketWriter writer(PacketType::PlayerInputData);
//    writer.Write(locPeer->outgoingPeerID);
//    writer.Write(input.myPosition);
//    writer.Write(input.myRotation);
//    writer.Write(input.myHealth);
//    writer.Write(input.myAmmo);
//    writer.Write(input.myPoints);
//    writer.Write(input.myShooting);
//    writer.Write(input.myUsername);
//    enet_peer_send(locPeer, 0, writer.CreatePacket(ENET_PACKET_FLAG_RELIABLE));
//}
//
////////////////////////////////////////////////////////////////////////////
//
//void Win_NetworkManager::Server::ReadPacket(NetPacketReader& reader)
//{
//    switch (reader.GetType())
//    {
//    case PacketType::PlayerInputData:
//        Game_PlayerNode::PlayerData input;
//        input.myPeerID = reader.ReadUShort();
//        input.myPosition = reader.ReadVec3();
//        input.myRotation = reader.ReadVec3();
//
//        input.myHealth = reader.ReadFloat();
//        input.myAmmo = reader.ReadInt();
//        input.myPoints = reader.ReadInt();
//        input.myShooting = reader.ReadBool();
//
//        input.myUsername = reader.ReadRemainingString();
//
//        if (ourOtherPlayers[input.myPeerID] == nullptr)
//        {
//            ourOtherPlayers[input.myPeerID] = (Game_PlayerNode*)Scene::SpawnPrefab("PlayerMP", input.myUsername); // pointer gets trashed after deleting nodes fix this with handles
//            ourOtherPlayers[input.myPeerID]->Initialize();
//            ourOtherPlayers[input.myPeerID]->InitializeChildren();
//        }
//
//        ourOtherPlayers[input.myPeerID]->myIsReplica = true;
//        ourOtherPlayers[input.myPeerID]->myNewNetworkPosition = input.myPosition;
//        ourOtherPlayers[input.myPeerID]->myTransform.myRotation = input.myRotation;
//        ourOtherPlayers[input.myPeerID]->myHealth = input.myHealth;
//        ourOtherPlayers[input.myPeerID]->myPoints = input.myPoints;
//
//        if (ourOtherPlayers[input.myPeerID]->myWeapon != nullptr)
//        {
//            ourOtherPlayers[input.myPeerID]->myWeapon->myEnergy = input.myAmmo;
//
//            if (input.myShooting)
//                ourOtherPlayers[input.myPeerID]->myWeapon->Shoot();
//        }
//
//        NetPacketWriter writer(PacketType::PlayerInputData);
//        writer.Write(input.myPeerID);
//        writer.Write(input.myPosition);
//        writer.Write(input.myRotation);
//        writer.Write(input.myHealth);
//        writer.Write(input.myAmmo);
//        writer.Write(input.myPoints);
//        writer.Write(input.myShooting);
//        writer.Write(input.myUsername);
//        enet_host_broadcast(locServer, 0, writer.CreatePacket(ENET_PACKET_FLAG_RELIABLE));
//        return;
//    }
//}
//
//void Win_NetworkManager::Server::SendPlayerData(const Game_PlayerNode::PlayerData& input)
//{
//    if (locServer == nullptr)
//        return;
//
//    NetPacketWriter writer(PacketType::PlayerInputData);
//    writer.Write(static_cast<uint16>(ourTotalUserCount - 1));
//    writer.Write(input.myPosition);
//    writer.Write(input.myRotation);
//    writer.Write(input.myHealth);
//    writer.Write(input.myAmmo);
//    writer.Write(input.myPoints);
//    writer.Write(input.myShooting);
//    writer.Write(input.myUsername);
//    enet_host_broadcast(locServer, 0, writer.CreatePacket(ENET_PACKET_FLAG_RELIABLE));
//}

#endif // GE_PLATFORM_PC && GE_NETWORK_ENABLED