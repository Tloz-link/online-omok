using DummyClient;
using Google.Protobuf;
using ServerCore;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    ServerSession _session = new ServerSession();
    bool _init = false;

    public void Send(ArraySegment<byte> sendBuff)
    {
        _session.Send(sendBuff);
    }

    public void Init()
    {
        if (_init == true)
            return;
        _init = true;

        // DNS (Domain Name System)
        string host = Dns.GetHostName();
        IPHostEntry ipHost = Dns.GetHostEntry(host);
        IPAddress ipAddr = IPAddress.Parse(Define.SERVER_IP);
        IPEndPoint endPoint = new IPEndPoint(ipAddr, 7777);

        Connector connector = new Connector();

        connector.Connect(endPoint,
            () => { return _session; },
            1);
    }

    void Update()
    {
        if (_init == false)
            return;

        // 이렇게 하면 메인 쓰레드가 직접 Packet을 건드리게 할 수 있음
        List<Packet> list = PacketQueue.Instance.PopAll();
        foreach (Packet packet in list)
            PacketManager.Instance.HandlePacket(_session, packet);
    }
}
