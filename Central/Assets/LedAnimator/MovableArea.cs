using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovableArea : MonoBehaviour
{
	[SerializeField]
	BorderHandle _leftHandle = null;
	[SerializeField]
	BorderHandle _rightHandle = null;
	[SerializeField]
	RectTransform _movable = null;

	public float LeftWidth { get { return (_leftHandle.transform as RectTransform).rect.width; } }
	public float RightWidth { get { return (_rightHandle.transform as RectTransform).rect.width; } }

	public RectTransform Movable { get { return _movable; } }

	public void Maximize()
	{
		var pos = _rightHandle.transform.localPosition;
		pos.x = (transform as RectTransform).rect.width;
		_rightHandle.transform.localPosition = pos;
		FitToHandles();
	}

	void FitToHandles()
	{
		OnLeftHandleMoved();
		OnRightHandleMoved();
	}

	void OnLeftHandleMoved()
	{
		var min = _movable.offsetMin;
		min.x = _leftHandle.transform.localPosition.x;
		_movable.offsetMin = min;
	}

	void OnRightHandleMoved()
	{
		var max = _movable.offsetMax;
		max.x = _rightHandle.transform.localPosition.x;
		_movable.offsetMax = max;
	}

	void OnEnable()
	{
		_leftHandle.Moved += OnLeftHandleMoved;
		_rightHandle.Moved += OnRightHandleMoved;
	}

	void OnDisable()
	{
		_leftHandle.Moved -= OnLeftHandleMoved;
		_rightHandle.Moved -= OnRightHandleMoved;
	}

	// Use this for initialization
	void Start()
	{
		FitToHandles();
	}

	// Update is called once per frame
	void Update()
	{
	}
}
