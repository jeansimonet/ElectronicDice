using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine.UI.Extensions;



public class Telemetry : MonoBehaviour
{
    public GraphUI GraphPrefab;
    public RectTransform GraphRoot;
    public Button startStop;
    public Text startStopText;

    public const int MaxPoints = 1000;
    public const int GraphMaxTime = 5000; // millis

	Central central;
    List<GraphInstance> graphs;
    int lastSampleTime;
    int messageCount = 0;

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

        // Until we can properly record data, disable
        startStop.enabled = false;
        startStopText.text = "Scan";

        // Wait until central is initialized
        yield return new WaitUntil(() => central.state == CentralState.Idle);

        // Scan for dice
        central.BeginScanForDice((newDie) =>
        {
            central.ConnectToDie(newDie, OnDieConnected, OnDieDisconnected);
        });
    }

    void Update()
    {
    }

    void OnDieConnected(Die die)
    {
        // Register for telemetry events
        die.OnTelemetry += (acc, millis) =>
        {
            float deltaTime = (float)(millis - lastSampleTime) / 1000.0f;
            // Update the graphs
            foreach (var graph in graphs)
            {
                float val = graph.extractValueFunc(acc, deltaTime);
                graph.samples.samples.Add(new Sample() { millis = millis, value = val });
                graph.instance.UpdateGraph();
            }
            messageCount += 1;
            lastSampleTime = millis; 
        };
    }

    void OnDieDisconnected(Die die)
    {

    }

    public void SaveToFile()
    {
        int sampleRate = 8000; //8kHz
        float wavScale = 10.0f;
        central.DisconnectAllDice();

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
}
