using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Define
{

    //public const string SERVER_IP = "3.35.166.253";
    public const string SERVER_IP = "127.0.0.1";
    public const int MAX_LINE = 19;

    public const string BLACK_STONE_PATH = "Sprites/BlackStone";
    public const string WHITE_STONE_PATH = "Sprites/WhiteStone";

    public const float MAX_VOLUME = 0.5f;
    public const int MAX_VOLUME_COUNT = 6;
    public const int MAX_SOUND_OVERLAPPED = 2;


    public enum Scene
    {
        Unknown,
        GameScene,
    }

    public enum Sound
    {
        Bgm,
        Effect,
        MaxCount
    }

    public enum UIEvent
    {
        Click,
        Pressed,
        Drag,
        PressedLong,
        PointerDown,
        PointerUp,
        PointerExit
    }
}
