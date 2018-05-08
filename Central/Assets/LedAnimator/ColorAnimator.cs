using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class ColorAnimator : MonoBehaviour, IFocusable
{
	[SerializeField]
	Image _image = null;
	[SerializeField]
	MovableArea _movableColorSlider = null;

	public event System.Action<ColorAnimator> GotFocus;
	public bool HasFocus { get; private set; }

	public MultiSlider ColorSlider { get { return _movableColorSlider.Movable.GetComponentInChildren<MultiSlider>(); } }

	public void ChangeLed()
	{
		LedSelector.Instance.PickLed(sprite =>
		{
			if (sprite != null)
			{
				SetLedSprite(sprite);
			}
		});
	}

	public void SetLedSprite(Sprite sprite)
	{
		_image.sprite = sprite;
	}

	public void GiveFocus()
	{
		if (!HasFocus)
		{
			HasFocus = true;
			if (GotFocus != null) GotFocus(this);
		}
	}

	public void RemoveFocus()
	{
		HasFocus = false;
		foreach (var focusable in FindFocusables())
		{
			focusable.RemoveFocus();
		}
	}

	IFocusable[] FindFocusables()
	{
		return GetComponentsInChildren<IFocusable>().Where(f => (object)f != this).ToArray();
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
