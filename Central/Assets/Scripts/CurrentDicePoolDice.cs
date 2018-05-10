using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CurrentDicePoolDice
    : MonoBehaviour
{
    public Text nameText;
    public Text voltageText;
    public Text statusText;
    public Image diceImage;
    public Button diceButton;
    public CanvasGroup commandGroup;
    public Button replaceButton;
    public Button forgetButton;
    public Button pingButon;

    public Die die { get; private set; }
    public Central central;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    public void Setup(Die die, Central central)
    {
        this.die = die;
        this.central = central;
        nameText.text = die.name;
        voltageText.text = "<voltage>";
        statusText.text = die.connected ? "Ready" : "Missing";
        diceImage.color = Color.white;
        HideCommands();

        diceButton.onClick.RemoveAllListeners();
        diceButton.onClick.AddListener(ShowCommands);
    }

    void ShowCommands()
    {
        commandGroup.interactable = true;
        commandGroup.blocksRaycasts = true;
        commandGroup.alpha = 1.0f;

        // Register commands
        forgetButton.onClick.RemoveAllListeners();
        forgetButton.onClick.AddListener(ForgetDie);
    }

    void HideCommands()
    {
        commandGroup.interactable = false;
        commandGroup.blocksRaycasts = false;
        commandGroup.alpha = 0.0f;
    }

    void ForgetDie()
    {
        HideCommands();

        // Tell central to forget about this die
        central.ForgetDie(die, null);
    }
}
