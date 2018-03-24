﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Linq;

public class CaptureVideo
    : MonoBehaviour
{
   
	// Use this for initialization
	void Start ()
    {
        foreach (var dev in WebCamTexture.devices)
        {
            Debug.Log(dev.name);
        }
        var cam = WebCamTexture.devices[0];
        WebCamTexture tex = new WebCamTexture(cam.name);
        tex.Play();

		GetComponent<MeshRenderer> ().material.mainTexture = tex;
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
