using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
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

	public void Maximize()
	{
		_movableColorSlider.Maximize();
	}

	public void ConfirmRemoveSelf()
	{
		ShowConfirmRemove();
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
		//TODO remove startOffset, see Deserialize()
		var rect = (ColorSlider.transform as RectTransform).rect;
		float start = transform.InverseTransformPoint(ColorSlider.transform.TransformPoint(rect.xMin, 0, 0)).x - startOffset;
		float end = transform.InverseTransformPoint(ColorSlider.transform.TransformPoint(rect.xMax, 0, 0)).x - startOffset;
		return new Animations.RGBAnimationTrack()
		{
			startTime = (short)Mathf.RoundToInt(1000 * start / unitSize),
			duration = (short)Mathf.RoundToInt(1000 * (end - start) / unitSize),
			ledIndex = (byte)LedSpriteToIndex(_image.sprite.name),
			count = 1,
			keyframes = ColorSlider.Serialize(),
		};
	}

	public void Deserialize(Animations.RGBAnimationTrack track, float unitSize)
	{
		ShowConfirmRemove(false);
		SetLedSprite(LedSelector.Instance.GetLedSprite(ColorAnimator.IndexToLedSprite(track.ledIndex)));

		StartCoroutine(TestCr(track, unitSize)); //TODO
	}

	IEnumerator TestCr(Animations.RGBAnimationTrack track, float unitSize)
	{
		yield return null;

		Maximize();
		//var rectTransf = ColorSlider.transform as RectTransform;
		//var min = rectTransf.offsetMin;
		//min.x = track.startTime / 1000f * unitSize;
		//Debug.Log(min.x);
		//rectTransf.offsetMin = min;
		//var max = rectTransf.offsetMax;
		//max.x = track.duration / 1000f * unitSize;
		//Debug.Log(max.x);
		//max.x += min.x;
		//rectTransf.offsetMax = max;

		//_movableColorSlider.

		yield return null;
		ColorSlider.Deserialize(track.keyframes);
	}

	public static int LedSpriteToIndex(string spriteName)
	{
		int face = int.Parse(spriteName[spriteName.IndexOf('-') - 1].ToString());
		int point = int.Parse(spriteName[spriteName.IndexOf('-') + 1].ToString());
		return Enumerable.Range(1, face - 1).Sum() + point - 1;
	}

	public static string IndexToLedSprite(int index)
	{
		int face = 1;
		int count = 0;	
		while ((count + face) <= index)
		{
			count += face;
			++face;
		}
		int point = 1 + index - count;
		return string.Format("Led{0}-{1}", face, point);
	}

	IFocusable[] FindFocusables()
	{
		return GetComponentsInChildren<IFocusable>().Where(f => (object)f != this).ToArray();
	}

	void ShowConfirmRemove(bool show = true)
	{
		_confirmRemovePanel.gameObject.SetActive(show);
	}

	// Use this for initialization
	void Start()
	{
		ShowConfirmRemove(false);
	}

	// Update is called once per frame
	void Update()
	{

	}
}
