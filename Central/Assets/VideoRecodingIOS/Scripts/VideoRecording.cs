using UnityEngine;
using System.Collections;

public class VideoRecording : MonoBehaviour {
	string messageFromIosReceivingGameObjectName = "UnityReceiveMessage";
	string messageFromIosReceivingMethodName = "RecordingSavedStatus";
	string albumName = "Test";
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnGUI()
	{
		if (GUI.Button (new Rect(10, 10, 120, 50), "Start Camera"))
			VideoRecordingBridge.OpenCamera(1, 300, 500, 300, 360,false);

		if (GUI.Button (new Rect(10, 70, 120, 50), "Open Library"))
			VideoRecordingBridge.OpenGallery(2, 300, 500, 300, 360,false);

		if (GUI.Button (new Rect(10, 130, 120, 50), "Show Recording"))
			VideoRecordingBridge.ShowCapturedRecording(true);

		if (GUI.Button (new Rect(10, 190, 120, 50), "Hide Recording"))
			VideoRecordingBridge.ShowCapturedRecording(false);

		if (GUI.Button (new Rect (150, 10, 220, 50), "Save Recording To Camera Roll")) {
			messageFromIosReceivingMethodName = "RecordingSavedStatusToCameraRoll";	
			VideoRecordingBridge.ToSaveRecordingToCameraRoll (messageFromIosReceivingGameObjectName, messageFromIosReceivingMethodName);
				}

		if (GUI.Button (new Rect(150, 70, 220, 50), "Save Recording To Specific Album")){
			messageFromIosReceivingMethodName = "RecordingSavedStatusToSpecificAlbum";	
			VideoRecordingBridge.ToSaveRecordingToSpecificAlbum(messageFromIosReceivingGameObjectName,messageFromIosReceivingMethodName,albumName);
		}

		GUI.Label (new Rect (340, 10, 200, 40), UnityReceiveMessages.Instance.CurrentRecordingStatusGetter ());
	}
}
