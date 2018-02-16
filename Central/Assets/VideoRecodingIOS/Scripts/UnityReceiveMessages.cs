using UnityEngine;
using System.Collections;
using System.IO;
public class UnityReceiveMessages : MonoBehaviour {
	public static UnityReceiveMessages Instance;
	string currentRecordingStatus;
	public GameObject guiTex;
	private Texture2D texture2D;
	void Awake(){
		Instance = this;
	}

	// Use this for initialization
	void Start () {
	
	}

	// Update is called once per frame
	void Update () {
	
	}
	public void RecordingSavedStatusToCameraRoll(string recodingStatus){
		currentRecordingStatus = recodingStatus;
	}

	public void RecordingSavedStatusToSpecificAlbum(string recodingStatus){
		currentRecordingStatus = recodingStatus;
	}

	public string CurrentRecordingStatusGetter()
	{
		return currentRecordingStatus;
	}
}
