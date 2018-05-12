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
	[SerializeField]
	RectTransform _confirmRemovePanel = null;

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

	public void ConfirmRemoveSelf()
	{
		_confirmRemovePanel.gameObject.SetActive(true);
	}

	public void RemoveSelf()
	{
		GameObject.Destroy(gameObject);
		//TODO ActiveColorAnimator = null;
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

	public Animations.RGBAnimationTrack Serialize(float startOffset, float unitSize)
	{
		var rect = (ColorSlider.transform as RectTransform).rect;
		string spriteName = _image.sprite.name;
		int face = int.Parse(spriteName[spriteName.IndexOf('-') - 1].ToString());
		int point = int.Parse(spriteName[spriteName.IndexOf('-') + 1].ToString());
		float start = transform.InverseTransformPoint(ColorSlider.transform.TransformPoint(rect.xMin, 0, 0)).x - startOffset;
		float end = transform.InverseTransformPoint(ColorSlider.transform.TransformPoint(rect.xMax, 0, 0)).x - startOffset;
		return new Animations.RGBAnimationTrack()
		{
			startTime = (short)Mathf.RoundToInt(1000 * start / unitSize),
			duration = (short)Mathf.RoundToInt(1000 * (end - start) / unitSize),
			ledIndex = (byte)(Enumerable.Range(1, face - 1).Sum() + point - 1),
			count = 1,
			keyframes = ColorSlider.Serialize(),
		};
	}

	IFocusable[] FindFocusables()
	{
		return GetComponentsInChildren<IFocusable>().Where(f => (object)f != this).ToArray();
	}

	// Use this for initialization
	void Start()
	{
		_confirmRemovePanel.gameObject.SetActive(false);
	}

	// Update is called once per frame
	void Update()
	{

	}
}
