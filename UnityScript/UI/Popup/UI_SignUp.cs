using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class UI_SignUp : UI_Popup
{
    enum Texts
    {
        NickNameText
    }

    enum Buttons
    {
        SubmitButton
    }

    public override bool Init()
    {
        if (base.Init() == false)
            return false;

        BindText(typeof(Texts));
        BindButton(typeof(Buttons));

        GetButton((int)Buttons.SubmitButton).gameObject.BindEvent(SubmitName);
        return true;
    }

    void Update()
    {
        
    }

    void SubmitName(PointerEventData evt)
    {
        string name = GetText((int)Texts.NickNameText).text;
        Managers.Network.Send(PacketHandler.Make_C_SignUp(name));
        Managers.UI.ClosePopupUI(this);
    }
}
