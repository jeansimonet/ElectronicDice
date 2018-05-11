using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovableArea : MonoBehaviour
{
	[SerializeField]
	RectTransform _left = null;
	[SerializeField]
	RectTransform _movable = null;
	[SerializeField]
	RectTransform _right = null;

	public float LeftWidth { get { return _left.rect.width; } }
	public float RightWidth { get { return _right.rect.width; } }

	public RectTransform Left { get { return _left; } }
	public RectTransform Movable { get { return _movable; } }
	public RectTransform Right { get { return _right; } }

	// Use this for initialization
	IEnumerator Start()
	{
		yield return null;
		// Start maximized (need to wait for next frame to get correct width, I guess because it's auto sized by layout system later on)
		var pos = _right.localPosition;
		pos.x = (transform as RectTransform).rect.width;
		_right.localPosition = pos;
	}

	// Update is called once per frame
	void Update()
	{
		var min = _movable.offsetMin;
		min.x = _left.localPosition.x;
		_movable.offsetMin = min;

		var max = _movable.offsetMax;
		max.x = _right.localPosition.x;
		_movable.offsetMax = max;
	}
}
