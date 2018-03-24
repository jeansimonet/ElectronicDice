using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum CentralState
{
    Uninitialized = 0,
    Initializing,
    Idle,
    Scanning,
    Connecting,
    Error,
}


public interface ICentral
{
    CentralState state { get; }
    void BeginScanForDice(System.Action<Die> foundDieCallback);
    void StopScanForDice();
    void ConnectToDie(Die die, System.Action<Die> dieConnectedCallback, System.Action<Die> dieDisconnectedCallback);
    void DisconnectDie(Die die);
    void DisconnectAllDice();
}

public interface ISendBytes
{
    void SendBytes(Die die, byte[] bytes, int length, System.Action bytesWrittenCallback);
}

public class Central
	: MonoBehaviour
    , ICentral
    , ISendBytes
{
	public string serviceGUID = "fe84";
	public string subscribeCharacteristic = "2d30c082-f39f-4ce6-923f-3484ea480596";
	public string writeCharacteristic = "2d30c083-f39f-4ce6-923f-3484ea480596";

    CentralState _state = CentralState.Uninitialized;

    public CentralState state
    {
        get { return _state; }
    }

    void Start()
	{
		_state = CentralState.Initializing;
		BluetoothLEHardwareInterface.Initialize(true, false,
		() =>
		{
			_state = CentralState.Idle;
		},
		(err) =>
		{
			_state = CentralState.Error;
            Debug.LogError("Error initializing Bluetooth Central: " + err);

        });
	}

	public void BeginScanForDice(System.Action<Die> foundDieCallback)
	{
        if (_state == CentralState.Idle)
        {
            // Destroy any die that is not currently connected!
            foreach (var die in GetComponentsInChildren<Die>())
            {
                if (!die.connected)
                {
                    DestroyDie(die);
                }
            }

            _state = CentralState.Scanning;
            BluetoothLEHardwareInterface.ScanForPeripheralsWithServices(
                new string[] { serviceGUID },
                (name, address) => foundDieCallback(CreateDie(name, address)), null, false, false);
        }
        else
        {
            Debug.LogError("Central is not ready to scan, current state is " + _state);
        }
	}

    Die CreateDie(string name, string address)
    {
        // Create a die game object
        var dieGO = new GameObject(name);
        dieGO.transform.SetParent(transform, false);

        // Add the Die component and return it!
        var ret = dieGO.AddComponent<Die>();
        ret.address = address;
        return ret;
    }

    public void StopScanForDice()
    {
        if (_state == CentralState.Scanning)
        {
            BluetoothLEHardwareInterface.StopScan();
            _state = CentralState.Idle;
        }
        else
        {
            Debug.LogError("Central is not currently scanning for devices, current state is " + _state);
        }
    }

	public void ConnectToDie(Die die,
        System.Action<Die> dieConnectedCallback,
        System.Action<Die> dieDisconnectedCallback)
	{
        if (_state == CentralState.Idle)
        {
            _state = CentralState.Connecting;
            bool readCharacDiscovered = false;
            bool writeCharacDiscovered = false;
            BluetoothLEHardwareInterface.ConnectToPeripheral(die.address,
                null,
                null,
                (ad, serv, charac) =>
                {
                    // Check for the service guid to match that for our dice (it's the Simblee one)
                    if (ad == die.address && serv == serviceGUID)
                    {
                        // Check the discovered characteristic
                        if (charac == subscribeCharacteristic)
                        {
                            // It's the read characteristic, subscribe to it!
                            BluetoothLEHardwareInterface.SubscribeCharacteristic(die.address,
                                serviceGUID,
                                subscribeCharacteristic,
                                null,
                                (dev, data) =>
                                {
                                    if (dev == die.address)
                                    {
                                        die.DataReceived(data);
                                    }
                                });
                            readCharacDiscovered = true;
                        }
                        else if (charac == writeCharacteristic)
                        {
                            // It's the write characteristic, remember that
                            writeCharacDiscovered = true;
                        }

                        // Do we have both read and write access? If so we're good to go!
                        if (readCharacDiscovered && writeCharacDiscovered)
                        {
                            // We're ready to go
                            die.Connect(this);
                            _state = CentralState.Idle;
                            dieConnectedCallback(die);
                        }
                    }
                },
                (ad) =>
                {
                    if (ad == die.address)
                    {
                        dieDisconnectedCallback(die);
                    }
                });
        }
        else
        {
            Debug.LogError("Central is not ready to connect, current state is " + _state);
        }
	}

    public void DisconnectDie(Die die)
    {
        die.Disconnect();
        BluetoothLEHardwareInterface.DisconnectPeripheral(die.address, null);
    }

    public void DisconnectAllDice()
    {
        foreach (var die in GetComponentsInChildren<Die>())
        {
            die.Disconnect();
        }
        BluetoothLEHardwareInterface.DisconnectAll();
    }

    void DestroyDie(Die die)
    {
        GameObject.Destroy(die.gameObject);
    }

    public void SendBytes(Die die, byte[] bytes, int length, System.Action bytesWrittenCallback)
	{
		BluetoothLEHardwareInterface.WriteCharacteristic(die.address, serviceGUID, writeCharacteristic, bytes, length, false, (ignore) => bytesWrittenCallback());
	}

    public void OnApplicationQuit()
    {
        DisconnectAllDice();
    }
}
