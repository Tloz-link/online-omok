using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace ServerCore
{
    public class RecvBuffer
    {
        ArraySegment<byte> _buffer;
        int _readPos;    //컨텐츠로 읽어야 하는 offset을 가리킴
        int _writePos;   //패킷을 수신해야 하는 offset을 가리킴

        public int DataSize { get { return _writePos - _readPos; } }  //유효한 데이터의 크기
        public int FreeSize { get { return _buffer.Count - _writePos; } }  //남은 공간의 크기

        public ArraySegment<byte> ReadSegment  // 현재 읽을 수 있는 바이트 배열
        {
            get { return new ArraySegment<byte>(_buffer.Array, _buffer.Offset + _readPos, DataSize); }
        }

        public ArraySegment<byte> WriteSegment  // 다음에 수신할 수 있는 바이트 배열
        {
            get { return new ArraySegment<byte>(_buffer.Array, _buffer.Offset + _writePos, FreeSize); }
        }

        public RecvBuffer(int bufferSize)
        {
            _buffer = new ArraySegment<byte>(new byte[bufferSize], 0, bufferSize);
        }

        public void Clean()
        {
            int dataSize = DataSize;
            if (dataSize == 0)
            {
                // 남은 데이터가 없으면 복사하지 않고 커서 위치만 리셋
                _readPos = 0;
                _writePos = 0;
            }
            else
            {
                // 남은 찌끄레기가 있으면 시작 위치로 복사
                Array.Copy(_buffer.Array, _buffer.Offset + _readPos, _buffer.Array, _buffer.Offset, dataSize);
                _readPos = 0;
                _writePos = dataSize;
            }
        }

        public bool OnRead(int numOfBytes)
        {
            if (numOfBytes > DataSize)
                return false;

            _readPos += numOfBytes;
            return true;
        }

        public bool OnWrite(int numOfBytes)
        {
            if (numOfBytes > FreeSize || numOfBytes < 0)
                return false;

            _writePos += numOfBytes;
            return true;
        }
    }
}
