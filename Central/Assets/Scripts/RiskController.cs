using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Linq;

public class RiskController : MonoBehaviour
{
	public Text consoleOutputUI;
	public Button scanButtonUI;
	public Text scanButtonTextUI;
	public GameObject attackerRootUI;
	public GameObject defenderRootUI;
	public UIDie diePrefab;
	public GameObject mainUI;
	public DiceControl diceControlUI;

	Central central;

	void Awake()
	{
		central = GetComponent<Central>();
	}

	// Use this for initialization
	IEnumerator Start()
	{
		// Initialize Central
		yield return central.InitializeCr();

		// Our running list of die
		var dice = new List<Die>();

		System.Func<Central.NameAndAddress, Die> addNewDie = (dna) => {

			// DEMO HACK!!!
			// Attach first 3 die to attacker and rest to defense
			var root = attackerRootUI.transform;
			if (dice.Count >= 3)
				root = defenderRootUI.transform;

			var dieUI = GameObject.Instantiate<UIDie>(diePrefab);
			dieUI.transform.SetParent(root);
			dieUI.transform.position = Vector3.zero;
			dieUI.transform.rotation = Quaternion.identity;
			dieUI.transform.localScale = Vector3.one;
			dieUI.diceName = dna.name;
			dieUI.faceNumber = -1;

			var ret = new Die(dna.name, dna.address, dieUI);
			dieUI.OnMoreOptions += () => ShowDieCommands(ret);
			dice.Add(ret);
			return ret;
		};

		// Trigger periodic scans
		bool resetBattle = false;
		bool triggerScan = true;
		//StartCoroutine(PingDice(dice, 0.1f));

		scanButtonUI.onClick.AddListener(() =>
		{
			resetBattle = true;
		});

		while (true)
		{
			if (resetBattle)
			{
				Debug.Log("Clearing");
				resetBattle = false;
				foreach (var d in dice)
				{
					d.Face = -1;
				}
			}

			if (triggerScan)
			{
				Debug.Log("Scanning...");
				triggerScan = false;
				var discoveredDice = new List<Central.NameAndAddress>();
				yield return central.ScanForDevicesCr((na) =>
				{
					foreach (var d in na)
					{
						Debug.Log("Discovered " + d.name);
					}
					discoveredDice.AddRange(na);
				});

				if (discoveredDice.Count > 0)
				{
					foreach (var dna in discoveredDice)
					{
						// Find die by name
						var udie = dice.Find(d => d.name == dna.name);
						if (udie != null) {
							Debug.Log("Updating " + dna.name + "'s address (" + udie.address + "->" + dna.address + ")");
							// Update its address
							udie.address = dna.address;
						} else {
							Debug.Log("Creating new entry for " + dna.name + " (" + dna.address + ")");
							// Create new die entry
							udie = addNewDie(dna);
						}

						// And re-connect!
						Debug.Log("Connecting to " + udie.name);
						yield return central.ConnectToDeviceCr (udie.address,
							data => {
								Debug.Log("Data received for " + udie.name + " (" + data[0] + ")");
								OnDieDataReceived(udie, data[0]);
								udie.Face = data [0];
							},
							() => {
								Debug.Log("Lost connection to " + udie.name);
								// On disconnection, retrigger scan
								triggerScan = true;
							});
					}
				}
			}
			else
			{
				yield return null;
			}

			// Update the dice!
			foreach (var die in dice)
			{
				die.Update();
			}
		}
	}

	IEnumerator PingDice(List<Die> dice, float interval)
	{
		int dieIndex = 0;
		while (true)
		{
			// Wait a bit
			yield return new WaitForSeconds (interval);

			// Ping the die!
			PingDie(dice[dieIndex]);

			// Increment and wrap around
			dieIndex++;
			if (dieIndex >= dice.Count)
				dieIndex = 0;
		}
	}

	// Update is called once per frame
	void Update ()
	{
	}

	void DisplayMessage(string message)
	{
		consoleOutputUI.text = message;
		consoleOutputUI.color = Color.white;
	}

	void DisplayError(string message)
	{
		consoleOutputUI.text = message;
		consoleOutputUI.color = Color.red;
	}

	void ClearConsole()
	{
		consoleOutputUI.text = "";
	}

	void ShowDieCommands(Die die)
	{
		mainUI.SetActive(false);
		diceControlUI.Setup(die.name, HideDieCommands);
		diceControlUI.gameObject.SetActive(true);
		diceControlUI.AddCommand("1", () => PlayAnimation(die, 0));
		diceControlUI.AddCommand("2", () => PlayAnimation(die, 1));
		diceControlUI.AddCommand("3", () => PlayAnimation(die, 2));
		diceControlUI.AddCommand("4", () => PlayAnimation(die, 3));
		diceControlUI.AddCommand("5", () => PlayAnimation(die, 4));
		diceControlUI.AddCommand("6", () => PlayAnimation(die, 5));
		diceControlUI.AddCommand("Rand", () => PlayAnimation(die, 6));
	}

	void HideDieCommands()
	{
		diceControlUI.gameObject.SetActive(false);
		mainUI.SetActive(true);
	}

	public Coroutine PlayAnimation(Die die, int animationIndex)
	{
		byte[] data = new byte[] { (byte)'A', (byte)'N', (byte)'I', (byte)'M', (byte)animationIndex };
		return central.SendBytesCr(die.address, data);
	}


	public Coroutine PingDie(Die die)
	{
		byte[] data = new byte[] { (byte)'R', (byte)'D', (byte)'G', (byte)'T' };
		return central.SendBytesCr(die.address, data);
	}

	public void OnDieDataReceived(Die die, int face)
	{

	}
}
