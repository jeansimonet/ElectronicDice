using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine.UI.Extensions;



public class Telemetry : MonoBehaviour
{
	public Text MessageCounter;
	public Text MessageSpeed;

    public GraphUI GraphPrefab;
    public RectTransform GraphRoot;

    public const float SCALE_2G = 2.0f;
    public const float SCALE_4G = 4.0f;
    public const float SCALE_8G = 8.0f;
    public const int MaxPoints = 1000;
    public const int GraphMaxTime = 5000; // millis

	Central central;
	float scale = SCALE_8G;

    List<GraphInstance> graphs;
    int lastSampleTime;

    public struct Sample
    {
        public int millis;
        public float value;
    }

    public class Samples
    {
        public List<Sample> samples = new List<Sample>();
    }

    class GraphInstance
    {
        public string name;
        public GraphUI instance;
        public Samples samples;
        public System.Func<Vector3, float, float> extractValueFunc;
    }

	void Awake()
	{
		central = GetComponent<Central>();
        graphs = new List<GraphInstance>();
    }

    class Die
	{
		public Die(string name, string address)
		{
			this.name = name;
			this.address = address;
		}

		public string name
		{
			get;
			private set;
		}
		public string address
		{
			get;
			set;
		}
	}

    void AddGraph(System.Func<Vector3, float, float> func, float minY, float maxY, Color color, string graphName)
    {
        // Create the samples
        var samples = new Samples();

        // Instantiate a ui object!
        var ui = GameObject.Instantiate<GraphUI>(GraphPrefab);
        ui.transform.SetParent(GraphRoot, false);
        ui.transform.localScale = Vector3.one;
        ui.transform.localPosition = Vector3.zero;
        ui.transform.localRotation = Quaternion.identity;

        var rectX = ui.GetComponent<RectTransform>();
        rectX.offsetMax = new Vector2(0.0f, -150.0f * graphs.Count);
        rectX.offsetMin = new Vector2(0.0f, -150.0f * graphs.Count - 300.0f);

        // set the scales
        ui.minY = minY;
        ui.maxY = maxY;
        ui.timeSpanMillis = GraphMaxTime;
        ui.color = color;
        ui.samples = samples;

        // And store it
        graphs.Add(new GraphInstance()
        {
            name = graphName,
            instance = ui,
            samples = samples,
            extractValueFunc = func
        });
    }

    class LowPass
    {
        public float param = 1.0f;
        public float value;

        public float Update(float val, float dt)
        {
            float p = param * dt;
            value = value * (1.0f - p) + val * p;
            return value;
        }
    }

    IEnumerator Start()
	{
        // Add our telemetry graphs

        // Graph the magnitude and euler angles
        AddGraph((acc, dt) => acc.x, -4, 4, Color.red, "X");
        AddGraph((acc, dt) => acc.y, -4, 4, Color.green, "Y");
        AddGraph((acc, dt) => acc.z, -4, 4, Color.blue, "Z");
        AddGraph((acc, dt) => acc.magnitude, -4, 4, Color.yellow, "Mag");

        //LowPass magLP = new LowPass() { value = 1.0f, param = 20.0f };
        //AddGraph((acc, dt) => magLP.Update(acc.magnitude, dt), -4, 4, Color.magenta);
        //AddGraph((acc, dt) => (acc.x - lastAcc.x) / dt, -100, 100, Color.magenta);
        //AddGraph((acc, dt) => (acc.y - lastAcc.y) / dt, -100, 100, Color.cyan);
        //AddGraph((acc, dt) => (acc.z - lastAcc.z) / dt, -100, 100, Color.gray);
        //AddGraph((acc, dt) => Quaternion.LookRotation(acc).eulerAngles.y, 0, 360, Color.black);
        //AddGraph((acc, dt) => Quaternion.LookRotation(acc).eulerAngles.x, 0, 360, Color.cyan);

        // Initialize Central
        yield return central.InitializeCr();

		// Our running list of die
		Die die = null;
		int messageCount = 0;

		yield return central.ScanForDevicesCr((na) =>
		{
			var firstDie = na.FirstOrDefault();
			if (firstDie != null)
			{
				die = new Die(firstDie.name, firstDie.address);
			}
		});

		if (die != null)
		{
			float startTime = Time.time;
			yield return central.ConnectToDeviceCr(die.address,
				data => {
					var message = DieMessages.FromByteArray(data);
					if (message is DieMessageAcc)
					{
						var telem = (DieMessageAcc)message;
						float currentTime = Time.time;
						float msgPerSec = messageCount / (currentTime - startTime);

						MessageCounter.text = messageCount.ToString();
						MessageSpeed.text = msgPerSec.ToString("F2");

                        for (int i = 0; i < 2; ++i)
                        {
                            // Compute actual accelerometer readings (in Gs)
                            float cx = (float)telem.data[i].X / (float)(1 << 11) * (float)(scale);
                            float cy = (float)telem.data[i].Y / (float)(1 << 11) * (float)(scale);
                            float cz = (float)telem.data[i].Z / (float)(1 << 11) * (float)(scale);

                            Vector3 acc = new Vector3(cx, cy, cz);

                            // Update the graphs
                            foreach (var graph in graphs)
                            {
                                float val = graph.extractValueFunc(acc, (float)telem.data[i].DeltaTime / 1000.0f);
                                graph.samples.samples.Add(new Sample() { millis = lastSampleTime, value = val });
                                graph.instance.UpdateGraph();
                            }

                            lastSampleTime += telem.data[i].DeltaTime;
                        }
                        messageCount += 1;
                    }
                },
				() => {
					Debug.Log("Lost connection to " + die.name);
				});
		}
		else
		{
			Debug.Log("Could not find any die");
		}
	}

    public void SaveToFile()
    {
        int sampleRate = 8000; //8kHz
        float wavScale = 10.0f;
        central.DisconnectAllDevices();

        foreach (var graph in graphs)
        {
            // How many samples do we want to use?
            var vals = graph.samples.samples;
            int start = vals.First().millis;
            int end = vals.Last().millis;
            int millis = end - start;
            int sampleCount = (millis + 999) * sampleRate / 1000; // To ensure rounding up!

            // Create our audio data
            float[] samples = new float[sampleCount];
            for (int i = 0; i < vals.Count-1; ++i)
            {
                // Figure out where to place the sample
                int indexStart = (vals[i].millis - start) * sampleRate / 1000;
                int indexEnd = (vals[i + 1].millis - start) * sampleRate / 1000;
                for (int j = indexStart; j < indexEnd; ++j)
                {
                    float pct = (float)(j - indexStart) / (indexEnd - indexStart);
                    samples[j] = Mathf.Lerp(vals[i].value, vals[i+1].value, pct) / wavScale;
                }
            }

            // Last sample
            int indexLast = (vals.Last().millis - start) * sampleRate / 1000;
            for (int j = indexLast; j < samples.Length; ++j)
            {
                samples[j] = vals.Last().value / wavScale;
            }

            // Create the audio clip
            var clip = AudioClip.Create(graph.name, sampleCount, 1, sampleRate, false);
            clip.SetData(samples, 0);
            SavWav.Save(graph.name, clip);
        }
    }

    // Update is called once per frame
    void Update () {
		
	}
}
