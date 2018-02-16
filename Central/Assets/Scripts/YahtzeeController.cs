using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


public class YahtzeeController : MonoBehaviour
{
	public Text consoleOutputUI;
	public Button scanButtonUI;
	public Text scanButtonTextUI;
	public GameObject diceRootUI;
	public UIDie diePrefab;
	public GameObject mainUI;
	public DiceControl diceControlUI;
	public Text CurrentYahtzeeUI;

	Central central;

	// Our running list of die
	List<Die> dice;

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
		dice = new List<Die>();

		System.Func<Central.NameAndAddress, Die> addNewDie = (dna) => {
			var dieUI = GameObject.Instantiate<UIDie>(diePrefab);
			dieUI.transform.SetParent(diceRootUI.transform);
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
		bool triggerScan = true;
		bool triggerClear = true;
		//StartCoroutine(PingDice(() => dice, 0.1f));

		scanButtonUI.onClick.AddListener(() =>
		{
			triggerClear = true;
			triggerScan = true;
		});

		var lostDice = new List<Central.NameAndAddress>();
		while (true)
		{
			if (triggerClear)
			{
				triggerClear = false;
				foreach (var d in dice)
				{
					d.Dispose();
				}
				dice.Clear();
			}

			var discoveredDice = new List<Central.NameAndAddress>();
			if (triggerScan)
			{
				triggerScan = false;
				yield return central.ScanForDevicesCr((na) =>
				{
					//foreach (var d in na)
					//{
					//	Debug.Log("Discovered " + d.name);
					//}
					discoveredDice.AddRange(na);
				});
			}

			if (discoveredDice.Count > 0)
			{
				foreach (var dna in discoveredDice)
				{
					// Find die by name
					var udie = dice.Find(d => d.name == dna.name);
					if (udie != null)
					{
						//Debug.Log("Updating " + dna.name + "'s address (" + udie.address + "->" + dna.address + ")");
						// Update its address
						udie.address = dna.address;
					}
					else
					{
						//Debug.Log("Creating new entry for " + dna.name + " (" + dna.address + ")");
						// Create new die entry
						udie = addNewDie(dna);
					}

					// And re-connect!
					Debug.Log("Connecting to " + udie.name);
					yield return central.ConnectToDeviceCr(udie.address,
						data => {
                            var message = DieMessages.FromByteArray(data);
                            if (message is DieMessageFace)
                            {
                                var faceMessage = (DieMessageFace)message;
                                udie.Face = faceMessage.face;
                                OnFaceDataReceived();
                            }
                        },
						() => {
							Debug.Log("Lost connection to " + udie.name);
							// On disconnection, retrigger scan
							//triggerScan = true;
							lostDice.Add(dna);
						});
				}

				discoveredDice.AddRange(lostDice);
				lostDice.Clear();
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

	IEnumerator PingDice(System.Func<List<Die>> getDice, float interval)
	{
		int dieIndex = 0;
		List<Die> dice = getDice();
		while (true)
		{
			if (dice.Count > 0)
			{

				// Wait a bit
				yield return new WaitForSeconds(interval);

				// Ping the die!
				PingDie(dice[dieIndex]);

				// Increment and wrap around
				dieIndex++;
				if (dieIndex >= dice.Count)
					dieIndex = 0;
			}
			else
			{
				yield return new WaitForSeconds(1.0f);
			}
			dice = getDice();
		}
	}

	// Update is called once per frame
	void Update()
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
		Debug.Log("Pinging " + die.name);
		byte[] data = new byte[] { (byte)'R', (byte)'D', (byte)'G', (byte)'T' };
		return central.SendBytesCr(die.address, data);
	}

	enum YahtzeeValue
	{
		ThreeOfAKind,
		FourOfAKind,
		FullHouse,
		SmallStraight,
		LargeStraight,
		Yahtzee
	}

	public void OnFaceDataReceived()
	{
		// List all die faces
		List<int> faces = new List<int>(dice.Select(d => d.Face));
		faces.Sort();
		faces.Reverse();

		// Count how many of each
		int[] counts = new int[6];
		foreach (var die in dice)
		{
			counts[die.Face]++;
		}

		switch (counts.Max())
		{
			case 5:
				CurrentYahtzeeUI.text = "YAHTZEE";
				break;
			case 4:
				CurrentYahtzeeUI.text = "Four of a kind";
				break;
			case 3:
				if (faces.Contains(2))
				{
					CurrentYahtzeeUI.text = "Full House";
				}
				else
				{
					CurrentYahtzeeUI.text = "Three of a kind";
				}
				break;
			case 2:
			case 1:
				if (counts[0] >= 1 &&
					 counts[1] >= 1 &&
					 counts[2] >= 1 &&
					 counts[3] >= 1 &&
					 counts[4] >= 1)
				{
					CurrentYahtzeeUI.text = "Small Straight";
				}
				else if (counts[1] >= 1 &&
					 counts[2] >= 1 &&
					 counts[3] >= 1 &&
					 counts[4] >= 1 &&
					 counts[5] >= 1)
				{
					CurrentYahtzeeUI.text = "Large Straight";
				}
				else
				{
					CurrentYahtzeeUI.text = "";
				}
				break;
			default:
				CurrentYahtzeeUI.text = "";
				break;
		}
	}
}
