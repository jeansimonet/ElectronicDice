using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DiceAnimProgrammer
    : MonoBehaviour
    , IClient
{
    public Central central;
    public TimelineView timeline;

    HashSet<Die> dice;

    void Awake()
    {
        dice = new HashSet<Die>();
    }

    // Use this for initialization
    IEnumerator Start()
    {
        // Until we can properly record data, disable
        yield return new WaitUntil(() => central.state == CentralState.Idle);

        // Register to be notified of new dice getting connected
        central.RegisterClient(this);
    }

    public void OnNewDie(Die die)
    {
        //// Create the UI for the die
        //// Make sure to turn on the list!
        //noDiceIndicator.SetActive(false);
        //diceRoot.SetActive(true);

        //var template = diceRoot.transform.GetChild(0).gameObject;
        //var go = template;
        //if (trackedDice.Count > 0)
        //{
        //    // Copy first item rather than use it
        //    go = GameObject.Instantiate(template, diceRoot.transform);
        //}

        //var cmp = go.GetComponent<TelemetryDemoDie>();
        //cmp.Setup(die);

        dice.Add(die);
    }


    // Update is called once per frame
    void Update () {
		
	}

    public void UploadCurrentAnim()
    {
        // Try to send the anim down!
        Animations.AnimationSet animSet = new Animations.AnimationSet();
        animSet.animations = new Animations.RGBAnimation[1];
        animSet.animations[0] = timeline.Serialize();

        foreach (var die in dice)
        {
            die.UploadAnimationSet(animSet);
        }

    }

    public void PlayAnim()
    {
        foreach (var die in dice)
        {
            die.PlayAnimation(0);
        }
    }
}
