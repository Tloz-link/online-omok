using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Protocol;

public class UI_Game : UI_Popup
{
    enum State
    {
        None,
        MyTurn,
        Wait
    }
    private State _state;
    private Sprite _whiteStone;
    private Sprite _blackStone;

    // GameStart
    List<UI_Stone> _stones;
    PlayerInfo _myPlayer;

    enum GameObjects
    {
        Grid,
        Winner,
        Loser,
        Forbidden,
    }

    enum Texts
    {
        TurnText,
        PlayerNameText,
        PlayerColorText,
        EnemyNameText,
        EnemyColorText
    }

    public override bool Init()
    {
        if (base.Init() == false)
            return false;

        BindObject(typeof(GameObjects));
        BindText(typeof(Texts));

        _whiteStone = Managers.Resource.Load<Sprite>(Define.WHITE_STONE_PATH);
        _blackStone = Managers.Resource.Load<Sprite>(Define.BLACK_STONE_PATH);
        _stones = new List<UI_Stone>(Define.MAX_LINE * Define.MAX_LINE);

        Clear();

        return true;
    }

    public void StartGame(EnemyInfo enemyInfo)
    {
        for (uint y = 0; y < Define.MAX_LINE; ++y)
        {
            for (uint x = 0; x < Define.MAX_LINE; ++x)
            {
                UI_Stone stone = Managers.UI.makeSubItem<UI_Stone>(GetObject((int)GameObjects.Grid).transform);

                StoneInfo stoneInfo = new StoneInfo();
                stoneInfo.StoneType = STONE_TYPE.Blank;
                stoneInfo.PosY = y;
                stoneInfo.PosX = x;

                stone.SetInfo(stoneInfo, PlayStoneCallback);
                _stones.Add(stone);
            }
        }

        _myPlayer = Managers.Game.MyPlayer;
        _myPlayer.PlayerType = (enemyInfo.PlayerType == PLAYER_TYPE.Black) ? PLAYER_TYPE.White : PLAYER_TYPE.Black;
        GetText((int)Texts.PlayerColorText).text = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? "검정" : "하양";

        GetText((int)Texts.EnemyNameText).text = enemyInfo.Name;
        GetText((int)Texts.EnemyColorText).text = (enemyInfo.PlayerType == PLAYER_TYPE.Black) ? "검정" : "하양";

        GetText((int)Texts.TurnText).text = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? "내턴" : "상대턴";

        _state = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? State.MyTurn : State.Wait;
    }

    public void Clear()
    {
        foreach (UI_Stone stone in _stones)
        {
            Managers.Resource.Destroy(stone.gameObject);
        }
        _stones.Clear();

        GetText((int)Texts.PlayerNameText).text = Managers.Game.MyPlayer.Name;
        GetText((int)Texts.PlayerColorText).text = "";
        GetText((int)Texts.EnemyNameText).text = "";
        GetText((int)Texts.EnemyColorText).text = "";
        GetText((int)Texts.TurnText).text = "";

        GetObject((int)GameObjects.Winner).SetActive(false);
        GetObject((int)GameObjects.Loser).SetActive(false);
        GetObject((int)GameObjects.Forbidden).SetActive(false);

        _state = State.None;
    }

    public void CheckStone(bool enable)
    {
        if (enable == false)
        {
            StartCoroutine(CoPrintForbiddenPlay());
        }
    }

    IEnumerator CoPrintForbiddenPlay()
    {
        _state = State.Wait;
        GetObject((int)GameObjects.Forbidden).SetActive(true);

        yield return new WaitForSeconds(1.0f);

        _state = State.MyTurn;
        GetObject((int)GameObjects.Forbidden).SetActive(false);
    }

    public void DoNextTurn(StoneInfo stoneInfo)
    {
        UI_Stone stone = _stones[(int)(stoneInfo.PosY * Define.MAX_LINE + stoneInfo.PosX)];
        if (stoneInfo.StoneType == STONE_TYPE.Black)
        {
            stone.SetSprite(_blackStone);
            _state = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? State.Wait : State.MyTurn;
        }
        else if (stoneInfo.StoneType == STONE_TYPE.White)
        {
            stone.SetSprite(_whiteStone);
            _state = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? State.MyTurn : State.Wait;
        }
        else
        {
            Debug.Log("오류");
            return;
        }

        GetText((int)Texts.TurnText).text = (_state == State.MyTurn) ? "내턴" : "상대턴";
    }

    public void EndGame(StoneInfo stoneInfo)
    {
        _state = State.None;

        UI_Stone stone = _stones[(int)(stoneInfo.PosY * Define.MAX_LINE + stoneInfo.PosX)];
        GameObjects flag = GameObjects.Winner;

        if (stoneInfo.StoneType == STONE_TYPE.Black)
        {
            stone.SetSprite(_blackStone);
            flag = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? GameObjects.Winner : GameObjects.Loser;
        }
        else if (stoneInfo.StoneType == STONE_TYPE.White)
        {
            stone.SetSprite(_whiteStone);
            flag = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? GameObjects.Loser : GameObjects.Winner;
        }
        else
        {
            Debug.Log("오류");
            return;
        }

        GetObject((int)flag).SetActive(true);
    }

    void PlayStoneCallback(StoneInfo stoneInfo)
    {
        if (_state != State.MyTurn || stoneInfo.StoneType != STONE_TYPE.Blank)
            return;

        _state = State.Wait;

        StoneInfo newStone = new StoneInfo(stoneInfo);
        newStone.StoneType = (_myPlayer.PlayerType == PLAYER_TYPE.Black) ? STONE_TYPE.Black : STONE_TYPE.White;

        Managers.Network.Send(PacketHandler.Make_C_PlayStone(newStone));
    }
}
