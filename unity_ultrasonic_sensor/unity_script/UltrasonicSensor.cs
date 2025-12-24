using UnityEngine;
using System.IO.Ports;

public class UltrasonicSensor : MonoBehaviour
{
 
    [Header("Serial")]
    [SerializeField] string portName = "COM9";
    [SerializeField] int baudRate = 9600;
    [SerializeField] int readTimeOut = 50;

    [Header("Object")]
    [SerializeField] Transform obj;
    [SerializeField] float smoothTime = 100f;
    [SerializeField] float maxDistance = 15f;

    [Header("Distance Sensor")]
    public int value;
    public int calibration;
    
    private SerialPort serial;
    private Vector3 refVelocity;
    private Vector3 startPosition;

    void Start()
    {
        serial = new SerialPort(portName, baudRate);
        serial.ReadTimeout = readTimeOut;
        serial.Open();

        startPosition = obj.position;
    }

    void Update()
    {
        if (serial.IsOpen)
            value = int.Parse(serial.ReadLine());

        float magnitude = Mathf.Clamp(value, 0f, calibration);
        float normalized = magnitude / (calibration != 0 ? calibration : 1f);

        Vector3 desiredPosition = obj.position;
        desiredPosition.x = normalized * maxDistance;

        if (obj != null)
            obj.position = Vector3.SmoothDamp(obj.position, startPosition + desiredPosition, ref refVelocity, smoothTime * Time.deltaTime);
    }

    void OnApplicationQuit()
    {
        if (serial != null && serial.IsOpen)
            serial.Close();
    }
}
