using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Die3D : MonoBehaviour
{
    public Camera dieCamera;
    public GameObject die;
    public int RenderTextureSize = 512;
    public float MaxRotationSpeed = 1080.0f; // d/s

    RenderTexture renderTexture;
    Quaternion currentDiceRot;

	// Use this for initialization
	void Start () {
		
	}

    public RenderTexture Setup(int index)
    {
        int layerIndex = LayerMask.NameToLayer("Dice " + index);
        renderTexture = new RenderTexture(RenderTextureSize, RenderTextureSize, 0, RenderTextureFormat.ARGB32, RenderTextureReadWrite.Linear);
        renderTexture.wrapMode = TextureWrapMode.Clamp;
        renderTexture.filterMode = FilterMode.Point;
        renderTexture.Create();

        dieCamera.cullingMask = 1 << layerIndex; // only render particle effects
        dieCamera.targetTexture = renderTexture;

        die.layer = layerIndex;

        return renderTexture;
    }

    public void UpdateAcceleration(Vector3 acc)
    {
        // -acc is up
        Vector3 up = (-acc).normalized;
        Vector3 right = Vector3.Cross(up, transform.forward);
        Vector3 newForward = Vector3.Cross(right, up);
        currentDiceRot = Quaternion.LookRotation(newForward, up);
    }

    void OnDestroy()
    {
        if (renderTexture != null)
        {
            renderTexture.Release();
            renderTexture = null;
        }
    }


    // Update is called once per frame
    void Update ()
    {
        float maxDelta = Time.deltaTime * MaxRotationSpeed;
        die.transform.localRotation = Quaternion.RotateTowards(die.transform.localRotation, currentDiceRot, maxDelta);
	}
}
