using UnityEngine;
using UnityEngine.InputSystem;
using System.IO.Ports;

public class ServoController : MonoBehaviour
{
    [Header("Serial")]
    [SerializeField]
    string serialPort = "COM9";
    [SerializeField]
    int baudRate = 9600;

    [Header("Servo")]
    [SerializeField]
    LayerMask selectableLayers;
    [SerializeField]
    float rotationSpeed;
    [SerializeField]
    float maxAngle;

    private bool isMoveable = false;
    private SerialPort serial;

    void Start()
    {
        serial = new SerialPort(serialPort, baudRate);
        serial.Open();

        UpdateServoAngle(Vector3.Angle(Vector3.right, transform.up));
    }

    void UpdateServoAngle(float angle)
    {
        if (angle < 0f) angle += 360f;
        int finalAngle = (int)Mathf.Clamp(angle, 0f, 180f);
        serial.WriteLine(finalAngle.ToString());  
    }

    void Update()
    {
        Vector3 mousePosition = Camera.main.ScreenToWorldPoint(Mouse.current.position.ReadValue());
        mousePosition.z = 0f;

        if  (isMoveable)
        {
            Vector3 mouseDirection = (mousePosition - transform.position).normalized;

            float targetAngle = Vector2.SignedAngle(Vector2.up, mouseDirection);
            float clampedAngle = Mathf.Clamp(targetAngle, -maxAngle, maxAngle);

            Vector3 desiredDirection = Quaternion.Euler(0f, 0f, clampedAngle) * Vector3.up;

            Quaternion toRotation = Quaternion.LookRotation(Vector3.forward, desiredDirection);
            transform.rotation = Quaternion.RotateTowards(transform.rotation, toRotation, rotationSpeed * Time.deltaTime);

            UpdateServoAngle(Vector3.Angle(Vector3.right, transform.up)); 
        }

        if (Mouse.current.leftButton.wasPressedThisFrame)
        {
            Collider2D detectCollider = Physics2D.OverlapPoint(mousePosition, selectableLayers);
            if (detectCollider != null)
            {
                isMoveable = detectCollider.gameObject.name == "Arm";
            }
        }

        if (Mouse.current.leftButton.wasReleasedThisFrame)
            isMoveable = false;
    }

    void OnApplicationQuit()
    {
        UpdateServoAngle(90f);
        serial.Close();
    }
}
