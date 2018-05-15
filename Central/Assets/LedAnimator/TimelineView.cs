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
	float _unitWidth = 200; // Width for 1 second
	[SerializeField]
	float _ticksLength = 0.5f;
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
	public int AnimationCount { get { return _colorAnimsRoot.childCount - 1; } }

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
				var colorAnim = CreateAnimation(sprite);
				StartCoroutine(TestCr(colorAnim)); //TODO
				colorAnim.GiveFocus();
				colorAnim.ColorSlider.SelectHandle(colorAnim.ColorSlider.AllHandles[0]);
				Repaint();
			}
		});
	}

	ColorAnimator CreateAnimation(Sprite sprite = null)
	{
		var colorAnim = GameObject.Instantiate<ColorAnimator>(_colorAnimPrefab, _colorAnimsRoot);
		_controls.SetAsLastSibling(); // Keep controls at the bottom
		if (sprite != null)
		{
			colorAnim.SetLedSprite(sprite);
		}
		colorAnim.GotFocus += OnColorAnimatorGotFocus;
		return colorAnim;
	}

	IEnumerator TestCr(ColorAnimator colorAnim)
	{
		yield return null;
		colorAnim.Maximize();
	}

	Animations.RGBAnimation _serializeDataTest;

	public void TestSerialize()
	{
        _serializeDataTest  = Serialize();
		var str = new System.Text.StringBuilder();
		str.AppendLine(_serializeDataTest.duration.ToString());
		foreach (var t in _serializeDataTest.tracks)
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

	public void TestDeserialize()
	{
		if (_serializeDataTest != null)
		{
			Deserialize(_serializeDataTest);
		}
	}

	public Animations.RGBAnimation Serialize()
	{
		return new Animations.RGBAnimation()
		{
			duration = (short)Mathf.RoundToInt(1000 * Duration),
			tracks = GetComponentsInChildren<ColorAnimator>().Select(a => a.Serialize(StartOffset, Unit)).ToArray()
		};
	}

	public void Deserialize(Animations.RGBAnimation data)
	{
		Debug.LogFormat("Deserializing {0} color animations", data.tracks.Length);

		Clear();
		Duration = data.duration / 1000f;
		Repaint();

		foreach (var track in data.tracks)
		{
			var colorAnim = CreateAnimation();
			colorAnim.Deserialize(track, Unit);
		}
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

	void Clear()
	{
		ActiveColorAnimator = null;
		for (int i = _colorAnimsRoot.childCount - 1; i >= 0; --i)
		{
			var child = _colorAnimsRoot.GetChild(i);
			if (child != _controls)
			{
				child.SetParent(null);
				GameObject.Destroy(child.gameObject);
			}
		}
		for (int i = _ticksRoot.childCount - 1; i > 0; --i)
		{
			var child = _ticksRoot.GetChild(i);
			child.SetParent(null);
			GameObject.Destroy(child.gameObject);
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
		int numLines = Mathf.RoundToInt(Duration / _ticksLength);
		float time = 0;
		for (int i = 0; i <= numLines; ++i)
		{
			RectTransform lineTransf;
			if (i < numExistingLines)
			{
				lineTransf = _ticksRoot.GetChild(i) as RectTransform;
			}
			else
			{
				// Duplicate first line
				lineTransf = GameObject.Instantiate(_ticksRoot.GetChild(0).gameObject, _ticksRoot).transform as RectTransform;
			}

			// Update text
			var text = lineTransf.GetComponentInChildren<Text>();
			text.text = time + "s";
			text.fontStyle = (int)time == time ? FontStyle.Bold : FontStyle.Normal;

			// And position
			Vector2 pos = lineTransf.anchoredPosition;
			pos.x = Unit * time;
			lineTransf.anchoredPosition = pos;

			time += _ticksLength;
		}
		for (int t = numLines + 1; t < numExistingLines; ++t)
		{
			GameObject.Destroy(_ticksRoot.GetChild(t).gameObject);
		}
	}

	void Awake()
	{
		Clear();
		_widthPadding = (transform as RectTransform).rect.width - _ticksRoot.rect.width;
		Duration = _minDuration;
		Zoom = _minZoom;
	}

	// Use this for initialization
	void Start()
	{
		Repaint();
	}

	// Update is called once per frame
	void Update()
	{

	}
}
