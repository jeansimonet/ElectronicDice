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

	public RectTransform Left { get { return _left; } }
	public RectTransform Movable { get { return _movable; } }
	public RectTransform Right { get { return _right; } }

	// Use this for initialization
	void Start()
	{
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
