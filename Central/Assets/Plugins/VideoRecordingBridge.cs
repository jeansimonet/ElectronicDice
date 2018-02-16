using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class VideoRecordingBridge {

string MessageReceivingGameObjectNameVariable;
string MessageFromIosReceivingMethodNameVariable;
//----------------------------------------------Show Recording-------------------------------------------------------------------
	[DllImport("__Internal")]
	private static extern bool _ShowRecordingCapture (string modeTypeOfDatePicker,string origin_x,string origin_y,string width,string height,bool isShowRecording);
	public static void ShowRecordingCapture(int modeTypeInt,float origin_x,float origin_y,float width,float height,bool isShowRecording){
		string modeType = "" + ((modeTypeInt % 4)+1);
		if (modeType.Equals ("1") || modeType.Equals ("2") || modeType.Equals ("3") || modeType.Equals ("4")) {
			_ShowRecordingCapture (modeType,origin_x.ToString(),origin_y.ToString(),width.ToString(),height.ToString(),isShowRecording);
		} 
	}
//----------------------------------------------Show Camera-------------------------------------------------------------------
	[DllImport("__Internal")]
	private static extern void _OpenCamera ();
	public static void OpenCamera(int modeTypeInt,float origin_x,float origin_y,float width,float height,bool isShowRecording){
		ShowRecordingCapture (modeTypeInt, origin_x, origin_y, width, height,isShowRecording);
			_OpenCamera ();
	}
//----------------------------------------------Show Library-------------------------------------------------------------------
	[DllImport("__Internal")]
	private static extern void _OpenGallery ();
	public static void OpenGallery(int modeTypeInt,float origin_x,float origin_y,float width,float height,bool isShowRecording){
		ShowRecordingCapture (modeTypeInt, origin_x, origin_y, width, height,isShowRecording);
		_OpenGallery ();
	}
//----------------------------------------------Show Or Hide Recording-------------------------------------------------------------------

	// if isSowRecording is true then recording will be shown in recording view and if false then recording will be hideen

	[DllImport("__Internal")]
	private static extern void _ShowCapturedRecording (bool isShowRecording);
	public static void ShowCapturedRecording(bool isShowRecording){
		_ShowCapturedRecording (isShowRecording);
	}
//----------------------------------------------Save Recording To Camera Roll-------------------------------------------------------------------
	[DllImport("__Internal")]
	private static extern void _ToSaveRecordingToCameraRoll (string msgReceivingGameObjectName,string msgReceivingMethodName);
	public static void ToSaveRecordingToCameraRoll(string msgReceivingGameObjectName,string msgReceivingMethodName){
		_ToSaveRecordingToCameraRoll (msgReceivingGameObjectName,msgReceivingMethodName);
	}
//----------------------------------------------Save Recording To Specific Album-------------------------------------------------------------------
	[DllImport("__Internal")]
	private static extern void _ToSaveRecordingToSpecificAlbum (string msgReceivingGameObjectName,string msgReceivingMethodName,string albumName);
	public static void ToSaveRecordingToSpecificAlbum(string msgReceivingGameObjectName,string msgReceivingMethodName,string albumName){
		_ToSaveRecordingToSpecificAlbum (msgReceivingGameObjectName,msgReceivingMethodName,albumName);
	}
}