using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Linq;

public class Die : System.IDisposable
{
	public Die(string name, string address, UIDie uiInstance)
	{
		this.name = name;
		this.address = address;
		UIInstance = uiInstance;
		throwCounts = new int[6];
		currentFace = -1;
		timeSinceLastFaceChange = 0.0f;
	}

	public string name
	{
		get;
		private set;
	}
	public string address
	{
		get;
		set;
	}
	public int totalThrows
	{
		get
		{
			return throwCounts.Sum();
		}
	}

	UIDie UIInstance;
	int[] throwCounts;
	int currentFace;
	float timeSinceLastFaceChange;

	public void Dispose()
	{
		if (UIInstance != null)
		{
			GameObject.Destroy(UIInstance.gameObject);
		}
		UIInstance = null;
	}

	public int Face
	{
		set
		{
			currentFace = value;
			UIInstance.faceNumber = value;
			timeSinceLastFaceChange = 0.0f;
		}
		get
		{
			return currentFace;
		}
	}

	public void Update()
	{
		timeSinceLastFaceChange += Time.deltaTime;
		if (timeSinceLastFaceChange > 0.3f)
		{
			if (currentFace != -1)
			{
				throwCounts[currentFace]++;

				// Update stats!
				for (int i = 0; i < 6; ++i)
				{
					float pct = (float)throwCounts[i] / totalThrows;
					UIInstance.SetFacePercent(i, pct);
				}
				currentFace = -1;
			}

			// Ping the dice

		}
	}

	public void Show()
	{
		UIInstance.gameObject.SetActive(true);
	}

	public void Hide()
	{
		UIInstance.gameObject.SetActive(false);
	}
}

public class DemoController : MonoBehaviour
{
	public Text consoleOutputUI;
	public Button scanButtonUI;
	public Text scanButtonTextUI;
	public GameObject diceRootUI;
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

		while (true)
		{
			if (triggerClear)
			{
				Debug.Log("Clearing");
				triggerClear = false;
				foreach (var d in dice)
				{
					d.Dispose();
				}
				dice.Clear();
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

	IEnumerator PingDice(System.Func<List<Die>> getDice, float interval)
	{
		int dieIndex = 0;
		List<Die> dice = getDice ();
		while (true)
		{
			if (dice.Count > 0) {
				
				// Wait a bit
				yield return new WaitForSeconds (interval);

				// Ping the die!
				PingDie (dice [dieIndex]);

				// Increment and wrap around
				dieIndex++;
				if (dieIndex >= dice.Count)
					dieIndex = 0;
			} else {
				yield return new WaitForSeconds (1.0f);
			}
			dice = getDice ();
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
		Debug.Log ("Pinging " + die.name);
		byte[] data = new byte[] { (byte)'R', (byte)'D', (byte)'G', (byte)'T' };
		return central.SendBytesCr(die.address, data);
	}
}
