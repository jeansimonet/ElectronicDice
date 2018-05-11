using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

using static MultiSlider;

public class MultiSliderHandle : MonoBehaviour, IPointerDownHandler, IDragHandler
{
	[SerializeField]
	RectTransform _selectedOvr = null;

	MultiSlider _slider;

	//public bool IsSelected { get { return EventSystem.current.currentSelectedGameObject == gameObject; } }
	public bool Selected { get { return _slider.ActiveHandle == this; } }

	public void ChangeColor(Color color)
	{
		GetComponent<Image>().color = color;
		_slider.Repaint();
	}

	public void DuplicateSelf()
	{
		var dupHandle = GameObject.Instantiate<MultiSliderHandle>(this, transform.parent);
		//Keep same color dupHandle.ChangeColor(Palette.Instance.ActiveColor);
		_slider.Repaint(); //TODO slider should be notified instead
		_slider.SelectHandle(dupHandle);
	}

	public void RemoveSelf()
	{
		_slider.RemoveHandle(this);
	}

	void IPointerDownHandler.OnPointerDown(PointerEventData eventData)
	{
		EventSystem.current.SetSelectedGameObject(gameObject);
		_slider.SelectHandle(this);
	}

	void IDragHandler.OnDrag(PointerEventData eventData)
	{
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
		Repaint();

		Palette.Instance.ColorSelected -= ChangeColor;
		if (Selected)
		{
			Palette.Instance.ColorSelected += ChangeColor;
			transform.localScale = 1.2f * Vector2.one;
		}
		else
		{
			transform.localScale = Vector2.one;
		}
	}

	void Repaint()
	{
		if (_selectedOvr != null)
		{
			_selectedOvr.gameObject.SetActive(Selected);
		}
	}

	void OnEnable()
	{
		_slider = GetComponentInParent<MultiSlider>();
		_slider.HandleSelected += OnHandleSelected;
		Repaint();
	}

	void OnDisable()
	{
		Palette.Instance.ColorSelected -= ChangeColor;
		_slider.HandleSelected -= OnHandleSelected;
		_slider = null;
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
