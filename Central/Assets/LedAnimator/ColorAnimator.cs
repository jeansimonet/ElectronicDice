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

	public Animations.RGBAnimationTrack Serialize(float unitSize)
	{
		int offset = 120; //TODO
		var rect = (ColorSlider.transform as RectTransform).rect;
		string spriteName = _image.sprite.name;
		int face = int.Parse(spriteName[spriteName.IndexOf('-') - 1].ToString());
		int point = int.Parse(spriteName[spriteName.IndexOf('-') + 1].ToString());
		float start = transform.InverseTransformPoint(ColorSlider.transform.TransformPoint(rect.xMin - offset, 0, 0)).x / unitSize;
		float end = transform.InverseTransformPoint(ColorSlider.transform.TransformPoint(rect.xMax - offset, 0, 0)).x / unitSize;
		return new Animations.RGBAnimationTrack()
		{
			startTime = (short)Mathf.RoundToInt(1000 * start),
			duration = (short)Mathf.RoundToInt(1000 * (end - start)),
			ledIndex = (byte)(Enumerable.Range(1, face - 1).Sum() + point - 1),
			count = 1,
			keyframes = ColorSlider.Serialize(end - start),
		};
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
