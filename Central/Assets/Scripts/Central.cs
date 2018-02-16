using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Central
	: MonoBehaviour
{
	public string serviceGUID = "fe84";
	public string subscribeCharacteristic = "2d30c082-f39f-4ce6-923f-3484ea480596";
	public string writeCharacteristic = "2d30c083-f39f-4ce6-923f-3484ea480596";

	public class NameAndAddress
	{
		public string name;
		public string address;
	}

	public Coroutine InitializeCr()
	{
		Coroutine ret = null;
		if (state == State.Disconnected || state == State.Error)
		{
			ret = StartCoroutine(Initialize());
		}
		return ret;
	}

	public Coroutine ScanForDevicesCr(System.Action<IEnumerable<NameAndAddress>> foundDevicesCallback)
	{
		Coroutine ret = null;
		if (state == State.Idle)
		{
			ret = StartCoroutine(ScanForDevices(foundDevicesCallback));
		}
		return ret;
	}

	public Coroutine ConnectToDeviceCr(string deviceAddress, System.Action<byte[]> dataReceivedCallback, System.Action deviceDisconnectedCallback)
	{
		Coroutine ret = null;
		if (state == State.Idle)
		{
			ret = StartCoroutine(ConnectToDevice(deviceAddress, dataReceivedCallback, deviceDisconnectedCallback));
		}
		return ret;
	}

	public Coroutine SendBytesCr(string deviceAddress, byte[] bytes)
	{
		return SendBytesCr(deviceAddress, bytes, bytes.Length);
	}

	public Coroutine SendBytesCr(string deviceAddress, byte[] bytes, int length)
	{
		Coroutine ret = null;
		if (state == State.Idle)
		{
			ret = StartCoroutine(SendBytes(deviceAddress, bytes, length));
		}
		return ret;
	}


	public void DisconnectAllDevices()
	{
		BluetoothLEHardwareInterface.DisconnectAll();
	}

	enum State
	{
		Disconnected = 0,
		Initializing,
		Idle,
		Scanning,
		Connecting,
		Waiting,
		Error,
	}

	State state = State.Disconnected;
	// Use this for initialization
	void Start()
	{

	}

	// Update is called once per frame
	void Update()
	{
	}

	IEnumerator Initialize()
	{
		state = State.Initializing;
		BluetoothLEHardwareInterface.Initialize(true, false,
		() =>
		{
			state = State.Idle;
		},
		(err) =>
		{
			state = State.Error;
		});

		while (state == State.Initializing)
			yield return null;
	}

	IEnumerator ScanForDevices(System.Action<IEnumerable<NameAndAddress>> foundDevicesCallback)
	{
		state = State.Scanning;

		// Scan for one second, store the names and addresses
		List<NameAndAddress> dice = new List<NameAndAddress>();
		BluetoothLEHardwareInterface.ScanForPeripheralsWithServices(new string[] { serviceGUID }, (add, name) => dice.Add(new NameAndAddress() { name = name, address = add }), null, false, false);
		yield return new WaitForSeconds(1.0f);
		BluetoothLEHardwareInterface.StopScan();

		// Callback
		state = State.Idle;
		foundDevicesCallback(dice);
	}

	IEnumerator ConnectToDevice(string deviceAddress, System.Action<byte[]> dataReceivedCallback, System.Action deviceDisconnectedCallback)
	{
		state = State.Connecting;
		BluetoothLEHardwareInterface.ConnectToPeripheral(deviceAddress,
			(ad) =>
			{
				state = State.Waiting;
			},
			null, null,
			(ad) =>
			{
				deviceDisconnectedCallback();
			});

		while (state == State.Connecting)
			yield return null;

		// Wait a bit for the central to discover all the characteristics
		yield return new WaitForSeconds(1.0f);

		// Register for the generic data characteristic
		BluetoothLEHardwareInterface.SubscribeCharacteristic(deviceAddress, serviceGUID, subscribeCharacteristic, null, (dev, data) => dataReceivedCallback(data));

		// Done!
		state = State.Idle;
	}

	IEnumerator SendBytes(string deviceAddress, byte[] bytes, int length)
	{
		bool written = false;
		BluetoothLEHardwareInterface.WriteCharacteristic(deviceAddress, serviceGUID, writeCharacteristic, bytes, length, false, (ignore) => written = true);
		while (!written)
			yield return null;
	}

    void OnApplicationQuit()
    {
        DisconnectAllDevices();
    }
}
