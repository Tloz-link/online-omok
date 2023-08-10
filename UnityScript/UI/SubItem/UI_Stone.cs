using Protocol;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_Stone : UI_Base
{
    enum Images
    {
        Stone
    }

    public StoneInfo StoneInfo { get; private set; }

    private Action<StoneInfo> _playStoneCallback;

    public override bool Init()
    {
        if (base.Init() == false)
            return false;

        BindImage(typeof(Images));
        GetImage((int)Images.Stone).gameObject.BindEvent(PlayStone);
        return true;
    }

    public void SetInfo(StoneInfo stoneInfo, Action<StoneInfo> playStoneCallback)
    {
        Init();

        StoneInfo = stoneInfo;
        _playStoneCallback = playStoneCallback;
    }

    public void SetSprite(Sprite sprite)
    {
        GetImage((int)Images.Stone).sprite = sprite;
        Color color = GetImage((int)Images.Stone).color;
        color.a = 1f;
        GetImage((int)Images.Stone).color = color;
    }

    void PlayStone(PointerEventData evt)
    {
        if (StoneInfo.StoneType != STONE_TYPE.Blank)
        {
            return;
        }

        _playStoneCallback.Invoke(StoneInfo);
    }
}
