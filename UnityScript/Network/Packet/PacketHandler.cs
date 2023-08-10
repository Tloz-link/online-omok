using DummyClient;
using Google.Protobuf;
using Protocol;
using ServerCore;
using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;

class PacketHandler
{
    public static void Handle_S_Login(PacketSession session, Packet packet)
    {
        S_Login pkt = packet.Pkt as S_Login;
        ServerSession serverSession = session as ServerSession;

        if (pkt.Success == false)
        {
            switch (pkt.Error)
            {
                case LOG_IN_ERROR.AlreadyConnected:
                    Application.Quit();
                    break;
                case LOG_IN_ERROR.NoAccount:
                    Managers.UI.ShowPopupUI<UI_SignUp>();
                    break;
                default:
                    break;
            }
            return;
        }

        Managers.Game.Init(pkt.PlayerInfo);
        Managers.UI.ShowPopupUI<UI_Game>();
        Managers.Network.Send(PacketHandler.Make_C_EnterRoom(pkt.PlayerInfo.PlayerId));
    }

    public static void Handle_S_ThrowSignUpError(PacketSession session, Packet packet)
    {
        S_ThrowSignUpError pkt = packet.Pkt as S_ThrowSignUpError;
        ServerSession serverSession = session as ServerSession;

        switch (pkt.Error)
        {
            case SIGN_UP_ERROR.AlreadySignUp:
                Application.Quit();
                break;
            case SIGN_UP_ERROR.WrongCount:
                break;
            default:
                break;
        }
    }

    public static void Handle_S_StartGame(PacketSession session, Packet packet)
    {
        S_StartGame pkt = packet.Pkt as S_StartGame;
        ServerSession serverSession = session as ServerSession;

        UI_Game game = Managers.UI.FindPopup<UI_Game>();
        game.StartGame(pkt.EnemyInfo);
    }

    public static void Handle_S_LeaveGame(PacketSession session, Packet packet)
    {
        S_LeaveGame pkt = packet.Pkt as S_LeaveGame;
        ServerSession serverSession = session as ServerSession;

        UI_Game game = Managers.UI.FindPopup<UI_Game>();
        game.Clear();
    }

    public static void Handle_S_CheckStone(PacketSession session, Packet packet)
    {
        S_CheckStone pkt = packet.Pkt as S_CheckStone;
        ServerSession serverSession = session as ServerSession;

        UI_Game game = Managers.UI.FindPopup<UI_Game>();
        game.CheckStone(pkt.Enable);
        Debug.Log("check " + pkt.Enable);
    }

    public static void Handle_S_PlayStone(PacketSession session, Packet packet)
    {
        S_PlayStone pkt = packet.Pkt as S_PlayStone;
        ServerSession serverSession = session as ServerSession;
        Debug.Log("Y : " + pkt.StoneInfo.PosY + " X : " + pkt.StoneInfo.PosX);

        UI_Game game = Managers.UI.FindPopup<UI_Game>();
        game.DoNextTurn(pkt.StoneInfo);
    }
    
    public static void Handle_S_ThrowResult(PacketSession session, Packet packet)
    {
        S_ThrowResult pkt = packet.Pkt as S_ThrowResult;
        ServerSession serverSession = session as ServerSession;

        UI_Game game = Managers.UI.FindPopup<UI_Game>();
        game.EndGame(pkt.StoneInfo);
    }


    public static ArraySegment<byte> Make_C_SignUp(string name)
    {
        C_SignUp pkt = new C_SignUp();
        pkt.Name = name;

        return PacketManager.Instance.MakeMessage(pkt, (ushort)pkt.CalculateSize(), (ushort)PacketID.C_SignUp);
    }

    public static ArraySegment<byte> Make_C_EnterRoom(ulong id)
    {
        C_EnterRoom pkt = new C_EnterRoom();
        pkt.AccountId = id;

        return PacketManager.Instance.MakeMessage(pkt, (ushort)pkt.CalculateSize(), (ushort)PacketID.C_EnterRoom);
    }

    public static ArraySegment<byte> Make_C_PlayStone(StoneInfo stoneInfo)
    {
        C_PlayStone pkt = new C_PlayStone();
        pkt.StoneInfo = stoneInfo;

        return PacketManager.Instance.MakeMessage(pkt, (ushort)pkt.CalculateSize(), (ushort)PacketID.C_PlayStone);
    }
}