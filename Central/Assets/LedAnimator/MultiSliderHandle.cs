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

	public MultiSliderHandle DuplicateSelf()
	{
		var dupHandle = GameObject.Instantiate<MultiSliderHandle>(this, transform.parent);
		_slider.Repaint(); //TODO slider should be notified instead
		_slider.SelectHandle(dupHandle);
		return dupHandle;
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
		float xMin = _slider.transform.TransformPoint(Vector3.right * rect.xMin).x;
		float xMax = _slider.transform.TransformPoint(Vector3.right * rect.xMax).x;
		float yMin = _slider.transform.TransformPoint(Vector3.up * rect.yMin).y;
		float yMax = _slider.transform.TransformPoint(Vector3.up * rect.yMax).y;

		if (_slider.Direction == SliderDirection.Horizontal)
		{
			float x = Mathf.Clamp(Input.mousePosition.x, xMin, xMax);
			float y = Mathf.Lerp(yMin, yMax, _slider.HandlePosition);
			transform.position = new Vector2(x, y);
		}
		else
		{
			float y = Mathf.Clamp(Input.mousePosition.y, yMin, yMax);
			float x = Mathf.Lerp(xMin, xMax, _slider.HandlePosition);
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
