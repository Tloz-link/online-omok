using Google.Protobuf;
using System.Collections;
using System.Collections.Generic;

public class PacketQueue
{
    public static PacketQueue Instance { get; } = new PacketQueue();

    Queue<Packet> _packetQueue = new Queue<Packet>();
    object _lock = new object();

    public void Push(Packet packet)
    {
        lock (_lock)
        {
            _packetQueue.Enqueue(packet);
        }
    }

    public Packet Pop()
    {
        lock (_lock)
        {
            if (_packetQueue.Count == 0)
                return null;

            return _packetQueue.Dequeue();
        }
    }

    public List<Packet> PopAll()
    {
        List<Packet> list = new List<Packet>();

        lock (_lock)
        {
            while (_packetQueue.Count >0)
                list.Add(_packetQueue.Dequeue());
        }

        return list;
    }
}
