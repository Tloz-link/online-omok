using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Protocol;

public class GameManagerEX
{
    public PlayerInfo MyPlayer { get; private set; }

    public void Init(PlayerInfo myPlayer)
    {
        MyPlayer = myPlayer;
    }

}
