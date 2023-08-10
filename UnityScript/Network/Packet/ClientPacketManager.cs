using ServerCore;
using System;
using System.Collections.Generic;
using Protocol;
using Google.Protobuf;

public enum PacketID
{
    S_Login = 1,

    C_SignUp = 2,
    S_ThrowSignUpError = 3,

    C_EnterRoom = 4,
    S_StartGame = 5,
    S_LeaveGame = 6,

    C_PlayStone = 7,
    S_PlayStone = 8,

    S_CheckStone = 9,
    S_ThrowResult = 10,
}

public class Packet
{
    public ushort Protocol { get; set; }
    public IMessage Pkt { get; set; }
}

public class PacketManager
{
    #region SingleTon
    static PacketManager _instance = new PacketManager();
    public static PacketManager Instance { get { return _instance; } }
    #endregion

    PacketManager()
    {
        Register();
    }

    Dictionary<ushort, Func<PacketSession, ArraySegment<byte>, Packet>> _makeFunc = new Dictionary<ushort, Func<PacketSession, ArraySegment<byte>, Packet>>();
    Dictionary<ushort, Action<PacketSession, Packet>> _handler = new Dictionary<ushort, Action<PacketSession, Packet>>();

    public void Register()
    {
        _makeFunc.Add((ushort)PacketID.S_Login, MakePacket<S_Login>);
        _handler.Add((ushort)PacketID.S_Login, PacketHandler.Handle_S_Login);
        _makeFunc.Add((ushort)PacketID.S_ThrowSignUpError, MakePacket<S_ThrowSignUpError>);
        _handler.Add((ushort)PacketID.S_ThrowSignUpError, PacketHandler.Handle_S_ThrowSignUpError);
        _makeFunc.Add((ushort)PacketID.S_StartGame, MakePacket<S_StartGame>);
        _handler.Add((ushort)PacketID.S_StartGame, PacketHandler.Handle_S_StartGame);
        _makeFunc.Add((ushort)PacketID.S_LeaveGame, MakePacket<S_LeaveGame>);
        _handler.Add((ushort)PacketID.S_LeaveGame, PacketHandler.Handle_S_LeaveGame);
        _makeFunc.Add((ushort)PacketID.S_CheckStone, MakePacket<S_CheckStone>);
        _handler.Add((ushort)PacketID.S_CheckStone, PacketHandler.Handle_S_CheckStone);
        _makeFunc.Add((ushort)PacketID.S_PlayStone, MakePacket<S_PlayStone>);
        _handler.Add((ushort)PacketID.S_PlayStone, PacketHandler.Handle_S_PlayStone);
        _makeFunc.Add((ushort)PacketID.S_ThrowResult, MakePacket<S_ThrowResult>);
        _handler.Add((ushort)PacketID.S_ThrowResult, PacketHandler.Handle_S_ThrowResult);
    }

    #region PacketProcess
    public void OnRecvPacket(PacketSession session, ArraySegment<byte> buffer, Action<PacketSession, Packet> onRecvCallback = null)
    {
        ushort count = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
        count += 2;
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
        count += 2;

        Func<PacketSession, ArraySegment<byte>, Packet> func = null;
        if (_makeFunc.TryGetValue(id, out func))
        {
            Packet packet = func.Invoke(session, buffer);
            if (onRecvCallback != null)
                onRecvCallback.Invoke(session, packet);
            else
                HandlePacket(session, packet);
        }
    }

    Packet MakePacket<T>(PacketSession session, ArraySegment<byte> buffer) where T : IMessage<T>, new()
    {
        ushort count = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
        count += 2;
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + count);
        count += 2;

        MessageParser<T> parser = new MessageParser<T>(() => { return new T(); });
        T pkt = parser.ParseFrom(buffer.Array, count, size - count);

        Packet packet = new Packet();
        packet.Protocol = id;
        packet.Pkt = pkt;
        return packet;
    }

    public void HandlePacket(PacketSession session, Packet packet)
    {
        Action<PacketSession, Packet> action = null;
        if (_handler.TryGetValue(packet.Protocol, out action))
            action.Invoke(session, packet);
    }

    public ArraySegment<byte> MakeMessage(IMessage pkt, ushort size, ushort id)
    {
        const ushort HEADER_SIZE = 4;
        ArraySegment<byte> segment = SendBufferHelper.Open(4096);

        ushort count = 0;
        bool success = true;

        Span<byte> s = new Span<byte>(segment.Array, segment.Offset, segment.Count);
        success &= BitConverter.TryWriteBytes(s, size + HEADER_SIZE);
        count += sizeof(ushort);
        success &= BitConverter.TryWriteBytes(s.Slice(count, s.Length - count), id);
        count += sizeof(ushort);

        byte[] buffer = pkt.ToByteArray();
        buffer.CopyTo(segment.Array, segment.Offset + count);

        if (success == false)
            return null;
        return SendBufferHelper.Close(size + HEADER_SIZE);
    }
    #endregion
}