using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

public class MultiSlider : MonoBehaviour, IFocusable
{
	public enum SliderDirection { Horizontal, Verical }

	public SliderDirection Direction = SliderDirection.Horizontal;
	public float HandlePosition = 0.5f;

	public event System.Action<MultiSliderHandle> HandleSelected;

	public bool HasFocus { get; private set; }
	public MultiSliderHandle ActiveHandle { get; private set; }
	public MultiSliderHandle[] AllHandles { get { return transform.GetComponentsInChildren<MultiSliderHandle>(); } }

	Texture2D _texture;

	public void GiveFocus()
	{
		HasFocus = true;
		GetComponentsInParent<IFocusable>().First(f => (object)f != this).GiveFocus();
	}

	public void RemoveFocus()
	{
		HasFocus = false;
		SelectHandle(null);
	}

	public void SelectHandle(MultiSliderHandle multiSliderHandle)
	{
		if (multiSliderHandle != ActiveHandle)
		{
			ActiveHandle = multiSliderHandle;
			if (HandleSelected != null) HandleSelected(ActiveHandle);
		}
		if (ActiveHandle != null)
		{
			GiveFocus();
		}
	}

	public void RemoveHandle(MultiSliderHandle handle)
	{
		var all = AllHandles;
		if ((all.Length > 1) && (all.Contains(handle)))
		{
			GameObject.Destroy(handle.gameObject);
			if (ActiveHandle == handle)
			{
				SelectHandle(null);
			}
			Repaint();
		}
	}

	struct ColorAndPos
	{
		public Color Color;
		public float Pos;
		public ColorAndPos(Color color, float pos)
		{
			Color = color; Pos = pos;
		}
	}

	public void Repaint()
	{
		var rect = (transform as RectTransform).rect;
		var colorsAndPos = transform.OfType<RectTransform>().Select(t => t.GetComponent<Image>()).Where(i => i != null)
			.OrderBy(i => i.transform.localPosition.x)
			.Select(i => new ColorAndPos(i.color, (i.transform.localPosition.x - rect.xMin) / rect.width))
			.ToList();

		colorsAndPos.Insert(0, new ColorAndPos(colorsAndPos[0].Color, 0));
		colorsAndPos.Add(new ColorAndPos(colorsAndPos.Last().Color, 1));

		Color[] pixels = _texture.GetPixels();
		int x = 0, lastMax = 0;
		for (int i = 1; i < colorsAndPos.Count; ++i)
		{
			//cols[i] = Color.green;// Color.Lerp(cols[i], colors[mip], 0.33f);
			//int max = i * pixels.Length / (colorsAndPos.Count - 1);
			int max = Mathf.RoundToInt(colorsAndPos[i].Pos * pixels.Length);
			for (; x < max; ++x)
			{
				pixels[x] = Color.Lerp(colorsAndPos[i - 1].Color, colorsAndPos[i].Color, ((float)x - lastMax) / (max - lastMax));
			}
			lastMax = max;
		}
		_texture.SetPixels(pixels);

		// actually apply all SetPixels, don't recalculate mip levels
		_texture.Apply(false);
	}

	// Use this for initialization
	void Start()
	{
		var texture = GetComponent<Image>().sprite.texture;
		if (texture.mipmapCount != 1)
		{
			Debug.LogWarning("Texture used for color gradient should have only one mipmap level");
		}

		_texture = texture;
		Debug.LogError("TODO: need to duplicate texture!!");

		Repaint();
	}

	// Update is called once per frame
	void Update()
	{

	}
}
