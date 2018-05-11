using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

public class TimelineView : MonoBehaviour
{
	[SerializeField]
	float _minDuration = 1;
	[SerializeField]
	int _minZoom = 1;
	[SerializeField]
	int _maxZoom = 10;
	[SerializeField]
	float _unitWidth = 500; // Width for 1 second
	[SerializeField]
	RectTransform _ticksRoot = null;
	[SerializeField]
	RectTransform _colorAnimsRoot = null;
	[SerializeField]
	RectTransform _controls = null;
	[SerializeField]
	ColorAnimator _colorAnimPrefab = null;

	float _widthPadding;

	public float Duration { get; private set; }
	public int Zoom { get; private set; }
	public float Unit { get { return _unitWidth * Zoom; } }
	public float StartOffset { get { return transform.InverseTransformPoint(_ticksRoot.transform.TransformPoint(_ticksRoot.rect.xMin, 0, 0)).x; } }

	public ColorAnimator ActiveColorAnimator { get; private set; }

	public void ModifyDuration(float signedOffset)
	{
		Duration = Mathf.Max(_minDuration, Duration + signedOffset);
		Repaint();
	}

	public void ModifyZoom(int signedOffset)
	{
		Zoom = Mathf.Clamp(Zoom + signedOffset, _minZoom, _maxZoom);
		Repaint();
	}

	public void AddAnim()
	{
		LedSelector.Instance.PickLed(sprite =>
		{
			if (sprite != null)
			{
				var colorAnim = GameObject.Instantiate<ColorAnimator>(_colorAnimPrefab, _colorAnimsRoot);
				_controls.SetAsLastSibling(); // Keep controls at the bottom
				colorAnim.SetLedSprite(sprite);
				colorAnim.GotFocus += OnColorAnimatorGotFocus;
				colorAnim.GiveFocus();
				colorAnim.ColorSlider.SelectHandle(colorAnim.ColorSlider.AllHandles[0]);
				Repaint();
			}
		});
	}

	public void RemoveCurrentAnim()
	{
		if (ActiveColorAnimator != null)
		{
			GameObject.Destroy(ActiveColorAnimator.gameObject);
			ActiveColorAnimator = null;
		}
	}

	public void TestSerialize()
	{
		var a = Serialize();
		var str = new System.Text.StringBuilder();
		str.AppendLine(a.duration.ToString());
		foreach (var t in a.tracks)
		{
			str.Append(" * ");
			str.Append(t.startTime);
			str.Append(", ");
			str.Append(t.duration);
			str.Append(", ");
			str.Append(t.ledIndex);
			str.AppendLine();
			foreach (var k in t.keyframes)
			{
				str.Append("    * ");
				str.Append(k.time);
				str.Append(", (");
				str.Append(k.red);
				str.Append(", ");
				str.Append(k.green);
				str.Append(", ");
				str.Append(k.blue);
				str.Append(")");
				str.AppendLine();
			}

		}
		Debug.Log(str.ToString());
	}

	public Animations.RGBAnimation Serialize()
	{
		return new Animations.RGBAnimation()
		{
			duration = (short)Mathf.RoundToInt(1000 * Duration),
			tracks = GetComponentsInChildren<ColorAnimator>().Select(a => a.Serialize(StartOffset, Unit)).ToArray()
		};
	}

	void OnColorAnimatorGotFocus(ColorAnimator colorAnim)
	{
		if (ActiveColorAnimator != colorAnim)
		{
			if (ActiveColorAnimator != null)
			{
				ActiveColorAnimator.RemoveFocus();
			}
			ActiveColorAnimator = colorAnim;
		}
	}

	void Repaint()
	{
		var rectTransf = transform as RectTransform;

		// Update width
		var size = rectTransf.sizeDelta;
		size.x = _widthPadding + Unit * Duration;
		size.y = 49 + 10 + _colorAnimsRoot.childCount * 80 + (_colorAnimsRoot.childCount - 1) * 10; //TODO
		rectTransf.sizeDelta = size;

		// Update vertical lines
		int numExistingLines = _ticksRoot.childCount;
		for (int t = 0; t <= Duration; ++t)
		{
			RectTransform lineTransf;
			if (t < numExistingLines)
			{
				lineTransf = _ticksRoot.GetChild(t) as RectTransform;
			}
			else
			{
				// Duplicate first line
				lineTransf = GameObject.Instantiate(_ticksRoot.GetChild(0).gameObject, _ticksRoot).transform as RectTransform;
			}

			// Update text
			lineTransf.GetComponentInChildren<Text>().text = t + "s";

			// And position
			Vector2 pos = lineTransf.anchoredPosition;
			pos.x = Unit * t;
			lineTransf.anchoredPosition = pos;
		}
		for (int t = (int)Duration + 1; t < numExistingLines; ++t)
		{
			GameObject.Destroy(_ticksRoot.GetChild(t).gameObject);
		}
	}

	// Use this for initialization
	void Start()
	{
		foreach (Transform child in _colorAnimsRoot)
		{
			if (child != _controls)
			{
				child.SetParent(null);
				GameObject.Destroy(child.gameObject);
			}
		}

		_widthPadding = (transform as RectTransform).rect.width - _ticksRoot.rect.width;
		Duration = _minDuration;
		Zoom = _minZoom;
		Repaint();
	}

	// Update is called once per frame
	void Update()
	{

	}
}
