using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

public class ScannerTestScript : MonoBehaviour
{
	public float Timeout = 1f;
	public GameObject ScannedItemPrefab;

	private float _timeout;
	private Dictionary<string, ScannedItemScript> _scannedItems;

	// Use this for initialization
	void Start ()
	{
		_scannedItems = new Dictionary<string, ScannedItemScript> ();

		BluetoothLEHardwareInterface.Initialize (true, false, () => {

			_timeout = Timeout;
		}, 
		(error) => {
			
			BluetoothLEHardwareInterface.Log ("Error: " + error);

			if (error.Contains ("Bluetooth LE Not Enabled"))
				BluetoothLEHardwareInterface.BluetoothEnable (true);
		});
	}
	
	// Update is called once per frame
	void Update ()
	{
		_timeout -= Time.deltaTime;
		if (_timeout <= 0f)
		{
			_timeout = Timeout;

			BluetoothLEHardwareInterface.StopScan ();
			BluetoothLEHardwareInterface.ScanForPeripheralsWithServices (null, (address, name) => {

				BluetoothLEHardwareInterface.Log ("item scanned: " + address);
				if (_scannedItems.ContainsKey (address))
				{
					var scannedItem = _scannedItems[address];
					BluetoothLEHardwareInterface.Log ("already in list");
				}
				else
				{
					BluetoothLEHardwareInterface.Log ("item new: " + address);
					var newItem = Instantiate (ScannedItemPrefab);
					if (newItem != null)
					{
						BluetoothLEHardwareInterface.Log ("item created: " + address);
						newItem.transform.SetParent(transform);

						var scannedItem = newItem.GetComponent<ScannedItemScript> ();
						if (scannedItem != null)
						{
							BluetoothLEHardwareInterface.Log ("item set: " + address);
							scannedItem.TextAddressValue.text = address;
							scannedItem.TextNameValue.text = name;
							_scannedItems[address] = scannedItem;
						}
					}
				}
			}, null, true);
		}
	}
}
