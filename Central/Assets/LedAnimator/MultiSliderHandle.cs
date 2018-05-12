﻿using System;
using System.Collections;
using System.Collections.Generic;
using Animations;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

using static MultiSlider;

public class MultiSliderHandle : MonoBehaviour, IPointerDownHandler, IDragHandler
{
	[SerializeField]
	RectTransform _selectedOvr = null;

	Image _image;
	MultiSlider _slider;

	//public bool IsSelected { get { return EventSystem.current.currentSelectedGameObject == gameObject; } }
	public bool Selected { get { return _slider.ActiveHandle == this; } }
	public Color Color { get { return _image.color; } }

	public void ChangeColor(Color color)
	{
		ChangeColor(color, false);
	}

	public void ChangeColor(Color color, bool noRepaint)
	{
		_image.color = color;
		if (!noRepaint)
		{
			_slider.Repaint();
		}
	}

	public MultiSliderHandle Duplicate()
	{
		var dupHandle = GameObject.Instantiate<MultiSliderHandle>(this, transform.parent);
		//Keep same color dupHandle.ChangeColor(Palette.Instance.ActiveColor);
		_slider.Repaint(); //TODO slider should be notified instead
		_slider.SelectHandle(dupHandle);
		return dupHandle;
	}

	public void DuplicateSelf()
	{
		var dupHandle = Duplicate();
		dupHandle.transform.localPosition = transform.localPosition;
	}

	public void RemoveSelf()
	{
		_slider.RemoveHandle(this);
	}

	void IPointerDownHandler.OnPointerDown(PointerEventData eventData)
	{
		//Debug.Log("OnPointerDown " + name);

		//EventSystem.current.SetSelectedGameObject(gameObject);
		_slider.SelectHandle(this);
	}

	void IDragHandler.OnDrag(PointerEventData eventData)
	{
		//Debug.Log("OnDrag " + name);

		var rect = (_slider.transform as RectTransform).rect;
		Vector2 min = _slider.transform.TransformPoint(rect.xMin, rect.yMin, 0);
		Vector2 max = _slider.transform.TransformPoint(rect.xMax, rect.yMax, 0);

		if (_slider.Direction == SliderDirection.Horizontal)
		{
			float x = Mathf.Clamp(Input.mousePosition.x, min.x, max.x);
			float y = Mathf.Lerp(min.y, max.y, _slider.HandlePosition);
			transform.position = new Vector2(x, y);
		}
		else
		{
			float y = Mathf.Clamp(Input.mousePosition.y, min.y, max.y);
			float x = Mathf.Lerp(min.x, max.x, _slider.HandlePosition);
			transform.position = new Vector2(x, y);
		}

		_slider.Repaint();
	}

	void OnHandleSelected(MultiSliderHandle handle)
	{
		//Debug.Log("OnHandleSelected " + name);
		Repaint();

		Palette.Instance.ColorSelected -= ChangeColor;
		if (Selected)
		{
			Palette.Instance.ColorSelected += ChangeColor;
		}
	}

	void Repaint()
	{
		//Debug.Log("Repaint " + name);
		transform.localScale = Vector2.one * (Selected ? 1.2f : 1f);
		if (_selectedOvr != null)
		{
			_selectedOvr.gameObject.SetActive(Selected);
		}
	}

	void OnEnable()
	{
		_slider = GetComponentInParent<MultiSlider>();
		_slider.HandleSelected += OnHandleSelected;
		//Repaint();
	}

	void OnDisable()
	{
		Palette.Instance.ColorSelected -= ChangeColor;
		_slider.HandleSelected -= OnHandleSelected;
		_slider = null;
	}

	void Awake()
	{
		_image = GetComponent<Image>();
		_selectedOvr.gameObject.SetActive(false);
	}

	// Use this for initialization
	void Start()
	{
	}

	// Update is called once per frame
	void Update()
	{
	}
}
