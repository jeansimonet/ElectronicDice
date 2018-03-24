using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Die
	: MonoBehaviour
{
	public const float SCALE_2G = 2.0f;
	public const float SCALE_4G = 4.0f;
	public const float SCALE_8G = 8.0f;
	float scale = SCALE_8G;

	public enum State
	{
		Disconnected = -1,
		Unknown = 0,
		Face1 = 1,
		Face2,
		Face3,
		Face4,
		Face5,
		Face6,
		Handling,
		Falling,
		Rolling,
		Jerking,
		Crooked,
		Count
	}

	public State state
	{
		get { return _state; }
	}

	// Name is already a part of Monobehaviour
	public string address
	{
		get;
		set;
	}

	public bool connected
	{
		get
		{
			return _state != State.Disconnected;
		}
	}

	public int face
	{
		get
		{
			int ret = (int)_state;
			if (ret < 1 || ret > 6)
			{
				ret = -1;
			}
			return ret;
		}
	}

	public delegate void TelemetryEvent(Vector3 acc, int millis);
	public TelemetryEvent OnTelemetry;

	public delegate void StateChangedEvent(State newState);
	public StateChangedEvent OnStateChanged;

	// For telemetry
	int lastSampleTime; // ms
	ISendBytes _sendBytes;
	State _state = State.Disconnected;

	void Awake()
	{
	}

	// Use this for initialization
	void Start ()
	{
	}
	
	// Update is called once per frame
	void Update ()
	{
		
	}

	public void PlayAnimation(int animationIndex)
	{
		if (connected)
		{
			byte[] data = new byte[] { (byte)'A', (byte)'N', (byte)'I', (byte)'M', (byte)animationIndex };
			_sendBytes.SendBytes(this, data, 5, null);
		}
	}


	public void Ping()
	{
		if (connected)
		{
			byte[] data = new byte[] { (byte)'R', (byte)'D', (byte)'G', (byte)'T' };
			_sendBytes.SendBytes(this, data, 4, null);
		}
	}

	public void ForceState(State forcedState)
	{
		Debug.Log("Forcing state of die " + name + " to " + forcedState);
		_state = forcedState;
	}

	public void Connect(ISendBytes sb)
	{
		_sendBytes = sb;
		_state = State.Unknown;

		// Ping the die so we know its initial state
		Ping();
	}

	public void Disconnect()
	{
		_sendBytes = null;
		_state = State.Disconnected;
	}

	public void DataReceived(byte[] data)
	{
		if (!connected)
		{
			Debug.LogError("Die " + name + " received data while disconnected!");
			return;
		}
		
		// Process the message coming from the actual die!
		var message = DieMessages.FromByteArray(data);
		if (message is DieMessageFace)
		{
			// Handle the message
			var faceMessage = (DieMessageFace)message;
			_state = (State)faceMessage.face;

			// Notify anyone who cares
			if (OnStateChanged != null)
			{
				OnStateChanged.Invoke(state);
			}
		}
		else if (message is DieMessageAcc)
		{
			var telem = (DieMessageAcc)message;

			for (int i = 0; i < 2; ++i)
			{
				// Compute actual accelerometer readings (in Gs)
				float cx = (float)telem.data[i].X / (float)(1 << 11) * (float)(scale);
				float cy = (float)telem.data[i].Y / (float)(1 << 11) * (float)(scale);
				float cz = (float)telem.data[i].Z / (float)(1 << 11) * (float)(scale);
				Vector3 acc = new Vector3(cx, cy, cz);

				lastSampleTime += telem.data[i].DeltaTime;

				// Notify anyone who cares
				if (OnTelemetry != null)
				{
					OnTelemetry.Invoke(acc, lastSampleTime);
				}
			}
		}
	}
}
